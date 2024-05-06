#ifndef OBJET
#define OBJET

#include <Arduino.h>

class MOTEUR {
  private:
    float preV=0;
    float iV=0;
    float prePID=0;

    float kP=100;
    float kI=0*kP/2;
    float kD=0*kP/5;

    int deltaT;
    float acc_max=255;
    float Vmax=255;

    int dirPinP;
    int dirPinN;
    int pwnPin;

  public:
    MOTEUR(int dT, int dirmotPinP, int dirmotPinN, int pwnmotPin);
    void ctrl(float eV);
};

#endif