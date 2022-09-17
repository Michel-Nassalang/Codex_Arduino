#include <LiquidCrystal.h>
LiquidCrystal lcd(12,11,5,4,3,2);
//importation du module pour le lcd
#include <Adafruit_Fingerprint.h>
//importation pour le module de détecteur d'obstacle
#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(6, 7);
#else
#define mySerial Serial1
#endif

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;
const int buttonPin = 8;
const int ledPin = 9;
const int buzzerPin = 10;
int etatButton = 0;

//--------------------------------------------------
void setup()
{
  pinMode(ledPin,OUTPUT);
  pinMode(buttonPin,INPUT);
  pinMode(buzzerPin, OUTPUT);
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Bienvenue");
  lcd.setCursor(0,1);
  lcd.print("cher client");
  Serial.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit Fingerprint sensor");

  // set the data rate for the sensor serial port
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Détecteur d'empreinte détecté");
  } else {
    Serial.println("Absence de détecteur d'empreinte :(");
    while (1) { delay(1); }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

    finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Le détecteur d'empreinte n'a enrigistré aucune empreinte. \n Veuillez insérer votre empreinte");
  }
  else {
    Serial.println("Recherche d'empreintes");
      Serial.print("Le détecteur d'empreintes a enrigistré "); Serial.print(finger.templateCount); Serial.println(" empreintes digitales");
  }
}

//----------------------------------------
void loop()                
{
  finger.getTemplateCount();
  Serial.println(finger.templateCount);
  etatButton = digitalRead(buttonPin);
  if(etatButton == LOW){
        Serial.println("Enrigistrez votre empreinte");
        if(finger.templateCount < 127){
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Veuillez vous");
          lcd.setCursor(0,1);
          lcd.print("enrigistrer");
            id = finger.templateCount+1;
            while(! getFingerprintEnroll() ); //permet d'enrigistrer une empreinte
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Bienvenue");
            lcd.setCursor(0,1);
            lcd.print("Cher client");
            digitalWrite(ledPin,HIGH);
            delay(1500);
            digitalWrite(ledPin,LOW);
          }else{
              Serial.println("La base de données est pleine. \n Veuillez nous excusez.");
            }
        }else{
      getFingerprintID(); //permet de vérifier une empreinte
      delay(1000); 
    }   
}
//----------------------------------------
uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image capturé");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("Empreinte non détecté");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erreur de communication");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Erreur sur l'image");
      return p;
    default:
      Serial.println("Erreur inconnu");
      return p;
  }

  // Capture terminé

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image convertie");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image désordonné");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erreur de communication");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("caractère d'empreinte non détecté");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("trait d'empreinte non détecté");
      return p;
    default:
      Serial.println("Erreur inconnu");
      return p;
  }

  // Convertion terminé
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Empreinte présente dans la base");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Empreinte");
    lcd.setCursor(0,1);
    lcd.print("identifie");
    digitalWrite(ledPin,HIGH);
    delay(1500);
    digitalWrite(ledPin,LOW);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Bienvenue");
    lcd.setCursor(0,1);
    lcd.print("Cher client");
    digitalWrite(ledPin,HIGH);
    delay(1500);
    digitalWrite(ledPin,LOW);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Erreur de communication");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    digitalWrite(buzzerPin,HIGH);
    delay(100);
    for(int i=0;i<=5;i++){
      digitalWrite(buzzerPin,LOW);
      delay(100);
      digitalWrite(buzzerPin,HIGH);
      delay(100);
    }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Empreinte");
    lcd.setCursor(0,1);
    lcd.print("non identifie");
    Serial.println("Empreinte absente dans la base");
    delay(1500);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Bienvenue");
    lcd.setCursor(0,1);
    lcd.print("Cher client");
    return p;
  } else {
    Serial.println("Erreur inconnu");
    return p;
  }

  // Correspondance
  Serial.print("Id trouvé #"); Serial.print(finger.fingerID);
  Serial.print(" Avec une assurance de "); Serial.println(finger.confidence);

  return finger.fingerID;
}
//----------------------------------------
uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Attente pour la validation d'empreinte #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image prise");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erreur de communication");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Erreur sur image");
      break;
    default:
      Serial.println("Erreur inconnu");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image convertie");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image désordonné");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erreur de communication");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Caractère d'empreintes non trouvé");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Caractère d'empreintes non trouvé");
      return p;
    default:
      Serial.println("Erreur inconnu");
      return p;
  }

  Serial.println("Enlevez votre doigt");
  delay(1000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Replacez le doigt");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Prise images");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erreur de communication");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Erreur sur image");
      break;
    default:
      Serial.println("Erreur inconnu");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image convertie");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image désordonné");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erreur de communication");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Caractère d'empreintes non trouvé");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Caractère d'empreintes non trouvé");
      return p;
    default:
      Serial.println("Erreur inconnu");
      return p;
  }

  // OK converted!
  Serial.print("Création de modèle pour #");  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Correspondance d'empreintes");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Erreur de communication");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Pas de correspondance d'empreintes");
    return p;
  } else {
    Serial.println("Erreur inconnu");
    return p;
  }

  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Enrigistré");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Empreinte");
    lcd.setCursor(0,1);
    lcd.print("enrigistre");
    digitalWrite(ledPin,HIGH);
    delay(500);
    digitalWrite(ledPin,LOW);
    delay(500);
    digitalWrite(ledPin,HIGH);
    delay(500);
    digitalWrite(ledPin,LOW);
    delay(2000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Bonjour");
    lcd.setCursor(0,1);
    lcd.print("Cher client");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Erreur de communication");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Defaut de stockage");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Erreur d'écriture flash");
    return p;
  } else {
    Serial.println("Erreur inconnu");
    return p;
  }

  return true;
}
