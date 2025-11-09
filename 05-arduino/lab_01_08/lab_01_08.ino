#define ledPin 13
#define resetPin 3

volatile unsigned int k = 0;

void reset() {
  k = 0;
}

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(resetPin, INPUT_PULLUP);   // przycisk do GND
  attachInterrupt(digitalPinToInterrupt(resetPin), reset, FALLING);
}

void displayInt(int number) {
  for (int i = 0; i < number; i++) {
    digitalWrite(ledPin, HIGH);
    delay(250);
    digitalWrite(ledPin, LOW);
    delay(250);
  }
  delay(500);
}

void loop() {
  k++;
  displayInt(k);
}
