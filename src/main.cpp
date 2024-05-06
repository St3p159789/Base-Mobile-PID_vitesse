#include <Arduino.h>
#include "objet.h"
#include "fonction.h"

#define DEBUG true

// Definition des pin de l'arduino
#define interruptPinRA 3
#define interruptPinRB 5
#define interruptPinLA 2
#define interruptPinLB 4

#define dirmotPinRP 11  // IN1
#define dirmotPinRN 10  // IN2
#define dirmotPinLP 9   // IN3
#define dirmotPinLN 6   // IN4

#define pwnmotPinR 12  // ENA
#define pwnmotPinL 8  // ENB

const int deltaT = 100;
const int Tinactif = 1000;

// Variables de correction
const int centtickpartour = 25; // nombre de tick par centieme de tour

const float tickpcmR = 297.3;
const float tickpcmL = 299.6;

const float tickpradR = 4713;
const float tickpradL = 4668;

// Variables de communication
int incomingByte = 0;
char streamChar[32];
int i_char = 0;

//position
float X = 0.01;
float Y = 0.01;
float Z = 0.01;

//mesure
unsigned long preMillis=0;
unsigned long lastTime=0;
long countR = 0;
long preCountR = 0;
long countL = 0;
long preCountL = 0;

int VRc = 0;  // Valeur commande droit en centieme de tr/s 
int VLc = 0;  // Valeur maximum théorique 485

String VitencodR = String(); //vitesse des encodeurs pour la com
String VitencodL = String();

void decryptIncom();
void interruptR();
void interruptL();

MOTEUR motR(deltaT, dirmotPinRP, dirmotPinRN, pwnmotPinR);
MOTEUR motL(deltaT, dirmotPinLP, dirmotPinLN, pwnmotPinL);

void setup() {
  Serial.begin(115200);

  pinMode(interruptPinRA, INPUT_PULLUP);
  pinMode(interruptPinRB, INPUT_PULLUP);
  pinMode(interruptPinLA, INPUT_PULLUP);
  pinMode(interruptPinLB, INPUT_PULLUP);

  pinMode(dirmotPinRP,OUTPUT);
  pinMode(dirmotPinRN,OUTPUT);
  pinMode(dirmotPinLP,OUTPUT);
  pinMode(dirmotPinLN,OUTPUT);

  pinMode(pwnmotPinR,OUTPUT);
  pinMode(pwnmotPinL,OUTPUT);

  attachInterrupt(digitalPinToInterrupt(interruptPinRA), interruptR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(interruptPinLA), interruptL, CHANGE);

}

void loop() {
  // On decrypte la commande envoyée par le port série
  if (Serial.available() > 0) {   // si reception - attention au baud rate !!!
    incomingByte = Serial.read();   // lit le caractere
    streamChar[i_char] = (char)incomingByte;   // et met le dans l'emplacement suivant de la liste
    i_char++;
  }
  if (incomingByte == 10) {    // si retour à la ligne
    incomingByte = 0;
    decryptIncom();   // decrypte le message
    i_char = 0;
  }

  // On boucle de manière régulière
  if ( millis() - preMillis > deltaT) {
    //Serial.println(VRc);

    // Mise a 0 des moteur si on a pas recu d'ordres depuis longtemps
    preMillis = millis();
    if(preMillis-lastTime > Tinactif){ VRc=0; VLc=0;}

    float dR = (float)(countR - preCountR);
    preCountR = countR;
    float dL = (float)(countL - preCountL);
    preCountL = countL;

    /*obtention de dD et dZ*/
    float dD = (dR / tickpcmR + dL / tickpcmL) / 2; //moyenne des déplacement avec gain de correction

    float dZ = ((dR / tickpradR) - (dL / tickpradL)) / 2; //différentiel de vitesse

    /*obtention des coordonées absolu*/
    X += dD * cos(Z + dZ / 2);
    Y += dD * sin(Z + dZ / 2);
    Z += dZ;
    if (Z > PI) {
      Z += -2 * PI;
    }
    else if (Z < -PI) {
      Z += 2 * PI;
    }
 
    //controle des moteurs
    int VdR = (dR/centtickpartour) / (1000/deltaT); // Vitesse des encodeurs en centieme de tr/s 
    int VdL = (dL/centtickpartour) / (1000/deltaT);

    motR.ctrl(VRc - VdR);
    motL.ctrl(VLc - VdL);

    VitencodR =  VdR;
    VitencodL =  VdL;
    
  }
}

void decryptIncom() {
  int i_decrypt;
  Serial.println(streamChar[0]);
  String stringSend = String();
  switch (streamChar[0]) {
    default://pas de match
      Serial.println("Pas de match");
      break;
    
    case 'm'://m:LLL:RRR 
    Serial.println('m');
      i_decrypt = 2;
      VLc=stringtofloat(streamChar,i_decrypt);
      while(streamChar[i_decrypt++]!=':');
      VRc=stringtofloat(streamChar,i_decrypt);
      lastTime=millis();
      break;

    case 'e':// e (envoie vitesse des encodeur en centieme de tr/s comme suis e:Vgauche:Vdroit)
      stringSend='e' + ':' + VitencodL + ':' + VitencodR;
      Serial.println(stringSend);
      break;
    
    case 'p':// p (envoie les coordonnees X (en cm), Y (en cm) et Z (en rad) comme suis p:X:Y:Z)
      stringSend='p' + ':' + String(X) + ':' + String(Y)  + ':' + String(Z);
      Serial.println(stringSend);
      break;
    /*
    case 'S'://S
      if (cmd != 'S'){
        precmd=cmd;
        cmd='S';
      }else if(isDigit(streamChar[1])){
        if(streamChar[1]=='0'){cmd=precmd;}
      }
      break;
      */
  }
}

/*interrupt du capteur*/
void interruptR() {
  if (digitalRead(interruptPinRA) == digitalRead(interruptPinRB)) {
    countR--;
  } else {
    countR++;
  }
}
void interruptL() {
  if (digitalRead(interruptPinLA) == digitalRead(interruptPinLB)) {
    countL++;
  } else {
    countL--;
  }
}