/*
 * Created by Club MAKERS
 *
 * Code open-source
 *
 * Institut Polytechnique de Saint-Louis (IPSL)
 */

#include <Keypad.h>                    //bibliothéque Keypad
#include <Servo.h>                     //bibliothéque servomoteur


Servo servo;  
const int buzzer  = A5;                // Connecter le buzzer au PIN A5 de l'Arduino Nano
const int ROW_NUM    = 4;              // Les 4 lignes du Keypad
const int COLUMN_NUM = 4;              // Les 4 colonnes du Keypad
int servoPin = 11;

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','4','7', '0'},
  {'2','5','8', '*'},
  {'3','6','9', '#'},
  {'A','B','C', 'D'}
};

byte pin_rows[ROW_NUM] = {9, 8, 7, 6};           // Connexion des lignes du keypad à l'arduino
byte pin_column[COLUMN_NUM] = {5, 4, 3, 2};      // Connexion des colonnes du keypad à l'arduino

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

const String password_1 = "1234";               // Mot de passe 1
const String password_2 = "5642";               // Mot de passe 2
const String password_3 = "4545";               // Mot de passe 3
String input_password;

void setup() {
  Serial.begin(9600);
  servo.attach(servoPin);
  input_password.reserve(10);                  // Caractére maximum d'entrée
  pinMode(buzzer, OUTPUT);                     // Initialisation du buzzer comme sortie
  digitalWrite(buzzer, LOW);                   // Eteindre le buzzer
 // servo.write(0);                            // Mettre le servomoteur à son état initial 0
  //delay(100);
  servo.detach();
}

void loop() {
  char key = keypad.getKey();

  if (key){
    Serial.println(key);

    if(key == '*') {
      input_password = "";                   // Effacer le mot de passe
    } else if(key == '#') {
      if(input_password == password_1 || input_password == password_2 || input_password == password_3) {
        Serial.println("Le Mot de passe est correcte");
        servo.attach(servoPin);
        delay(1);
        servo.write(0);  
        delay(3000);       
        servo.write(180);                   // Mouvement de rotation du servomoteur
        delay(1000);
        servo.detach();      
      } else {
        Serial.println("Mot de Passe INCORRECTE, veuillez réessayer");
        digitalWrite(buzzer, HIGH);
        delay(2000);
        digitalWrite(buzzer, LOW);
      }

      input_password = "";                  // Effacer le Mot de passe
    } else {
      input_password += key;                // Ajout de caractére pour réessayer un nouveau Mot de passe
    }
  }
}
