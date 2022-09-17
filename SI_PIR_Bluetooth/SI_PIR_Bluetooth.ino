
String message = "0";
void setup() {
  Serial.begin(9600);    // initialisation de la liaison série
  pinMode(2, INPUT);
  pinMode(12, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
    if (message == "1")
    {
      while (digitalRead(2) == HIGH)
      {
        digitalWrite(12, HIGH);
        delay(3000);
        digitalWrite(12, LOW);
        delay(100);
        Serial.println(digitalRead(2));
      }
    }
    else if (message == "0")
    {
      Serial.println(digitalRead(2));
      digitalWrite(12, LOW);   // éteint la LED
    }
    if (Serial.available() > 0 )   //Si un message a été reçu  faire ceci
    {
      message = Serial.readString(); // lire le message
    }

}
