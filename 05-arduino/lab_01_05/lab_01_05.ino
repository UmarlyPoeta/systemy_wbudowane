void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
}
int l = 0;
int w;
int stop = 0; 
void loop() {
  // put your main code here, to run repeatedly:
  if (l > 100) { l = 0;}
  
  if (Serial.available()) {
    
    w = Serial.parseInt();

    if (w == -1) {
       stop = !stop;
      } else if (w == 0) {
      l = 0;  
    } else if (w > 0){
        l = w;
    }
  }
  if (!stop) {
    Serial.println("licznik: ");
    Serial.println(l);
    l++;
    }
  delay(300);
}
