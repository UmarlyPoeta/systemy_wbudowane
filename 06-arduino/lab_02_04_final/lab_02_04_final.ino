const int ledPins[] = {7, 8, 9, 10};
const int buttonReset = 2;
const int buttonSetMode = 3;
const int buttonBit = 4;

int counter = 0;
bool setMode = false;
unsigned long previousMillis = 0;
const unsigned long interval = 1000;

bool readButton(int pin) {
  static unsigned long lastTime[10];
  static bool lastState[10];
  bool reading = digitalRead(pin);
  if (reading != lastState[pin]) lastTime[pin] = millis();
  if ((millis() - lastTime[pin]) > 50) {
    lastState[pin] = reading;
    if (reading == LOW) return true;
  }
  return false;
}

void resetCounter() {
  counter = 0;
}

void setStartValue() {
  int value = 0;
  for (int i = 0; i < 4; i++) {
    bool bitSet = false;
    while (!bitSet) {
      if (readButton(buttonBit)) {
        bitSet = true;
        value |= (1 << i);
        delay(300);
      }
      if (readButton(buttonSetMode)) {
        bitSet = true;
        delay(300);
      }
    }
  }
  counter = value;
  setMode = false;
}

void showValue(int val) {
  for (int i = 0; i < 4; i++) digitalWrite(ledPins[i], (val >> i) & 1);
}

void setup() {
  for (int i = 0; i < 4; i++) pinMode(ledPins[i], OUTPUT);
  pinMode(buttonReset, INPUT_PULLUP);
  pinMode(buttonSetMode, INPUT_PULLUP);
  pinMode(buttonBit, INPUT_PULLUP);
}

void loop() {
  if (readButton(buttonReset)) resetCounter();
  if (readButton(buttonSetMode)) setMode = true;
  if (setMode) setStartValue();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    counter++;
    if (counter > 15) counter = 0;
    showValue(counter);
  }
}
