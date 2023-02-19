#include "Arduino.h"
#include "Gen_pulse.h"

/*
Gen_pulse::Gen_pulse(int _pin, int _channel, int _resolution)
{
  ledcAttachPin(_pin, _channel);
  ledcSetup(_channel, Initial_Freq, _resolution);
  //ledcWrite(0, calcDuty(80000)); 


  //
  ref_Duty = floor((ref_out/ADC_in_MAX) * pow(2, _resolution));//fix it
};
*/

//------------------------------------------------------------
//PWM

void Gen_pulse::Set_PWM(double _ref_out, double _ref_out_max)
{

  //D = Uout/Uin [0,1] -> Uout/Uin * 2**resolution -> [0,2**resolution]
  //int _pin = this->pin;
  //int _channel = this->channel;
  
  //int _resolution = def_resolution; //не меняеться, но надо один раз расчитат обьвлении
  //так как частота не выше 150 кГц
  int _freq = Initial_Freq; //переменная в классе отражает текущее значение
  //и мжет отличаться от Initial_Freq
  
  //auto _ref_out = ref_out;
  //auto _ref_out_max = ADC_in_MAX;
  
  ledcAttachPin(this->pin, this->channel);
  ledcSetup(this->channel, _freq, this->resolution);

  this->ref_Duty = floor((_ref_out/_ref_out_max) * pow(2, this->resolution));//fix it
  this->Duty = this->ref_Duty;

  ledcWrite(this->channel, this->Duty);

};


void Gen_pulse::Change_PWM(double discrepancy, double out_MAX, int _Duty)
{
  //int _channel = def_channel;
  double _deviation = deviation;
  
  //D = (Uout + dU) /Uin = Uout/Uin + dU/Uin = refD + D'
  if (_Duty == NULL && abs(discrepancy) > _deviation)
  {
    int _discrepancy = floor(discrepancy/out_MAX * pow(2, this->resolution)); //fix it convert from [-Uout,+Uout] to [0,2**resolution]
    _Duty = this->ref_Duty + _discrepancy;
  };
  Serial.print(_Duty);
  _Duty = constrain(_Duty, 0, pow(2, this->resolution));//defend
  this->Duty = _Duty;

  ledcWrite(this->channel, this->Duty);
  //Serial.print(this->Duty);
};

//----------------------------------------------------
//PFM

int Gen_pulse::get_Duty(float _t_p, int _freq)
{
  //D = tp/T = tp*freq 
  float _Duty = (_t_p*_freq*pow(2, def_resolution));
  //Serial.print(_Duty);
  _Duty = constrain(_Duty, 0, pow(2,def_resolution));
  //Serial.print(_Duty);
  return _Duty;
};

void Gen_pulse::Set_PFM(int _t_p)
{
  int _init_freq = Initial_Freq;//???
  
  ledcAttachPin(this->pin, this->channel);
  ledcSetup(this->channel, _init_freq, this->resolution);

  double ref_freq = (ref_out)/(ADC_in_MAX * Initial_t_p);
  int _freq = int(ref_freq/1e3)*1e3;
  //Serial.print("freq:");
  //Serial.print(_freq);
  //Serial.print(",");
  _freq = constrain(_freq,0,MAX_freq);
  //delay(100);

  //Serial.print("freq:");
  //Serial.print(_freq);
  //Serial.print(",");
  ledcChangeFrequency(this->channel, _freq, this->resolution);
  this->freq = _freq;
  //delay(100);

  int _Duty = get_Duty(Initial_t_p,_freq);
  //Serial.print("_Duty:");
  //Serial.print(_Duty);
  //Serial.print(",");
  ledcWrite(this->channel, _Duty);
  this->Duty = _Duty;
  //Serial.print("duty:");
  //Serial.print(_Duty);
  //Serial.print(",");
};

void Gen_pulse::Change_PFM(double discrepancy, int _channel, int _resolution)
{
  //(dV + Vin)/Vout = tp/T = tp*(f + f') =>
  //f' = (dV + Vin)/(Vout * tp) - f

  int _freq = floor((ref_out + discrepancy)/(ADC_in_MAX * Initial_t_p));
  _freq = constrain(_freq, 0 , MAX_freq);
  ledcChangeFrequency(this->channel, _freq, this->resolution);
  this->freq = _freq;

  int _Duty = get_Duty(Initial_t_p,_freq);
  ledcWrite(this->channel, _Duty);
  this->Duty = _Duty;
};

//-------------------------------------------------------------------------------------
void Gen_pulse::Set_Hyst(double up_window, double down_window, int PIN)
{
  //up_window down_window - в процентах от 0 до 1 от номинального выходного напряжения;
  //переводим в вольты, в реальные границы по напряжению

  double _up_window = up_window*ref_out;
  this->up_window = _up_window;

  double _down_window = down_window*ref_out;
  this->down_window = _down_window;

  pinMode(PIN, OUTPUT);

};
void Gen_pulse::Change_Hyst(double discrepancy, int PIN)
{
  if (discrepancy >=  this->up_window)
  {
    digitalWrite(PIN, 1);
    Serial.print("Change_Hyst:");
    Serial.println("HIGH");
  }else if(discrepancy <=  this->down_window)
  {
    digitalWrite(PIN, 0);
    Serial.print("Change_Hyst:");
    Serial.println("LOW");
  };
};