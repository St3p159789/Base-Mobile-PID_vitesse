#include "objet.h"

MOTEUR::MOTEUR(int dT, int dirmotPinP, int dirmotPinN, int pwnmotPin ){
    deltaT=dT;
    dirPinP=dirmotPinP;
    dirPinN=dirmotPinN;
    pwnPin=pwnmotPin;
}

void MOTEUR::ctrl(float eV){  // Erreur vit
    float dV = (eV - preV)*1000/deltaT;
    preV = eV;
    iV += eV*deltaT/1000;
    float PID = kP*eV  + kI*iV + kD*dV;
    PID=constrain(PID,-Vmax,Vmax);
    
    if (abs(PID-prePID)>acc_max*deltaT/1000){
        if (PID>prePID){
            PID=prePID+acc_max*deltaT/1000;
        }else{
            PID=prePID-acc_max*deltaT/1000;
        }
    }
    prePID=PID;

    if (PID>=0) {
        digitalWrite(dirPinP,HIGH);
        digitalWrite(dirPinN,LOW);
    } else {
        digitalWrite(dirPinP,LOW);
        digitalWrite(dirPinN,HIGH);
    }
    if (abs(PID)>=20) {
        analogWrite(pwnPin,int(abs(PID)));
    } else {
        analogWrite(pwnPin,0);
    }

}