#include <DS3231.h>
#include <Wire.h>
#include <avr/sleep.h>  // AVR library for controlling the sleep modes

/* ========================== */
/* --> Constante <-- */
/* ========================== */
#define DEBUG 1          //Used only When true
#define MOSFET_MOTEUR 12 // Digital pin D9 where is connected MOSFET
//#define MOSFET_BATTERIE 12 // Digital pin D9 where is connected MOSFET
#define WAKE_INTERRUPT 2     // Digital pin D2 where is connected RTC SQW pin
#define LED_PIN 4        // Digital pin D4 where is connected LED
#define BUTTON_PIN 5     // Digital pin D5 where is connected BUTTON
//Pour la LED
#define DELAY_SEUIL_MAX 871   // Seuil max clignote toutes les 2 secondes
#define DELAY_SEUIL_MID 435   // Seuil middle clignote toutes les 1 seconde
#define DELAY_SEUIL_MIN 291   // Seuil min clignote toutes les 0.5 seconde
#define DELAY_FLASH_MAX 2000  // Seuil max clignote toutes les 2 secondes
#define DELAY_FLASH_MID 1000  // Seuil middle clignote toutes les 1 seconde
#define DELAY_FLASH_MIN 500  // Seuil middle clignote toutes les 1 seconde
//Pour le MOTEUR
#define POT_BORN_VALUE_MIN 0   // Valeur min du potentiometre gérant le moteur
#define POT_BORN_VALUE_MAX 255 // Valeur max  potentiometre gérant le moteur
#define POT_SPEED_VALUE_MIN 0   // Valeur min du potentiometre gérant le moteur
#define POT_SPEED_VALUE_MAX 1010 // Valeur max  potentiometre gérant le moteur
//Pour la LED
#define LED_PIN (5) // Blink Before sleeping

#define SERIAL_INTERFACE 115200 // 


/*-----------------------------------------*/
// Variables déclaration
/*-----------------------------------------*/
// A struct is a structure of logical variables used as a complete unit
struct ts {
uint8_t mySec ; // Seconds
uint8_t myMin;  // Minutes
uint8_t myHour; // Hours
uint8_t monthDay; // Day of the month
uint8_t myMonth;  // Month
int16_t myYear; // Year
uint8_t myWeekDay; // Day of the week
uint8_t myYearDay; // Day in the year
uint8_t isdst ; // daylight saving time
uint8_t shortYear; // year in short notation
};
/*-----------------------------------------*/
DS3231 myClock;           // Création d'une horloge RTC DS3231
volatile int myFlag = 0;
int myPotentiometre = 0;  // Initialisation de la valeur du potentiomètre faisant tourner le moteur
int myTensionBatt = 0;    // Initialisation de la valeur du test de la batterie
int myDelayFlashLed = 0;  // Delay pour le flash de la LED




void initPinMode() {
#if DEBUG
 Serial.println("Initialisation des PIN");
#endif

  /*---------------------------------
  MOSFET
  ---------------------------------
  pinMode(MOSFET_MOTEUR, OUTPUT); //pilotage du MOSFET gérant le moteur
  pinMode(LED_PIN, OUTPUT); //pilotage de la LED déterminant le niveau de la batterie

/*---------------------------------
  POTENTIOMETRE
  ---------------------------------
  pinMode(PIN_A0, INPUT); //Pilotage du POTENTIOMETRE de régulation du moteur 
  pinMode(PIN_A1, INPUT); //Pilotage du POTENTIOMETRE de régulation de la batterie

/*---------------------------------
  Write Information for Motor and Led
  ---------------------------------
  analogWrite(MOSFET_MOTEUR, 0);
  digitalWrite(LED_PIN, HIGH); //Activation de la LED

/*---------------------------------
  RTC DS3231
  ---------------------------------
  SQW -> Arduino D2 (Needs to be an interrupt capable pin)
  ---------------------------------
*/
pinMode(WAKE_INTERRUPT, INPUT_PULLUP); // Set alarm pin as pullup

  
} //Fin initPinMode

void initVariables() {
#if DEBUG
 Serial.println("Initialisation des variables");
#endif
    myDelayFlashLed = DELAY_FLASH_MAX;
}

void initMyRTC() {
#if DEBUG
 Serial.println("Initialisation Horloge");
#endif

/*---------------------------------
  POUR RAPPEL - Connections
  ---------------------------------
  SDA -> Arduino Analog (SDA pin)
  SCL -> Arduino Analog (SCL pin)
  VCC -> Arduino 5V
  GND -> Arduino GND
  SQW -> Arduino D2 (Needs to be an interrupt capable pin)
  ---------------------------------
  Clear the current alarm (puts DS3231 INT high)
*/
//myClock.

#if DEBUG
  Serial.println("Setup RTC completed.");
#endif
}

void testTensionBatterie() {
#if DEBUG
 Serial.println("Test tension batterie");
#endif

  myTensionBatt = analogRead(PIN_A1);
    
  digitalWrite(LED_PIN, HIGH);
  delay(myDelayFlashLed); // Wait for DelayFlash millisecond(s)
  digitalWrite(LED_PIN, LOW);

  delay(myDelayFlashLed); // Wait for DelayFlash millisecond(s)

  //Plus la tension de la batterie est faible plus la led clignote vite
  if (myTensionBatt <= DELAY_SEUIL_MIN) {
    myDelayFlashLed = DELAY_FLASH_MIN;
  } else if (myTensionBatt <= DELAY_SEUIL_MAX and myTensionBatt >= DELAY_SEUIL_MID) {
    myDelayFlashLed = DELAY_FLASH_MID;
  } else {
    myDelayFlashLed = DELAY_FLASH_MAX;
  }

}

void regulationVitesseMoteur() {
#if DEBUG
 Serial.println("Test de la régulation de la vitesse du moteur");
#endif

  int myValeur_Moteur = 0; 

  myPotentiometre = analogRead(PIN_A0);

#if DEBUG
  //Affiche la valeur du potentiomètre sur le moniteur série
  Serial.print("Potentiometre : "); Serial.println(myPotentiometre);
#endif

  // Ongère les bornes Max et Min du potentiometre
  if (myPotentiometre < 184)
  {
    myPotentiometre = 184;
  }
  // Valeur max du potentiometre
  if (myPotentiometre > 873)
  {
    myPotentiometre = 873;
  }

  //Definir la valeur à envoyer au moteur : 184 => 0 et 873 => 255)
  myValeur_Moteur = map(myPotentiometre, POT_SPEED_VALUE_MIN, POT_SPEED_VALUE_MAX, 0, 255);

#if DEBUG
  //Affiche la valeur du potentiomètre sur le moniteur série
  Serial.print("Moteur : "); Serial.println(myValeur_Moteur); //Affiche la valeur du potentiomètre sur le moniteur série
#endif

  // Envoie de la valeur de sortie au moteur
  analogWrite(MOSFET_MOTEUR, myValeur_Moteur);

  /*
  delay(DELAY_SEUIL_MIN);

  digitalWrite(MOSFET_MOTEUR, HIGH); //Activation du MOTEUR
*/
}

void setup() {

  Serial.begin(SERIAL_INTERFACE); //Initialise la vitesse de communication

#if DEBUG
 Serial.println("Dans le setup");
#endif

  Wire.begin();
  
  //Initialisation
  initPinMode();
  initVariables();
  initMyRTC();
}

void loop() {
#if DEBUG
 Serial.println("Dans le loop");
#endif

  //myPotentiometre = analogRead(PIN_A0);

 // analogWrite(MOSFET_MOTEUR, myPotentiometre); 

  //testTensionBatterie();

  regulationVitesseMoteur();
}
