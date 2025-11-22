// ==== BIBLIOTEKI ==== //
#include <LiquidCrystal.h>

// ==== STAŁE ==== //
#define LCD_RS 12
#define LCD_E 11
#define LCD_D4 5 
#define LCD_D5 4
#define LCD_D6 3
#define LCD_D7 2
#define LIGHT_TICK A5
// #define LINE1_TEXT "Wszystko lepsze"
// #define LINE2_TEXT "niz Assembler"
#define BTN_HOUR_PIN 8
#define BTN_MINUTE_PIN 9
#define BTN_MODE_PIN 10
#define DELAY_MS 1000UL
#define DAY_SECONDS 86400UL

// ==== OBIEKTY ==== //
LiquidCrystal LCD(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// ==== ZMIENNE ==== //
unsigned long totalSeconds = 0;
unsigned long updateMillis = 0;
unsigned long hourBtnMillis = 0;
unsigned long minuteBtnMillis = 0;
unsigned long blinkMillis = 0;

int alarmHour = 0;
int alarmMinute = 0;
bool alarmActive = false;
bool backlightState = false;

// ==== KONFIGURACJA ==== //
void setup() {
  LCD.begin(16, 2);
  LCD.clear();
  LCD.display();
  pinMode(BTN_HOUR_PIN, INPUT_PULLUP);
  pinMode(BTN_MINUTE_PIN, INPUT_PULLUP);
  pinMode(BTN_MODE_PIN, INPUT_PULLUP);
  pinMode(LIGHT_TICK, OUTPUT);
  digitalWrite(LIGHT_TICK,HIGH);
  Serial.begin(9600);
}

void loop() {
  unsigned long currentMillis = millis();

  int hours = (totalSeconds / 3600) % 24;
  int minutes = (totalSeconds / 60) % 60;

  if (currentMillis >= 2000) alarmActive = (hours == alarmHour && minutes == alarmMinute);

  if (currentMillis - blinkMillis > DELAY_MS) { 
    blinkMillis = currentMillis;
    if (alarmActive) {
      digitalWrite(LIGHT_TICK, !digitalRead(LIGHT_TICK));
    } 
  }

  if (currentMillis - updateMillis > DELAY_MS) { 
    updateMillis = currentMillis;
    totalSeconds = (totalSeconds + 1) % DAY_SECONDS;

    if (digitalRead(BTN_MODE_PIN)) { 
      if (!digitalRead(BTN_HOUR_PIN)) { 
        hourBtnMillis = currentMillis;
        totalSeconds += 3600;
      }
    
      if (!digitalRead(BTN_MINUTE_PIN)) { 
        minuteBtnMillis = currentMillis;
        totalSeconds += 60;
      } 
    } else { 
      if (!digitalRead(BTN_HOUR_PIN)) { 
        hourBtnMillis = currentMillis;
        if (alarmHour < 24) alarmHour += 1;
        else alarmHour = 0;
      }
    
      if (!digitalRead(BTN_MINUTE_PIN)) { 
        minuteBtnMillis = currentMillis;
        if (alarmMinute < 60) alarmMinute += 1;
        else alarmMinute = 0;
      } 
    }
  }

  int seconds = (totalSeconds % 60);

  char buffer1[17];
  sprintf(buffer1, "%02d:%02d:%02d", hours, minutes, seconds);
  LCD.setCursor(0, 0);
  LCD.print(buffer1);

  char buffer2[10];
  sprintf(buffer2, "%02d:%02d", alarmHour, alarmMinute);
  LCD.setCursor(0, 1);
  LCD.print(buffer2);
}
