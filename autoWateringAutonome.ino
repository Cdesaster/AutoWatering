#include <DS3231.h>

/* ========================== */
/* --> Constante <-- */
/* ========================== */
#define DEBUG 0          //Used only When true
#define MOSFET_MOTEUR 12 // Digital pin D9 where is connected MOSFET
//#define MOSFET_BATTERIE 12 // Digital pin D9 where is connected MOSFET
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
#define POT_SPEED_VALUE_MIN 0   // Valeur min du potentiometre gérant le moteur
#define POT_SPEED_VALUE_MAX 255 // Valeur max  potentiometre gérant le moteur


#define SERIAL_INTERFACE 115200 // 

int myPotentiometre = 0;  //Initialisation de la valeur du potentiomètre faisant tourner le moteur
int myTensionBatt = 0;      //Initialisation de la valeur du test de la batterie
int myDelayFlashLed = 0;    //Delay pour le flash de la LED


void initPinMode() {
#if DEBUG
 Serial.println("Initialisation des PIN");
#endif
  pinMode(MOSFET_MOTEUR, OUTPUT); //pilotage du MOSFET gérant le moteur
  pinMode(LED_PIN, OUTPUT); //pilotage du MOSFET gérant le moteur

  pinMode(PIN_A0, INPUT); //Pilotage du POTENTIOMETRE de régulation du moteur 
  pinMode(PIN_A1, INPUT); //Pilotage du POTENTIOMETRE de régulation de la batterie

  analogWrite(MOSFET_MOTEUR, 0);
  digitalWrite(LED_PIN, HIGH); //Activation de la LED
} //Fin initPinMode

void initVariables() {
#if DEBUG
 Serial.println("Initialisation des variables");
#endif
    myDelayFlashLed = DELAY_FLASH_MAX;
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
  myValeur_Moteur = map(myPotentiometre, 184, 873, 0, 255);

#if DEBUG
  //Affiche la valeur du potentiomètre sur le moniteur série
  Serial.print("Moteur : "); Serial.println(myValeur_Moteur); //Affiche la valeur du potentiomètre sur le moniteur série
#endif

  // Envoie de la valeur de sortie au moteur
  analogWrite(MOSFET_MOTEUR, myValeur_Moteur);

  delay(DELAY_SEUIL_MIN);

  digitalWrite(MOSFET_MOTEUR, HIGH); //Activation du MOTEUR

}

void setup() {

  Serial.begin(SERIAL_INTERFACE); //Initialise la vitesse de communication

#if DEBUG
 Serial.println("Dans le setup");
#endif


  //Initialisation
  initPinMode();
  initVariables();
}

void loop() {
#if DEBUG
 Serial.println("Dans le loop");
#endif

  myPotentiometre = analogRead(PIN_A0);

  analogWrite(MOSFET_MOTEUR, myPotentiometre); 

  testTensionBatterie();

  regulationVitesseMoteur();
}