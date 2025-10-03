#include <stdio.h>
#include "stm32f4xx.h"

int main ()  { 

// --- konfiguracja peryferiow ---

    RCC->AHB1ENR |= ((1<<6) | (1<<0));					// wlacz "zegar" dla GPIO A i G
    RCC->APB1ENR |= (1<<5);											// wlacz "zegar" dla TIM7

    GPIOG->MODER |= (1<<(2*13)) | (1<<(2*14));				// linie G13 i G14 jako wyjscia

    NVIC->ISER[1] |= 1<<23;						// wlacz linie przerwania TIM7


    TIM7->PSC	= 15999;							// preskaler (/16000)
    TIM7->ARR = 499;								// pojemnosc - 500 stanow (0...499)
    TIM7->DIER   |= 1;							// DMA/IRQ Enable Register - enable IRQ on update 
    TIM7->CR1    |= 1;							// wlacz timer 


// --- petla glowna ---

    for(;;) {

// Jesli linia 0 portu A jest wstanie wysokim - wymus stan wysoki linii 13 porttu G.
// W przeciwnym przypadku wymus stan niski. Operacje atomowe.

    (GPIOA->IDR & 1) ? (	GPIOG->BSRR = (1<<13)) : (GPIOG->BSRR = (1<<29));
    }
}

// --- procedura obslugi przerwania TIM7 ---

void TIM7_IRQHandler(void) {

    TIM7->SR &= ~1;										// skasuj UPDATE INTERRUPT FLAG
    GPIOG->ODR ^= (1<<14);						// zmien stan 14 linii portu G na przeciwny

}
