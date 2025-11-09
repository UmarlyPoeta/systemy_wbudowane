void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(13, OUTPUT);

}
int l = 1;
void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < l; i++) {
   digitalWrite(13, HIGH);
   delay(250);
   digitalWrite(13, LOW);
   delay(250); 
   }
  delay(1000);
  l++;
}
