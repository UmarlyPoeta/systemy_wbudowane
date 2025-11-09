


void reset() {
  l = 0;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);



  
  
}

int ledPins[] = {7, 8, 9, 10};
int l = 0;



void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < 4; i++)
  {
    int bitValue = (l >> i) & 1;

    digitalWrite(ledPins[i], bitValue);
    }

  delay(1000);
  l++;

  if (l > 15) {l = 0;}
} 
