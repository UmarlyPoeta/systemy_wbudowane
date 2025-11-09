int ledPins[] = {7, 8, 9, 10};
int l = 0;

unsigned long previousMillis = 0;
const unsigned long interval = 1000; // 1 sekunda

const int buttonPin = 2;
int button1 = 0;
int new_war = 0;
int button_war_1 = 0;
int button_war_2 = 0;


const int p1 = 0;
const int p2 = 1;

void reset() {
  l = 0;
}

void zmien_wart() {
  l = 0;

  int i = 0;
  while(i < 4) {
      
      while (!button_war_1 || !button_war_2) {
        debounce();
      }
      if (button_war_1) { l ^= (button_war_1 << i);}
      if (button_war_2) { l ^= (button_war_2 << i);}
      i++;
  }  
  
}


void debounce() {
  static byte lastState = HIGH;
  static unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 50;

  int reading = digitalRead(buttonPin);

  if (reading != lastState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading == LOW) {
      button1 = 1;
    }
  }

  lastState = reading;

  
  int r2 = digitalRead(p2);
  static unsigned long lastDebounceTime2 = 0;
  const unsigned long debounceDelay2 = 50;
  

  static byte lasts2 = HIGH;

  if (r2 != lasts2) {
    lastDebounceTime2 = millis();
  }

  if ((millis() - lastDebounceTime2) > debounceDelay2) {
    if (r2 == LOW) { new_war = 1;}
    }


  if (new_war) {
    int r2 = digitalRead(p2);
    static unsigned long lastDebounceTime2 = 0;
    const unsigned long debounceDelay2 = 50;
  

    static byte lasts2 = HIGH;

    if (r2 != lasts2) {
      lastDebounceTime2 = millis();
    }
  
    if ((millis() - lastDebounceTime2) > debounceDelay2) {
      if (r2 == LOW) { button_war_1 = 1;}
      }


        int r1 = digitalRead(p1);
    static unsigned long lastDebounceTime1 = 0;
    const unsigned long debounceDelay1 = 50;
  

    static byte lasts1 = HIGH;

    if (r1 != lasts1) {
      lastDebounceTime1 = millis();
    }
  
    if ((millis() - lastDebounceTime1) > debounceDelay1) {
      if (r1 == LOW) { button_war_2 = 1;}
      }
      
  }
 
}

void setup() {
  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(p1, INPUT_PULLUP);
  pinMode(p2, INPUT_PULLUP);
}

void loop() {
  unsigned long currentMillis = millis();

  debounce();
  if (button1) {
    reset();
    button1 = 0;
  }
  if (new_war) {
    zmien_wart();
    new_war = 0;  
  }

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    l++;
    if (l > 15) l = 0;

    for (int i = 0; i < 4; i++) {
      int bitValue = (l >> i) & 1;
      digitalWrite(ledPins[i], bitValue);
    }
  }
}
