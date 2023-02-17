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
    //конструктор, где настраиваем пины
    
    int Read_data();
    //считываем данные с пина
    
    double Get_real_volt(int &Data);
    double Get_real_volt();
    //получаем данные в вольтах на ADC
    
    double expRunningAverage(double newVal);
    double expRunningAverage();
    //прогоняем данные через фильтр Бегущее среднее
    float findMedianN_optim(float newVal);
    float findMedianN_optim();
    //прогоняем данные через медианный фильтр

    double Volt_on_Devider(double Data);
    double Volt_on_Devider();
};

#endif