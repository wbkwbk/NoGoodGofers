void setup() {
  Serial.begin(115200);
  pinMode(A6, INPUT_PULLUP);
  pinMode(A7, INPUT_PULLUP);
}

void loop() {
  Serial.print("A6: ");
  Serial.print(digitalRead(A6));
  Serial.print(" | A7: ");
  Serial.println(digitalRead(A7));
  delay(100);
}