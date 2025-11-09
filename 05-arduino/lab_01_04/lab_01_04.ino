void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
}
int sum = 0;
void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {76876        5454
    int liczba = Serial.parseInt();
    sum = sum + liczba;
    Serial.println("Suma wynosi: ");
    Serial.println(sum);
    }
  delay(300);
}
