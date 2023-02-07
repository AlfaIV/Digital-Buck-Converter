#include "InputVoltageRead.h"

//InputVoltageRead.cpp

#define ADC_pin 35
#define ADC_bit_with 12

InputVoltageRead::InputVoltageRead(){
  adcAttachPin(ADC_pin);
  analogSetWidth(ADC_bit_with);
  //analogSetAttenuation();
};

int InputVoltageRead::Read_data(){
  //Serial.print("analogRead:");
  ReadData = analogRead(ADC_pin);
  //Serial.print(ReadData);
  //Serial.print(";");
  //Serial.print("analogReadMilliVolts:");
  //int ReadData_2 = analogReadMilliVolts(ADC_pin);
  //Serial.print(";");
  //Serial.println();
  return ReadData;
};

double InputVoltageRead::Get_real_volt(int& Data){
  double K = 3.3/4096;
  return K*(Data);
}; 

double InputVoltageRead::Get_real_volt(){
  double K = 3.3/4096;
  ReadData = InputVoltageRead::Read_data();
  ReadDataInVolt = K*(ReadData);
  return ReadDataInVolt;
};

// бегущее среднее
double InputVoltageRead::expRunningAverage(double newVal) {
  double k = 0.1;  // коэффициент фильтрации, 0.0-1.0
  //???double systematic_error = 0.05;

  filVal += (newVal - filVal) * k;
  return filVal;
  //return 0;
};

double InputVoltageRead::expRunningAverage() {
  double k = 0.1;  // коэффициент фильтрации, 0.0-1.0
  //???double systematic_error = 0.05;

  filVal += (ReadDataInVolt - filVal) * k;
  return filVal;
  //return 0;
};