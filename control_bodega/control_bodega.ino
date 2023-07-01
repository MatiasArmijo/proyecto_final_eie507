#include <SPI.h>          // incluye libreria bus SPI
#include <MFRC522.h>      // incluye libreria especifica para MFRC522
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Constantes y variables para LCD y sensor de temperatura/humedad
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

int ValorAnterior = LOW;  
int SENSOR = 2;
int TEMPERATURA;
int HUMEDAD;
DHT dht(SENSOR, DHT11);

// Constantes y variables para MFRC522
#define RST_PIN 5         // constante para referenciar pin de reset
#define SS_PIN  53        // constante para referenciar pin de slave select
int buzzerPin = 7;        // pin de control para buzzer
int allow = 8;            // pin de control usuario permitido
int deny = 4;             // pin de control usuario no permitido
int sensorPIR = 24;
int ampolleta = 22;
int pinChapa = 33; 

MFRC522 mfrc522(SS_PIN, RST_PIN);               // crea objeto mfrc522 enviando pines de slave select y reset
byte LecturaUID[4];                             // crea array para almacenar el UID leido

byte Usuario1[4] = {0x39, 0x43, 0x89, 0xC2};    // UID USUARIO 1
byte Usuario2[4] = {0x87, 0xB3, 0xAA, 0x5F};    // UID USUARIO 2
byte Usuario3[4] = {0xE9, 0x7D, 0x43, 0xC2};    // UID USUARIO 3
byte Usuario4[4] = {0xA9, 0x4B, 0x44, 0xC2};    // UID USUARIO 4
byte Usuario5[4] = {0x77, 0x4C, 0xAE, 0x60};    // UID USUARIO 5

void setup() {

  //Parte LCD y Sensor de temperatura/Humedad
  lcd.begin(20, 4);
  dht.begin();
  lcd.backlight();
  byte degreeSymbol[8] = {0x04, 0x0A, 0x0A, 0x04, 0x00, 0x00, 0x00, 0x00};
  lcd.createChar(1, degreeSymbol);
  byte gotaSymbol[8] = {0x04, 0x04, 0x0A, 0x0A, 0x11, 0x11, 0x11, 0x0E};
  lcd.createChar(2, gotaSymbol);
  byte tempSymbol[8] = {0x04, 0x0A, 0x0A, 0x0A, 0x0E, 0x1F, 0x1F, 0x0E};
  lcd.createChar(3, tempSymbol);
  lcd.setCursor(1, 2);
  lcd.write(3);
  lcd.print("Temperatura:");
  lcd.setCursor(5, 3);
  delay(1000);
  lcd.write(2);
  lcd.print("Humedad:");


  // Parte para MFRC522
  Serial.begin(9600);           // inicializa comunicacion por monitor serie a 9600 bps
  SPI.begin();                  // inicializa bus SPI
  mfrc522.PCD_Init();           // inicializa modulo lector
  pinMode(buzzerPin,OUTPUT);    // declaracion pin de buzzer como salida
  pinMode(sensorPIR,INPUT);     // pin de control sensor de movimiento
  pinMode(ampolleta,OUTPUT);    // Ampolleta
  pinMode(pinChapa,OUTPUT);
 
}

void loop() {

  //Parte para LCD y Sensor de temperatura/humedad
  TEMPERATURA = dht.readTemperature();
  HUMEDAD = dht.readHumidity();
  lcd.setCursor(15, 2);
  lcd.print(TEMPERATURA);
  lcd.setCursor(15, 3);
  lcd.print(HUMEDAD);
  lcd.print("%");
  lcd.setCursor(17, 2);
  lcd.write(1);
  lcd.print("C");

 // Añadir las líneas para enviar datos por el puerto serial
  Serial.print("Temp:");
  Serial.print(TEMPERATURA);
  Serial.print(",Hum:");
  Serial.println(HUMEDAD);
  delay(1000);

  //Parte para MFRC522
    mvm_detect(sensorPIR, ampolleta);                      // Funcion sensor de movimiento
      
 if ( ! mfrc522.PICC_IsNewCardPresent())                   // si no hay una tarjeta presente
    return;                                                // retorna al loop esperando por una tarjeta
  
  if ( ! mfrc522.PICC_ReadCardSerial())                   // si no puede obtener datos de la tarjeta
    return;                                               // retorna al loop esperando por otra tarjeta
    
//    Serial.print("UID:");                                 // muestra texto UID:
    for (byte i = 0; i < mfrc522.uid.size; i++) {         // bucle recorre de a un byte por vez el UID

      if (mfrc522.uid.uidByte[i] < 0x10) {                // si el byte leido es menor a 0x10
          Serial.print("0");                             // imprime espacio en blanco y numero cero
        }

/*      else {
          Serial.print(" ");                              // imprime un espacio en blanco
        }
*/        
        Serial.print(mfrc522.uid.uidByte[i], HEX);        // imprime el byte del UID leido en hexadecimal
        LecturaUID[i]=mfrc522.uid.uidByte[i];             // almacena en array el byte del UID leido      
      }

          Serial.print(",");                             // imprime un espacio de tabulacion
                      
          if((comparaUID(LecturaUID,Usuario1))) {
            Serial.println("USUARIO 1 [PERMITIDO]");           // imprime la UID y el mensaje de "permitido"
            allowed(buzzerPin,allow);                     // activa buzzer y LED de "permitido"
            openDoor(pinChapa);
           
          }

          else if ((comparaUID(LecturaUID, Usuario2))) {
            Serial.println("USUARIO 2 [PERMITIDO]");           // imprime la UID y el mensaje de "permitido"
            allowed(buzzerPin,allow);                     // activa buzzer y LED de "permitido"
            openDoor(pinChapa);
          }

          else if ((comparaUID(LecturaUID, Usuario3))) {
            Serial.println("USUARIO 3 [PERMITIDO]");           // imprime la UID y el mensaje de "permitido"
            allowed(buzzerPin,allow);                     // activa buzzer y LED de "permitido"
            openDoor(pinChapa);
          }

          else if ((comparaUID(LecturaUID, Usuario4))){
            Serial.println("USUARIO 4 [PERMITIDO]");           // imprime la UID y el mensaje de "permitido"
            allowed(buzzerPin,allow);                     // activa buzzer y LED de "permitido"
            openDoor(pinChapa);
          }

          else {            
            mfrc522.PICC_HaltA();                         // detiene comunicacion con tarjeta  
            Serial.println("USUARIO NO PERMITIDO");           // muestra texto equivalente a acceso denegado 
            denied(buzzerPin,deny);              
          }        
          
 
}


boolean comparaUID(byte lectura[],byte usuario[])        // funcion comparaUID
{
  for (byte i=0; i < mfrc522.uid.size; i++){             // bucle recorre de a un byte por vez el UID
  if(lectura[i] != usuario[i])                           // si byte de UID leido es distinto a usuario
    return(false);                                       // retorna falso
  }
  return(true);                                          // si los 4 bytes coinciden retorna verdadero

 
}

void allowed(int buzzer1, int action1) {

            digitalWrite(action1,HIGH);
            digitalWrite(buzzer1,HIGH);
            delay(250);
            digitalWrite(action1,LOW);
            digitalWrite(buzzer1,LOW);

}

void denied (int buzzer2 , int action2) {
            
            digitalWrite(action2,HIGH);
            digitalWrite(buzzer2,HIGH);
            delay(50);
            digitalWrite(buzzer2,LOW);
            digitalWrite(action2,LOW);
            delay(50);
            digitalWrite(buzzer2,HIGH);
            digitalWrite(action2,HIGH);
            delay(50);
            digitalWrite(buzzer2,LOW);
            digitalWrite(action2,LOW);
            delay(50);
            digitalWrite(buzzer2,HIGH);
            digitalWrite(action2,HIGH);
            delay(50);
            digitalWrite(buzzer2,LOW);
            digitalWrite(action2,LOW);

}

void mvm_detect(int mvm, int luz){

bool registro = false;
int Valor = digitalRead(mvm);

   if(Valor == HIGH && ValorAnterior == LOW){
    Serial.println("Sensor de Movimiento activo");
    }

    ValorAnterior = Valor;

  if (Valor == HIGH) {
    digitalWrite(luz,HIGH);
    Serial.println("LUZ ACTIVA");    
  } else if (Valor == LOW) {
    digitalWrite(luz,LOW);
  }    
   
}

void openDoor(int puerta){
    digitalWrite(puerta,HIGH);
    Serial.println("PUERTA ABIERTA");
    delay(500);
    digitalWrite(puerta,LOW);
  }
