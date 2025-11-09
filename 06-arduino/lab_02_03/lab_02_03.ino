int ledPins[] = {7, 8, 9, 10};
int l = 0;

unsigned long previousMillis = 0;
const unsigned long interval = 1000; // 1 sekunda

const int buttonPin = 2;
int button1 = 0;

void reset() {
  l = 0;
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
}

void setup() {
  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  unsigned long currentMillis = millis();

  debounce();
  if (button1) {
    reset();
    button1 = 0;
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
