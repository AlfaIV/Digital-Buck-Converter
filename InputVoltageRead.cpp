#include "InputVoltageRead.h"

//InputVoltageRead.cpp

#define ADC_pin 15//CHANGE
#define ADC_bit_with 12
#define ADC_in_MAX 3.3


//change!!!!!
#define R_adc 96.0
#define R 455.0


//------------------------------------

InputVoltageRead::InputVoltageRead(){
  adcAttachPin(ADC_pin);
  analogSetWidth(ADC_bit_with);
  //analogSetAttenuation();
};

//------------------------------------

int InputVoltageRead::Read_data(){
  ReadData = analogRead(ADC_pin);
  //Serial.print("analogRead:");
  //Serial.print(ReadData);
  //Serial.print(";");
  //Serial.print("analogReadMilliVolts:");
  //int ReadData_2 = analogReadMilliVolts(ADC_pin);
  //Serial.print(";");
  //Serial.println();
  return ReadData;
};

//------------------------------------

double InputVoltageRead::Get_real_volt(int& Data){
  double K = ADC_in_MAX/pow(2,ADC_bit_with);
  //коэффициент переводит из принятых данных в вольты на ADC
  return K*(Data);
}; 

double InputVoltageRead::Get_real_volt(){
  double K = ADC_in_MAX/pow(2,ADC_bit_with);
  //коэффициент переводит из принятых данных в вольты на ADC
  //ReadData = InputVoltageRead::Read_data();
  ReadData = this->Read_data();
  ReadDataInVolt = K*(ReadData);
  return ReadDataInVolt;
};

//------------------------------------
// бегущее среднее
double InputVoltageRead::expRunningAverage(double newVal) {
  double k = 0.1;  
  // коэффициент фильтрации, 0.0-1.0
  //???double systematic_error = 0.05;

  filVal += (newVal - filVal) * k;
  return filVal;
  //return 0;
};

double InputVoltageRead::expRunningAverage() {
  double k = 0.1;  
  // коэффициент фильтрации, 0.0-1.0
  //???double systematic_error = 0.05;

  filVal += (this->ReadDataInVolt - filVal) * k;
  return filVal;
  //return 0;
};


// облегчённый вариант медианы для N значений
// предложен Виталием Емельяновым, доработан AlexGyver
// возвращает медиану по последним NUM_READ вызовам
// НАВЕРНОЕ ЛУЧШИЙ ВАРИАНТ!
#define NUM_READ 5 // порядок медианы
// медиана на N значений со своим буфером, ускоренный вариант
float InputVoltageRead::findMedianN_optim(float newVal)
{
  static float buffer[NUM_READ];  // статический буфер
  static byte count = 0;
  buffer[count] = newVal;
  if ((count < NUM_READ - 1) and (buffer[count] > buffer[count + 1])) {
    for (int i = count; i < NUM_READ - 1; i++) {
      if (buffer[i] > buffer[i + 1]) {
        float buff = buffer[i];
        buffer[i] = buffer[i + 1];
        buffer[i + 1] = buff;
      }
    }
  } else {
    if ((count > 0) and (buffer[count - 1] > buffer[count])) {
      for (int i = count; i > 0; i--) {
        if (buffer[i] < buffer[i - 1]) {
          float buff = buffer[i];
          buffer[i] = buffer[i - 1];
          buffer[i - 1] = buff;
        }
      }
    }
  }
  if (++count >= NUM_READ) count = 0;
  return buffer[(int)NUM_READ / 2];
}

float InputVoltageRead::findMedianN_optim()
{
  //data = this->Read_data();
  return this->findMedianN_optim(this->Read_data());
  //return 0;
}

//------------------------------------
double InputVoltageRead::Volt_on_Devider(double Data)
{
  //int _R_adc = R_adc;
  //int _R = R;
  
  double K = R_adc/(R_adc + R);
  //Serial.print(K);
  //Serial.print(Data);
  return Data/K;
};

double InputVoltageRead::Volt_on_Devider()
{
  double Data = this->expRunningAverage();
  //Serial.print(Data);
  return this->Volt_on_Devider(Data);
  //return Data;
}