void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}


void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
  int liczba = Serial.parseInt();
  Serial.println("Otrzymalem liczbe: ");
  Serial.println(liczba);
  }
  delay(200);
}
