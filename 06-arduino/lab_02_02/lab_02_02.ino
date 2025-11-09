
int ledPins[] = {7, 8, 9, 10};
int l = 0;
long previousMillis = 0;
long interval = 1000;

void reset() {
  l = 0;
}


void setup() {
  // put your setup code here, to run once:
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  attachInterrupt(0, reset, LOW);
  pinMode(2, INPUT_PULLUP);

  
  
}




void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();


    for (int i = 0; i < 4; i++)
  {
    int bitValue = (l >> i) & 1;

    digitalWrite(ledPins[i], bitValue);
    }


  
  l++;
  delay(1000);
  if (l > 15) {l = 0;}
} 
