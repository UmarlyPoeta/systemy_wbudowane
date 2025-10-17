// Priorytety przerwan (zewnetrzengo i timera).
//
// Mikrokontroler wykonuje cztery "zadania":
//
// 1. ISR* przerwania zewnetrznego (EXTI) - wywolywana nacisnieciem USER Button (niebieski klawisz).
//    Na czas zwarcia stykow zablokowany jest powrot z procedury: wymuszenie chwilowego "zawieszenia sie" ISR.
//
// 2. i 3. ISR wywolywane co 0.5 s przepelnieniami timerow TIM6 i TIM7, odpowiednio:
//    zmiana stanow linii G13 (zielona LED) i G14 (czerwona LED) na przeciwne,
//    oraz inkrementacja zmiennych int6 oraz int7.
//
// 4. Main - drukowanie wartosci zmiennych int6 oraz int7 na LCD.
//
// *ISR - Interrupt Service Routine - procedura obslugi przerwania.
//
// Do zrobienia:
// 1. Wlaczyc zglaszanie i przyjmowanie przerwania zewnetrznego EXTI - z linii 0 portu A.
// -> Sprawdzic zachowanie ukladu. Ktore zadania wykonuja sie nieprzerwanie, a ktore sa przerywane?
// 2. Ustawic priorytety przerwan tak, aby ISR timera TIM7 nigdy nie zostala zablokowana.
// -> Sprawdzic zachowanie ukladu.
// 3. Zadanie dodatkowe - uruchomic watchdoga - dobrac jego parametry i miejsce jego przeladowywania tak,
// aby przyblokowanie wykonywania jakiejkolwiek czesci programu na ok. cztery sekundy wywolywalo reset CPU.
// -> Sprawdzic zachowanie ukladu.

#include <stdio.h>
#include "stm32f4xx.h"
#include "GLCD.h"

uint32_t int6, int7;

char tekst[20];

int main ()  {

// --- 3 ---
// --- Konfiguracja watchdoga ---
// Opis - rozdzial 21 w Reference Manual.

	IWDG->KR = 0x5555;													// Odblokowanie rejestrow konfig.
	IWDG->PR = 3;																// Preskaler /32
	IWDG->KR = 0xCCCC;													// Wlacz watchdoga.

// Klucz przeladowujacy watchdoga:
//		IWDG->KR = 0xAAAA;
	
// --- Inicjalizacja i konfiguracja wyswietlacza LCD ---

  GLCD_Initialize();
  GLCD_Clear(White);

// --- Konfiguracja pozostalych peryferiow ---

  RCC->AHB1ENR |= ((1<<6) | (1<<0));					// Wlacz "zegar" dla GPIO A i G.
  RCC->APB1ENR |= (1<<5) |(1<<4);							// Wlacz "zegar" timerow TIM6 i TIM7.
  GPIOG->MODER |= (1<<(2*13)) | (1<<(2*14));	// Ustaw linie G13 i G14 jako wyjscia.

	TIM6->PSC = 15999;				// Preskaler 16-bitowy, dzieli przez wartosc 1 wieksza niz wpisana.
	TIM6->ARR = 499;					// Pojemnosc licznika - "n" stanow: od zera do n-1 wlacznie.
  TIM6->DIER = 1;						// DMA/IRQ Enable Register - enable IRQ on update.	
  TIM6->CR1 |=1;						// Wlacz timer.

	TIM7->PSC =	15999;
  TIM7->ARR =	499;
  TIM7->DIER = 1;
  TIM7->CR1	|= 1;

// --- 1b ---
// Skonfiguruj przyjowanie przerwan zewnetrznych
// z linii A0 (EXTI0, zbocze rosnace 0->1, niebieski przycisk).
// Opis - rozdzial 12.2 w Reference Manual.
// ---------

	EXTI->IMR = (1<<0);						// "odmaskuj"/wlacz linie EXTI0
	EXTI->RTSR = (1<<0);						// ustaw reakcje na zbocze rosnace 0->1

// --- 2 ---
// Ustaw priorytety przerwan:
// timera TIM7 i zewnetrzengo EXTI0 tak, aby obsluga
// przerwania timera 7 nie byla nigdy blokowana.
// Opis w rozdziale 4.3.7. Programming Manual.
// Priorytet n-tego przerwania ustawia sie w rejestrze IP[n].

	NVIC->IP[6] = 0x40;
	NVIC->IP[54] = 0x80;
	NVIC->IP[55] = 0x80;

// --- 1a ---
// Wlacz linie przerwan: zewnetrzne EXTI0 i timera TIM7
// w kontrolerze przerwan (NVIC).
// Tablica wektorow przerwan - Table 63. Reference Manual, kolumna "posistion".
// Kazdy rejestr ISER[n] ma 32 bity.
// ISER[0] obsluguje przerwania od 0 do 31, ISER[1] - od 32 do 63 itd.

	NVIC->ISER[0] = (1 << 6);
	NVIC->ISER[1] = (3 << (54 - 32));

//==============================================
// --- Petla glowna ---

	for(;;){
		
		GLCD_SetBackColor(Green);
		GLCD_SetTextColor(Black);		
		sprintf(tekst,"int6 = %u",int6);
		GLCD_DisplayString(0, 0, tekst);

		GLCD_SetBackColor(Red);
		GLCD_SetTextColor(White);		
		sprintf(tekst,"int7 = %u",int7);
		GLCD_DisplayString(2, 0, tekst);
		IWDG->KR = 0xAAAA;
	}
}

//==============================================
// Procedura obslugi przerwania (ISR) timera TIM6.

void TIM6_DAC_IRQHandler(void) {

// Opis zachowania Update Interrupt Flag w rozdzialach 20.3.2 i 20.4.4 Reference Manual.
	
	TIM6->SR &= ~1;
	int6++;
	GPIOG->ODR ^= (1<<13);		// Zmien stan zielonej LED.
}

//==============================================
// Procedura obslugi przerwania (ISR) timera TIM7.

void TIM7_IRQHandler(void) { 	 
	
	TIM7->SR &= ~1;
	int7++;
	GPIOG->ODR ^= (1<<14);
}

//==============================================	
// Procedura obslugi przerwania (ISR) zewnetrzengo EXTI0

void EXTI0_IRQHandler(void) {

// Opis zachowania flag w Pending Request Register: 12.2.2 i 12.3.6 w Reference Manual.

	EXTI->PR |= 1;																//Skasuj Interrupt Pending Request.

// PRZYBLOKUJ dopoki niebieski klawisz jest nacisniety.
	
	while(GPIOA->IDR & 1);
	
}
