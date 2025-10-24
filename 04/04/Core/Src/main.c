// Program przesyla dane z przetwornika AC do pamieci RAM kanalem DMA.
//
// Do wejscia przetwornika AC podlaczony jest wewnetrzny (w strukturze mikrokontrolera) czujnik temperatury.
//
// Zasada dzialania programu:
//
// 1. Cykliczne przepelnienie timera TIM3 generuje sygnal TRGO (Trigger Output) inicjujacy konwersje AC.
// 2. Po zakonczonej konwersji przetwornik AC zglasza zadanie (request) transferu danych kanalem DMA.
// 3. Uklad DMA przesyla dane z Data Register (ADC1->DR) przetwornika AC do pierwszego (i kolejnych)
//    elementu(�w) tablicy "buff" (w pamieci RAM).
//
// Zadania.
//
// 1. Skonfigurowac uklad DMA. Ustawic adresy odczytu i zapisu danych, uruchomic program w trybie
//    pojedynczych transferow sampli z przetwornika ADC1 do kolejnych elementow buff[i] (i = 0 ... 7),
//		(jedna konwersja -> jedna probka -> jeden transfer do pamieci pod odpowiednio inkrementowany adres).
//
//		-> Wyswietlic 8 kolejnych odczytow temperatury. Zaobserowac zachowanie ukladu.
//
// 2. Wlaczyc i ustawic kolejke FIFO w ukladzie DMA i wykorzystac transfery seriami
//    (zgromadzenie np. 8 sampli 12(16) bitowych w buforze FIFO -> jeden transfer calosci do pamieci).
//
//		-> Wyswietlic wszystkie temperatury - analogicznie jak w poprzednim punkcie. Co sie zmienilo?
//
// 3. Obliczyc srednia arytmetyczna z 8 ostatnich odczytow i ja wypisac -> sprawdzic wartosc.
//
// 4. Opcjonalnie: zmierzyc rzeczwista wartosc napiecia odniesienia Vref przetwornika ADC1 (domyslnie Vref = 3.0 V).
//		Skorygowac skalowanie temperatury, nastepnie sprawdzic wplyw zmiany Vref na poprawnosc pomiarow.

#include <stdio.h>
#include "stm32f4xx.h"
#include "GLCD.h"

// --- 4 ---
// Nalezy sprawdzic i zmierzyc Vref kazdego egzemplarza zestawu Discovery!

#define	Vref 3.0f

#define LSB Vref/4096.0f

float t;
uint32_t i;
uint16_t buff[8];
char str[20];

int main(void){

	GLCD_Initialize();
	GLCD_Clear(Black);                        
	GLCD_SetBackColor(Black);
  GLCD_SetTextColor(White);
	
  RCC->AHB1ENR |= (1<<22); // Wlacz "zegar" DMA2.
	RCC->APB1ENR |= (1<<1);  // Wlacz "zegar" TIM3.
	RCC->APB2ENR |= (1<<8);  // Wlacz "zegar" ADC1.

// --- 1 ---

// Ustawienia DMA (rozdzial 10.5 w Reference Manual, ew. Application Note AN4031).

// Konfiguracja parametrow transferu danych:
	
// 32-bitowy adres urzadzenia peryferyjnego (Peripheral Address Reg.):

DMA2_Stream0->PAR = (uint32_t)&ADC1->DR;
	
// 32-bitowy adres miejsca w pamieci RAM do ktorego beda zapisywane dane (Memory Address Reg.):	

DMA2_Stream0->M0AR = (uint32_t)&buff[0];
	
// Liczba przesylanych danych (probek) w jednej sekwencji:

DMA2_Stream0->NDTR = 8;

// Opis rejestru SxCR - rozdzial 10.5.5 w RM. W szczegolnosci nalezy zwrocic uwage na: 
// kierunek przeplywu danych,
// typy/rozmiar danych, inkrementacje wskaznikow,
// wlaczyc tryb pierscieniowy (circular)!

//	DMA2_Stream0->CR =
DMA2_Stream0 -> CR =
	(0 << 25) |  // Channel 0
    (1 << 10) |  // MINC = 1 (zwiększaj adres w pamięci: buff[0], buff[1]...)
    (0 << 9)  |  // PINC = 0 (adres ADC->DR się nie zmienia)
    (1 << 8)  |  // Circular mode (po buff[7] wróć do buff[0])
    (1 << 5)  |  // PFCTRL = 1 (DMA kontroluje przepływ)
    (1 << 1)  |  // TCIE = 1 (przerwanie po zakończeniu)
    (0 << 6);  
// --- 2 ---

// Ustawic pelna pojemnosc i wlaczyc bufor FIFO, wylaczyc Direct Mode.
// 0pis rejestru SxFCR - rozdzial 10.5.10 w RM.

	DMA2_Stream0->FCR = (1 << 7) |
		(3 << 0) |
		(1 << 2);
	
	DMA2_Stream0->CR |= 1;											// Wlacz DMA.
	
// Konfiguracja przetwornika AC.	
// Opis rejestru CR1 (Control Register 1) - rozdzial 13.13.2 w Reference Manual.

	ADC1->CR1= 0 << 24;													// Rozdzielczosc przetwornika AC.
	ADC1->CR2 = (1<<28)|(8<<24)|(1<<9)|(1<<8);	// External TIM3 TRGO Trigger/wlacz DMA.
	ADC1->SQR1 = 1<<20;													// Pojedyncza konwersja AC.
	ADC1->SQR3 = 18; 														// Kanal 18.	
	ADC->CCR = (1<<23)|(3<<16);									// Wlacz Temp_Sensor; ADC prescaler = /8.
	ADC1->SMPR1 |= 7<<24;												// Sampling time - maks. - 480 taktow.
	ADC1->CR2 |= 1;															// Wlacz ADC1.

// Konfiguracja timera TIM3. Czestotliwosc probkowania 1 Hz:

	TIM3->PSC = 15999;
	TIM3->ARR = 999;
	TIM3->CR2 |= 2<<4;													// W chwili przepelnienia -> TRGO.
	TIM3->CR1 |= 1;															// Wlacz timer.

 for (;;) {

        // Obliczenie temperatury na podstawie ostatniego DMA buff[i]
        float t = ((LSB * (float)buff[i]) - 0.76f) * 400.0f + 25.0f;

        // Wyświetlenie każdej próbki na GLCD
        sprintf(str, "t[%u] = %2.2f %cC", i, t, 0x82);
        GLCD_DisplayString(i, 0, str);

        // Inkrementacja indeksu w buforze kołowym
        i++;
        i &= 0x07; // modulo 8

        // --- Uśrednianie temperatury z 8 ostatnich próbek ---
        float sum = 0;
        for (int j = 0; j < 8; j++)
            sum += ((LSB * (float)buff[j] - 0.76f) * 400.0f + 25.0f);

        float avg = sum / 8.0f;
        sprintf(str, "Avg = %2.2f %cC", avg, 0x82);
        GLCD_DisplayString(9, 0, str);
    }
}

