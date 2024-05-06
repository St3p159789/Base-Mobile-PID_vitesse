
#include <Arduino.h>
#include "fonction.h"

float stringtofloat(char list[32],int i){
bool neg=false;
float val=0;
bool virgule=false;
float val_vir=0;
  while(list[i]!=':' && list[i]!=10){
    char digit=list[i];
    if (digit=='-'){neg=true;}
    if (digit=='.'){virgule=true;}
    if (isDigit(digit)) {
      if (!virgule) {
        val=val*10 + (digit-'0');
      } else {
        val_vir=val_vir*10 + (digit-'0');
      }
    }
    i++;
  }
  while(val_vir>=1){
    val_vir=val_vir*0.1;
  }
  val+=val_vir;
  if (neg){val=-val;}
  return val;
}