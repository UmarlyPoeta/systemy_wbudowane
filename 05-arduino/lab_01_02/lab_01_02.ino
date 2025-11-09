void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

char buff[100];

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
      Serial.readBytes(buff, 10);
      Serial.println(buff);
    }
  delay(100); 
}
