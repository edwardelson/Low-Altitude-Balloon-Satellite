void setup()
{
  Serial.begin(9600);
}

void loop()
{
  Serial.print(analogRead(A6));
  Serial.print(",");
  Serial.println(analogRead(A7));
  
  delay(1000);
}
