//AnalogRead.h
#ifndef AnalogRead_H

#define AnalogRead_H

#include "Arduino.h"
#include "math.h"
#include "Constants.h"

class InputVoltageRead{
  //данный класс инициализирует пины ЦАПа,
  //затем считывает с него значения, эти значения фильтруем
  //затем переводит в вольты на ацп и на делителе
   public:
    int ReadData = 0;
    double ReadDataInVolt = 0;
    
    //double RealVoltageOnADC;
    double filVal = 0;

    InputVoltageRead();
    //конструктор, где настраиваем пины
    
    int Read_data();
    //считываем данные с пина
    
    double Get_real_volt(int Data);
    double Get_real_volt();
    //получаем данные в вольтах на ADC
    
    int expRunningAverage(double newVal, double k = K_filter);
    int expRunningAverage();
    //прогоняем данные через фильтр Бегущее среднее
    int findMedianN_optim(float newVal);
    int findMedianN_optim();
    //прогоняем данные через медианный фильтр

    double Volt_on_Devider(double Data);
    double Volt_on_Devider();
};
#endif