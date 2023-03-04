/*

*/
#include "Stabilazer.h"


void setup() {
  Serial.begin(9600);
  start(current_state);
  //TestPWM();
  //TetsPFM();
  //TestHist();
  
  //delay(10);

};

void loop() {

  StabilizerTread(current_state);
  //TestAnalogRead();

  //TestAnalogRead();
  //TestControlFunc();
  //SomeControlTest();

}

