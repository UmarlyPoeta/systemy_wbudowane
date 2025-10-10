// Program mierzy czas utrzymywania sie stanu wysokiego na linii 0 portu A
// ("szerokosc" impulsu - czas pomiedzy kolejnymi zboczami: rosnacym i opadajacym).
// Opis trybu Input Caputre ukladu licznikowego - Reference Manual, rozdzial 18.3.5.
//
// Zadania obowiazkowe:
// 1. Dokonczyc konfiguracje timera TIM2,
// 2. Ustalic rozdzielczosc i zakres pomiaru (2a) i wyskalowac wynik (2b),
// 3. Sprawdzic poprawnosc wynikow porownujac zmierzony czas:
//
// - nacisniecia przycisku ze wskazaniem stopera/zegarka,
// - opcjonalnie: z szerokoscia impulsu pochodzacego z generatora wzorcowego.

#include <stdio.h>
#include "stm32f4xx.h"
#include "GLCD.h"

#define __FI       1
  #define __FONT_WIDTH  16
  #define __FONT_HEIGHT 24

char tekst[20];

int main(void){ 

// --- Inicjalizacja i konfiguracja wyswietlacza LCD ---

	GLCD_Initialize(); 
	GLCD_Clear(White);
	GLCD_SetBackColor(White);
  GLCD_SetTextColor(Black);

// --- Konfiguracja peryferiow ---

  RCC->AHB1ENR |= (1<<0);						       		// Wlacz "zegar" dla GPIO A.
	RCC->APB1ENR |= (1<<0);   									// Wlacz "zegar" dla TIM2.

// Ustaw tryb pracy linii A0 - Alternate Function 1 - wejscie kanalu 1. timera TIM2.	
// Opis rejestru GPIOx Mode Register (GPIOx->MODER) - Reference Manual, rozdzial 8.4.1.
	
	GPIOA->MODER |= (2<<2*0);										// Linia 0 portu A - ALTERNATE FUNCTION AF1

	GPIOA->AFR[0] |= (1<<4*0);

// --- 2a --- 
// Ustal zakres pomiarowy i rozdzielczosc pomiaru.

	TIM2->PSC	=  15999;															// Preskaler 16-bitowy, dzieli przez wartosc 1 wieksza niz wpisana.

	TIM2->ARR = 65																// Zakres zliczania [0,ARR].

// Konfiguracja timera TIM2 w trybie Input Capture (kanal 1).

	TIM2->CCMR1 |= 1; 													// CC1S = 01 - Capture Compare ch. 1 jako wejscie, linia T1.

// --- 1 ---
// Ustaw sposob sterowania licznikiem (Slave Mode):
// - (SMS) tryb zerowania (reset) w chwili nadejscia zbocza rosnacego,
// - (TS) wejscie synchronizacji licznika TI1 Edge Detector lub Filtered Timer Input 1.
// Opis rejestru TIMx Slave Mode Control Register (TIMx->SMCR) - Reference Manual, rozdzial 18.4.3.

	TIM2->SMCR = (4 << 0) | (4 << 5);

// Wlacz tryb Capture kanalu 1. - zapisu wartosci chwilowej licznika CNT do rejestru CCR1.
// oraz ustaw reakcje wejscia sterujacego praca timera na oba zbocza: rosnace i opadajace.
// Opis rejestru TIMx Capture/Compare Enable Register (TIMx->CCER) - Reference Manual, rozdzial 18.4.9.

	TIM2->CCER = (3 << 0) | (1 << 3);

	TIM2->EGR |= 1;															// Reinicjalizacja licznika, przeladowanie rejestrow.
  TIM2->CR1 |= 1;															// Wlacz licznik.

	for(;;) {

// Program glowny: 		
// Wyswietl odpowiedni komunikat gdy trwa pomiar. Po jego zakonczeniu - zmierzony czas.

			if (GPIOA->IDR & 1) GLCD_DisplayString(1, 0, __FI,"Pomiar!             ");
			else {

// --- 2b --- 
// Odczytaj i wyskaluj (do dziesiatych czesci sekundy)
// wartosc z rejestru Caputer/Compare kanalu 1 timera 2 (CCR1).

				sprintf(tekst,"Czas = %u %cs      ", TIM2->CCR1, 0x90);
				GLCD_DisplayString(1, 0, __FI,tekst);
			}
	}
}
