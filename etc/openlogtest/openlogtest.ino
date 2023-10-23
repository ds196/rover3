void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  Serial1.begin(9600); //OpenLog

  //delay(1000);
  /*while(!Serial1.available()) {
    delay(10);
  }*/
  delay(5000);
    Serial1.print("this is a test write.\n");

}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}
