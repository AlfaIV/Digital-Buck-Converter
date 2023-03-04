#include "HardwareSerial.h"
#include "InputVoltageRead.h"

//InputVoltageRead.cpp

//------------------------------------

InputVoltageRead::InputVoltageRead(){
  adcAttachPin(ADC_pin);
  analogSetWidth(ADC_bit_with);
  //analogSetAttenuation();
};

//------------------------------------

int InputVoltageRead::Read_data(){
  ReadData = analogRead(ADC_pin);
  // Serial.print("analogRead:");
  // Serial.print(ReadData);
  // Serial.print(";");
  // Serial.print("analogReadMilliVolts:");
  // int ReadData_2 = analogReadMilliVolts(ADC_pin);
  // Serial.print(";");
  // Serial.println();
  return ReadData;
};

//------------------------------------

double InputVoltageRead::Get_real_volt(int Data){
  double K = ADC_in_MAX/pow(2,ADC_bit_with);
  // Serial.print("Data: ");
  // Serial.println(Data);
  // Serial.print("K: ");
  // Serial.println(K);
  // //коэффициент переводит из принятых данных в вольты на ADC
  return K*(Data) + 0.08;
}; 

double InputVoltageRead::Get_real_volt(){
  //return this->Get_real_volt(this->Read_data());
  return this->Get_real_volt(this->expRunningAverage());
  //return this->Get_real_volt(this->findMedianN_optim());
  //сдесь нужно выбрать и настроить норм фильтр!!!!
};

//------------------------------------
// бегущее среднее
int InputVoltageRead::expRunningAverage(double newVal, double k) {
  //double k = 0.1;  
  // коэффициент фильтрации, 0.0-1.0
  //???double systematic_error = 0.05;

  filVal += (newVal - filVal) * k;
  return filVal;
  //return 0;
};

int InputVoltageRead::expRunningAverage() {
  /*
  double k = 0.1;  
  // коэффициент фильтрации, 0.0-1.0
  //???double systematic_error = 0.05;

  filVal += (this->ReadDataInVolt - filVal) * k;
  return filVal;
  //return 0;
  */
  return this->expRunningAverage(this->Read_data());
};


// облегчённый вариант медианы для N значений
// предложен Виталием Емельяновым, доработан AlexGyver
// возвращает медиану по последним NUM_READ вызовам
// НАВЕРНОЕ ЛУЧШИЙ ВАРИАНТ!
// медиана на N значений со своим буфером, ускоренный вариант
int InputVoltageRead::findMedianN_optim(float newVal)
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

int InputVoltageRead::findMedianN_optim()
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
  return Data/K + 0.1;
};

double InputVoltageRead::Volt_on_Devider()
{
  ///double Data = this->expRunningAverage();
  //Serial.print(Data);
  return this->Volt_on_Devider(this->Get_real_volt());
  //return Data;
}