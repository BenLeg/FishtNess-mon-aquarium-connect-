/*****************************************************************
 *                 Définition des bibliothèques                  *
 *****************************************************************/

#include <Constellation.h>
#include <ESP8266WiFi.h>
#include <max6675.h>
#include <Stepper.h>
#include <String.h>

/*****************************************************************
 *      Déclaration des pins utilisés par les composants         *
 *****************************************************************/
 
const int relayPin = 16; // D0

const int motorIN1 = 5; // D1
const int motorIN2 = 4; // D2
const int motorIN3 = 0; // D3
const int motorIN4 = 2; // D4

const int thermoSO = 12; // D6
const int thermoCS = 13; // D7
const int thermoSCK = 15; // D8

const int pHPin = A0;  // Entrée analogique auquel le pHmètre est attaché

/*****************************************************************
 *                   Déclarations des variables                  *
 *****************************************************************/

const char ssid[]="*"; // SSID pour Internet
const char mdp[] = "*"; // Mot de passe pour Internet

const char IP[] = "*"; // IP de l'ordinateur 
const int port = 8088; // Numéro du port utilisé
const char sentinel[] = "*"; // Sentinel Virtuelle
const char package[] = "FishtNess"; // Package Virtuelle
const char cds[] = "*"; // Clef d'accès

const int stepsPerRevolution = 32*64; // Nombre de pas pour réaliser un tour complet du moteur
const int stepperSpeed = 6; // Vitesse de rotation du moteur en tr/min
const int stepperMouv = 256; //Nombre de pas nécessaire pour réaliser 1/8_ème de la rotation du moteur

#define offset -2.83            //Compensation de la déviation dû au capteur (à modifier lors de l'etalonnage à pH 7)
#define samplingInterval 500    //Intervalle de temps entre chaque mesure 

/*****************************************************************
 *                   Déclarations des pins                       *
 *****************************************************************/

Constellation<WiFiClient> constellation(IP, port, sentinel, package, cds);
Stepper myStepper(stepsPerRevolution, motorIN2, motorIN4, motorIN3, motorIN1);
MAX6675 thermocouple(thermoSCK, thermoCS, thermoSO);

/*****************************************************************
 *                 Initialisation des fonctions                  *
 *****************************************************************/
 
void setup() {
  //Initialisation du moniteur série à 115200 bauds car imposé par l'ESP8266 mini
  Serial.begin(115200); 
  Serial.println("Initialisation du Programme ...");

  //Initialisation du pin D0 en mode sortie
  pinMode(relayPin,OUTPUT); 

  //Connexion de l'ESP8266 au réseau Wifi
  WiFi.begin(ssid, mdp);  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected. IP: ");
  Serial.println(WiFi.localIP());
  
  //Initialisation du state object sur l'état faux
  constellation.pushStateObject("State", false);

  //Mise en action du moteur du distributeur de nourriture
  constellation.registerMessageCallback("Feeding",
  MessageCallbackDescriptor().setDescription("Donne une portion de nourriture aux poissons"),
  [](JsonObject& json) {
    constellation.writeInfo("La nourriture a été distribué aux poissons !");
    myStepper.step(stepperMouv);
  });

  //Allumage et extinction de l'éclairage de l'aquarium
  constellation.registerMessageCallback("SwitchState",
  MessageCallbackDescriptor().setDescription("Change l'état du relai"),
  [](JsonObject& json) {
      bool state = json["Data"].as<bool>();
      Serial.println(state);
      // Changer l'état du relais
      digitalWrite(relayPin, state ? HIGH : LOW);
      // Mise à jour du StateObject de l'état du relais
      constellation.pushStateObject("State", state);
      constellation.writeInfo("On a changé l'état de l'éclairage !");
 });
  constellation.declarePackageDescriptor();

  //Définition de la vitesse de rotation du moteur
  myStepper.setSpeed(stepperSpeed);

  Serial.println("Program is running...");
}

/*****************************************************************
 *                 Fonction qui boucle à l'infinie               *
 *****************************************************************/
 
void loop() {

  static unsigned long samplingTime = millis();
  static float pHValue = 0;
  static float voltage = 0;
  static int temperatureValue = 0;

  //Condition qui permet de réaliser un delai sans stopper le programme
  if(millis()-samplingTime > samplingInterval) //Temps d'attente de 500ms
  {
      //Mesure de l'acidité
      voltage = analogRead(pHPin)*5.0/1024;
      pHValue = 7.12; //3.5*voltage+offset;

      //Mesure de la temperature
      temperatureValue = 21; //thermocouple.readCelsius();
      
      //Envoie des State Objects à Constellation
      constellation.pushStateObject("Temperature", temperatureValue, 500);
      constellation.pushStateObject("Acidity", pHValue, 500);

      //réinitialisation du délai
      samplingTime=millis();
  }

  constellation.loop();
}
