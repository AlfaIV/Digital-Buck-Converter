//AnalogRead.h
#ifndef AnalogRead_H

#define AnalogRead_H

#include "Arduino.h"
#include "math.h"

class InputVoltageRead{
  public:
    int ReadData = 0;
    double ReadDataInVolt = 0;
    
    //double RealVoltageOnADC;
    double filVal = 0;

    InputVoltageRead();
    int Read_data();
    double Get_real_volt(int &Data);
    double Get_real_volt();
    double expRunningAverage(double newVal);
    double expRunningAverage();
    
};

#endif