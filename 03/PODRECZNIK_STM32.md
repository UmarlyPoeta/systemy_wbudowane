# PODRĘCZNIK: STM32F429I - Obsługa Przerwań i Timerów

## WSTĘP

Ten dokument to kompletny przewodnik po projekcie `lab3.c` dla mikrokontrolera STM32F429I. Zawiera wyjaśnienie wszystkich koncepcji embedded programming od podstaw, wraz z praktycznymi przykładami.

**Projekt demonstruje:**
- Obsługę timerów (TIM6, TIM7)
- Przerwania zewnętrzne (EXTI)
- Priorytety przerwań
- Watchdog
- GPIO i LCD

---

## CZĘŚĆ 1: PODSTAWY EMBEDDED PROGRAMMING

### Co to jest mikrokontroler?

Mikrokontroler to "komputer w jednym układzie scalonym" zawierający:
- **Procesor** (mózg wykonujący program)
- **Pamięć** (RAM + Flash na program i dane)
- **Peryferia** (timery, GPIO, ADC, UART, itp.)

STM32F429I to mikrokontroler z procesorem ARM Cortex-M4.

### Jak działa program?

```c
#include <stdio.h>
#include "stm32f4xx.h"    // Definicje rejestrów STM32
#include "GLCD.h"         // Biblioteka LCD

uint32_t int6, int7;      // Liczniki przerwań
char tekst[20];           // Bufor na tekst LCD

int main() {
    // Konfiguracja sprzętu...
    // Pętla główna...
}
```

### Kluczowe pojęcia

#### Rejestry
- **Rejestr** = pamięć kontrolująca peryferium
- Dostęp przez `PERYFERIUM->REJESTR = wartość`
- Przykład: `TIM6->PSC = 15999`

#### Bitowe operacje
```c
// Ustawianie bitów
rejestr |= (1<<5);     // Ustaw bit 5
rejestr &= ~(1<<3);    // Wyczyść bit 3
rejestr ^= (1<<7);     // Przełącz bit 7
```

#### Zegary i zasilanie
Każdy peryferium potrzebuje "zegara" żeby działać. Bez niego "śpi".

---

## CZĘŚĆ 2: STRUKTURA PROGRAMU

Program wykonuje **4 zadania jednocześnie**:

1. **TIM6** - co 0.5s miga zieloną LED (PG13) i zwiększa `int6`
2. **TIM7** - co 0.5s miga czerwoną LED (PG14) i zwiększa `int7`
3. **EXTI0** - reaguje na naciśnięcie niebieskiego przycisku (PA0)
4. **Main** - wyświetla liczniki na LCD w pętli głównej

### Architektura systemu

```
┌─────────────────┐
│   PROGRAM MAIN  │ ← Wyświetla na LCD
└─────────┬───────┘
          │
    ┌─────▼─────┐
    │ PRZERWANIA│ ← Reagują natychmiast
    └─────┬─────┘
          │
    ┌─────▼─────┐
    │  PERYFERIA│ ← Timery, GPIO, EXTI
    └─────┬─────┘
          │
    ┌─────▼─────┐
    │ STM32F429I │ ← Mikrokontroler
    └─────────────┘
```

---

## CZĘŚĆ 3: KONFIGURACJA SPRZĘTU

### 1. Watchdog (Strażnik systemu)

```c
// Konfiguracja watchdog - ZAKOMENTOWANA
IWDG->KR = 0x5555;      // Odblokuj konfigurację
IWDG->PR = 1;           // Preskaler /32
IWDG->KR = 0xCCCC;      // Włącz watchdog

// W pętli głównej:
IWDG->KR = 0xAAAA;      // Karm watchdog
```

**Wyjaśnienie:**
- **IWDG** = Independent Watchdog
- Odlicza czas (domyślnie ~4s)
- Jeśli program się zawiesi → resetuje system
- "Karmienie" w pętli głównej = "wszystko OK"

### 2. Zegary i zasilanie

```c
RCC->AHB1ENR |= ((1<<6) | (1<<0));  // GPIO A + G
RCC->APB1ENR |= (1<<5) |(1<<4);     // TIM6 + TIM7
```

**Wyjaśnienie:**
- **RCC** = Reset and Clock Control
- Włącza zasilanie dla peryferiów
- Bez tego peryferia nie działają

### 3. Konfiguracja GPIO

```c
GPIOG->MODER |= (1<<(2*13)) | (1<<(2*14));  // PG13, PG14 wyjścia
```

**Wyjaśnienie:**
- **GPIO** = General Purpose Input/Output
- Każdy pin ma 2 bity w MODER:
  - `00` = wejście
  - `01` = wyjście
  - `10` = funkcja alternatywna
  - `11` = analogowe

### 4. Konfiguracja timerów

```c
// TIM6 - zielona LED
TIM6->PSC = 15999;      // Preskaler /16000
TIM6->ARR = 499;        // Liczy do 499 (500 stanów)
TIM6->DIER = 1;         // Włącz przerwanie
TIM6->CR1 |= 1;         // Uruchom timer

// TIM7 - czerwona LED (identycznie skonfigurowany)
TIM7->PSC = 15999;
TIM7->ARR = 499;
TIM7->DIER = 1;
TIM7->CR1 |= 1;
```

**Wyjaśnienie:**
- **PSC** = Preskaler: dzieli zegar (16MHz → 1000Hz)
- **ARR** = Auto-Reload: wartość maksymalna (1000Hz → 2Hz = 0.5s)
- **DIER** = DMA/Interrupt Enable: włącz przerwanie
- **CR1** = Control Register: uruchom timer

---

## CZĘŚĆ 4: PRZERWANIA

### Co to są przerwania?

**Problem bez przerwań:**
```c
while(1) {
    if (przycisk_wciśnięty) {  // Ciągłe sprawdzanie = marnowanie czasu
        zrób_coś();
    }
}
```

**Rozwiązanie - przerwania:**
Procesor robi swoje, a gdy zdarzenie nastąpi → natychmiast reaguje.

### Funkcje obsługi przerwań (ISR)

```c
void TIM6_DAC_IRQHandler(void) {
    TIM6->SR &= ~1;           // WYCZYŚĆ FLAGĘ!
    int6++;                   // Zrób robotę
    GPIOG->ODR ^= (1<<13);    // Przełącz zieloną LED
}

void TIM7_IRQHandler(void) {
    TIM7->SR &= ~1;           // WYCZYŚĆ FLAGĘ!
    int7++;                   // Zrób robotę
    GPIOG->ODR ^= (1<<14);    // Przełącz czerwoną LED
}

void EXTI0_IRQHandler(void) {
    EXTI->PR |= 1;            // Wyczyść flagę
    while(GPIOA->IDR & 1);    // Czekaj aż przycisk zwolniony
}
```

**Wyjaśnienie rejestrów w ISR:**
- **SR** = Status Register: zawiera flagi zdarzeń
- **ODR** = Output Data Register: steruje pinami wyjściowymi
- **PR** = Pending Register: oczekujące przerwania
- **IDR** = Input Data Register: czyta piny wejściowe

### Problem z EXTI0

Funkcja `EXTI0_IRQHandler` zawiera:
```c
while(GPIOA->IDR & 1);  // PĘTLA WIECZNA!
```

**Co się dzieje:**
1. Naciśniesz przycisk
2. ISR się wykonuje
3. Pętla czeka aż zwolnisz przycisk
4. **Wszystkie inne przerwania są zablokowane!**

### Rozwiązanie: Priorytety przerwań

```c
NVIC->IP[55] = 0x00;    // TIM7 - priorytet 0 (NAJWYŻSZY)
NVIC->IP[6] = 0xFF;     // EXTI0 - priorytet 255 (NAJNIŻSZY)
```

**Wyjaśnienie:**
- **NVIC** = Nested Vectored Interrupt Controller
- Niższa wartość = wyższy priorytet
- Przerwanie o wyższym priorytecie może przerwać niższe

### Włączanie przerwań

```c
// Konfiguracja EXTI
EXTI->IMR = (1<<0);     // Odmaskuj linię 0
EXTI->RTSR = (1<<0);    // Zbocze rosnące (0→1)

// Włącz w NVIC
NVIC->ISER[0] = (1<<6);     // EXTI0 (przerwanie nr 6)
NVIC->ISER[1] = (1<<23);    // TIM7 (przerwanie nr 55)
```

**Wyjaśnienie:**
- **IMR** = Interrupt Mask Register: włącz linie
- **RTSR** = Rising Trigger: reaguj na naciśnięcie
- **ISER** = Interrupt Set-Enable: włącz w kontrolerze

---

## CZĘŚĆ 5: PĘTLA GŁÓWNA

```c
for(;;) {
    // Wyświetl liczniki na LCD
    GLCD_SetBackColor(Green);
    GLCD_SetTextColor(Black);
    sprintf(tekst, "int6 = %u", int6);
    GLCD_DisplayString(0, 0, tekst);

    GLCD_SetBackColor(Red);
    GLCD_SetTextColor(White);
    sprintf(tekst, "int7 = %u", int7);
    GLCD_DisplayString(2, 0, tekst);

    // TUTAJ DODAJ: IWDG->KR = 0xAAAA;  // Karm watchdog
}
```

**Wyjaśnienie:**
- `GLCD_*` = funkcje biblioteki LCD
- `sprintf` = formatuje tekst jak `printf`
- Pętla działa "wiecznie" (aż reset)

---

## CZĘŚĆ 6: WSZYSTKIE REJESTRY - SŁOWNIK

| Rejestr | Pełna nazwa | Co robi |
|---------|-------------|---------|
| `RCC->AHB1ENR` | AHB1 Peripheral Clock Enable | Włącza GPIO A, G |
| `RCC->APB1ENR` | APB1 Peripheral Clock Enable | Włącza TIM6, TIM7 |
| `GPIOG->MODER` | Mode Register | Ustawia piny jako wej/wyj |
| `TIMx->PSC` | Prescaler | Dzieli zegar timera |
| `TIMx->ARR` | Auto-Reload Register | Max wartość licznika |
| `TIMx->DIER` | DMA/Interrupt Enable | Włącz przerwania timera |
| `TIMx->CR1` | Control Register 1 | Uruchamia timer |
| `TIMx->SR` | Status Register | Flagi zdarzeń timera |
| `GPIOG->ODR` | Output Data Register | Steruje pinami wyjściowymi |
| `EXTI->IMR` | Interrupt Mask Register | Włącz linie EXTI |
| `EXTI->RTSR` | Rising Trigger Selection | Zbocze rosnące |
| `EXTI->PR` | Pending Register | Oczekujące przerwania |
| `GPIOA->IDR` | Input Data Register | Czyta piny wejściowe |
| `NVIC->IP[]` | Interrupt Priority | Ustawia priorytety |
| `NVIC->ISER[]` | Interrupt Set-Enable | Włącz przerwania |
| `IWDG->KR` | Key Register | Steruje watchdog |
| `IWDG->PR` | Prescaler Register | Preskaler watchdog |

---

## CZĘŚĆ 7: ĆWICZENIA - DO ZROBIENIA

### Ćwiczenie 1: Włącz EXTI (przycisk)

Odkomentuj i uzupełnij:

```c
// W main(), po konfiguracji timerów:
EXTI->IMR = (1<<0);     // Odmaskuj linię 0
EXTI->RTSR = (1<<0);    // Zbocze rosnące
NVIC->ISER[0] = (1<<6); // Włącz EXTI0 w NVIC
```

**Test:** Naciśnij przycisk - program się "zawiesi" w ISR.

### Ćwiczenie 2: Ustaw priorytety

Odkomentuj i uzupełnij:

```c
NVIC->IP[55] = 0x00;    // TIM7 - najwyższy priorytet
NVIC->IP[6] = 0xFF;     // EXTI0 - najniższy priorytet
NVIC->ISER[1] = (1<<23); // Włącz TIM7 w NVIC
```

**Test:** Naciśnij przycisk - czerwona LED nadal miga!

### Ćwiczenie 3: Włącz watchdog

Odkomentuj konfigurację:

```c
IWDG->KR = 0x5555;
IWDG->PR = 1;
IWDG->KR = 0xCCCC;
```

Dodaj karmienie w pętli głównej:

```c
for(;;) {
    // ... kod LCD ...

    IWDG->KR = 0xAAAA;  // Karm watchdog
}
```

**Test:** Symuluj zawieszenie - usuń karmienie na 5 sekund.

---

## CZĘŚĆ 8: NAJWAŻNIEJSZE ZASADY

### Zasady ogólne
1. **Zawsze czyść flagi przerwań** w ISR
2. **ISR powinny być krótkie** - nie blokować systemu
3. **Używaj priorytetów** żeby ważne rzeczy działały
4. **Watchdog chroni** przed zawieszeniami
5. **Testuj wszystko** - embedded nie ma "restart"

### Bezpieczeństwo kodu
- **Nigdy nie ufaj wejściom** - sprawdzaj zakresy
- **Inicjalizuj wszystko** - undefined behavior jest niebezpieczny
- **Watchdog to podstawa** - system musi być niezawodny

### Debugowanie
- **Używaj debuggera** - Keil ma wbudowany
- **Dodawaj testowe printf** na UART
- **Sprawdzaj flagi rejestrów** w debuggerze

---

## CZĘŚĆ 9: ROZSZERZENIA PROJEKTU

### Możliwe ulepszenia
1. **Debouncing przycisku** - filtracja drgań styków
2. **PWM na LED** - płynne ściemnianie
3. **UART komunikacja** - wysyłanie danych na PC
4. **ADC pomiary** - czujniki analogowe
5. **RTC zegar** - czas rzeczywisty

### Przydatne linki
- [STM32F429 Reference Manual](https://www.st.com/resource/en/reference_manual/dm00031020-stm32f405-415-stm32f407-417-stm32f427-437-and-stm32f429-439-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf)
- [Cortex-M4 Programming Manual](https://developer.arm.com/documentation/dui0553/a/)
- [Keil uVision dokumentacja](https://www.keil.com/support/man/docs/uv4/)

---

## PODSUMOWANIE

Ten projekt uczy fundamentów embedded programming:

✅ **Rejestry i bitowe operacje** - manipulacja sprzętem  
✅ **Przerwania** - natychmiastowa reakcja na zdarzenia  
✅ **Priorytety** - zarządzanie ważnością przerwań  
✅ **Watchdog** - ochrona przed katastrofami  
✅ **Timery** - odmierzanie czasu  
✅ **GPIO** - kontrola pinów  
✅ **Debugging** - znajdowanie i naprawianie błędów  

**Pamiętaj:** Embedded to nie tylko kod - to zrozumienie sprzętu i jego ograniczeń!

---

*Podręcznik stworzony na podstawie analizy projektu lab3.c*  
*STM32F429I Discovery Board, Keil uVision, 2024*