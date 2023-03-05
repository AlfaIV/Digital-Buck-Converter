//AnalogRead.h
#ifndef AnalogRead_H

#define AnalogRead_H

#include "Arduino.h"
#include "math.h"

//ADC parametrs
#define ADC_pin 39//CHANGE
//39
#define ADC_bit_with 12
#define ADC_in_MAX 3.3//??


//change!!!!!
//devider paremetrs
//#define R_adc 1
#define K_devider 6.28
//#define R 455.0
//!! ---- нужно калибровать

//filters parametrs
#define K_filter 0.05
#define NUM_READ 30 // порядок медианы
//!! ---- нужно калибровать

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