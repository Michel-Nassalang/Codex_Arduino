/*
 * Created by Club MAKERS
 *
 * Code open-source
 *
 * Institut Polytechnique de Saint-Louis (IPSL)
 */

#include <Keypad.h>                    //bibliothéque Keypad
#include <Servo.h>                     //bibliothéque servomoteur
#include <EEPROM.h>                    // Memoire EEPROM pour stockage

Servo myservo;  
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

const String password_admin = "123456";               // Mot de passe administrateur
String password_classe;               // Mot de passe classe
String input_password;
String keypadadmin;
int adresse = 0;
bool changer;


void setup() {
  Serial.begin(9600);
  myservo.attach(servoPin);
  input_password.reserve(10);                  // Caractére maximum d'entrée
  pinMode(buzzer, OUTPUT);                     // Initialisation du buzzer comme sortie
  digitalWrite(buzzer, LOW);                   // Eteindre le buzzer
 // servo.write(0);                            // Mettre le servomoteur à son état initial 0
  //delay(100);
  myservo.detach();
  EEPROM.get(adresse,password_classe);
  if(password_classe == NULL){
    Serial.println("La mémoire est vide.");
    EEPROM.put(adresse,"112233");
  EEPROM.get(adresse,password_classe);
    Serial.println(password_classe);
    }else{
      Serial.println("La mémoire n'est pas vide.");
      Serial.println(password_classe);
    }
}

void loop() {
  EEPROM.get(adresse,password_classe);
  char key = keypad.getKey();
  if (key){
    Serial.print(key);
    if(key == '*') {
      input_password = "";                   // Effacer le mot de passe
    } else if(key == '#') {
      if(input_password == password_classe) {
        Serial.println("Le Mot de passe est correcte");
        myservo.attach(servoPin);
        delay(1);
        myservo.write(0);  
        delay(3000);       
        myservo.write(180);                   // Mouvement de rotation du servomoteur
        delay(1000);
        myservo.detach();      
      } else if(input_password == password_admin){
          changer = true;
          Serial.println("Changement de mot de passe : ");
          while(changer){
            char keyadmin = keypad.getKey();
            if(keyadmin){
              Serial.print(keyadmin);
              if(keyadmin == '*') {
                keypadadmin = "";                   // Effacer le mot de passe
              } else if(keyadmin == '#') {
                if(keypadadmin.length()==6){
                  EEPROM.put(adresse, keypadadmin);
                  digitalWrite(buzzer, HIGH);
                  delay(1000);
                  digitalWrite(buzzer, LOW);
                  delay(1000);
                  digitalWrite(buzzer, HIGH);
                  delay(1000);
                  digitalWrite(buzzer, LOW);
                  Serial.println("Mot de passe changé.");
                  changer = false;
                  }else{
                    Serial.println("Essayez un mot de passe de 6 caractères");
                    digitalWrite(buzzer, HIGH);
                    delay(1000);
                    digitalWrite(buzzer, LOW);
                  }
                  keypadadmin = "";
                }else{
                 keypadadmin += keyadmin;
              }
            }
          }
      }else{
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
