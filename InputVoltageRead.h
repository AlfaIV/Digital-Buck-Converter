//AnalogRead.h
#ifndef AnalogRead_H

#define AnalogRead_H

#include "Arduino.h"
#include "math.h"
#include <esp_adc_cal.h>

//ADC parametrs
#define ADC_pin 39  //CHANGE

// Command to see the REF_VOLTAGE: espefuse.py --port /dev/ttyUSB0 adc_info
// or dc2_vref_to_gpio(25)
#define REF_VOLTAGE 1135

#define K_devider 0.0057  //учитываем / на 1000
//!! ---- нужно калибровать

//filters parametrs
#define K_filter 0.1
#define NUM_READ 8 // порядок медианы
//!! ---- нужно калибровать

class InputVoltageRead {
  //данный класс инициализирует пины ЦАПа,
  //затем считывает с него значения, эти значения фильтруем
  //затем переводит в вольты на ацп и на делителе
public:
  //double RealVoltageOnADC;
  int filVal = 0;
  int readValue = 0;
  double returnValue = 0;

  void setup_adc();

  int findMedianN_optim(int newVal);

  int expRunningAverage(int newVal, double k = K_filter);

  double Volt_on_Devider(bool isPid = false);
};

#endif