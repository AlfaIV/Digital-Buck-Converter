#include "HardwareSerial.h"
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

void Gen_pulse::Set_PWM(double _ref_out, int _freq) {

  //D = Uout/Uin [0,1] -> Uout/Uin * 2**resolution -> [0,2**resolution]
  //int _pin = this->pin;
  //int _channel = this->channel;

  //int _resolution = def_resolution; //не меняеться, но надо один раз расчитат обьвлении
  //так как частота не выше 150 кГц
  //int _freq = Initial_Freq; //переменная в классе отражает текущее значение
  //и мжет отличаться от Initial_Freq

  //auto _ref_out = ref_out;
  //auto _ref_out_max = ADC_in_MAX;
  this->freq = _freq;
  this->resolution = floor(log2(80e6 / _freq));
  this->ref_out = _ref_out;

  // Serial.print("this->freq: ");
  // Serial.println(this->freq);
  // Serial.print("this->ref_out: ");
  // Serial.println(this->ref_out);


  ledcAttachPin(this->pin, this->channel);
  //ledcSetup(this->channel, _freq, this->resolution);
  ledcChangeFrequency(this->channel, _freq, this->resolution);

  //this->ref_Duty = floor((_ref_out/ADC_in_MAX) * pow(2, this->resolution));//fix it
  //this->Duty = this->ref_Duty;
  int _Duty = floor((_ref_out / ADC_in_MAX) * pow(2, this->resolution));  //fix it
  //Serial.print("_Duty: ");
  //Serial.println(_Duty);
  _Duty = constrain(_Duty, 0, pow(2, this->resolution) - 1);  //defend
  this->ref_Duty = _Duty;
  //this->Duty = this->ref_Duty;
  //Serial.print("this->ref_Duty: ");
  //Serial.println(this->ref_Duty);
  //delay(10);
  ledcWrite(this->channel, this->ref_Duty);
};

unsigned long prev_time_pwm = 0;

void Gen_pulse::Change_PWM(double discrepancy) {
  if (micros() - prev_time_pwm > 100) {
    //int _channel = def_channel;
    double _deviation = deviation;

    //D = (Uout + dU) /Uin = Uout/Uin + dU/Uin = refD + D'
    if (discrepancy > _deviation || discrepancy < -_deviation) {
      //int _discrepancy = floor(discrepancy/out_MAX * pow(2, this->resolution)); //fix it convert from [-Uout,+Uout] to [0,2**resolution]
      //double _discrepancy = ((discrepancy)/out_MAX * pow(2, this->resolution));
      //_Duty = this->ref_Duty + _discrepancy;
      int _Duty = floor((discrepancy + this->ref_out) / ADC_in_MAX * pow(2, this->resolution));
      _Duty = constrain(_Duty, 0, pow(2, this->resolution) - 1);  //defend
      if (_Duty == this->Duty) {
        if (discrepancy > _deviation) {
          _Duty++;
        } else {
          _Duty--;
        };
      };
      this->Duty = _Duty;
      ledcWrite(this->channel, this->Duty);
      prev_time_pwm = micros();
    };
  };
  //Serial.println(_Duty);
  //Serial.print(this->Duty);
};

//----------------------------------------------------
//PFM

int Gen_pulse::get_Duty(double _t_p, int _freq) {
  //D = tp/T = tp*freq
  int _Duty = (_t_p * _freq * pow(2, this->resolution));
  //Serial.print(_Duty);
  _Duty = constrain(_Duty, 0, pow(2, this->resolution) - 1);
  //Serial.print(_Duty);
  return _Duty;
};

void Gen_pulse::Set_PFM(double _t_p, double _ref_out) {
  this->t_p = _t_p;
  this->ref_out = _ref_out;

  double ref_freq = (_ref_out) / (ADC_in_MAX * _t_p);
  int _freq = int(ref_freq / 1e3) * 1e3;
  //Serial.print("freq:");
  //Serial.print(_freq);
  //Serial.print(",");
  _freq = constrain(_freq, 0, MAX_freq);
  //delay(100);

  //Serial.print("freq:");
  //Serial.print(_freq);
  //Serial.print(",");

  //предпологаем, что частота не выше 150кГц и следовательно разрешения в 9bit должно хватить
  //this->resolution = floor(log2(80e6/_freq));
  this->resolution = 9;

  ledcAttachPin(this->pin, this->channel);
  ledcSetup(this->channel, _freq, this->resolution);
  this->freq = _freq;
  //delay(100);

  int _Duty = get_Duty(_t_p, _freq);
  //Serial.print("_Duty:");
  //Serial.print(_Duty);
  //Serial.print(",");
  ledcWrite(this->channel, _Duty);
  this->Duty = _Duty;
  //Serial.print("duty:");
  //Serial.print(_Duty);
  //Serial.print(",");
};

void Gen_pulse::Change_PFM(double discrepancy) {
  //(dV + Vin)/Vout = tp/T = tp*(f + f') =>
  //f' = (dV + Vin)/(Vout * tp) - f

  // int _freq = floor((this->ref_out + discrepancy)/(ADC_in_MAX * this->t_p));
  int _freq = this->freq;
  if (discrepancy > deviation) {
    _freq += 1e3;
  } else if (discrepancy < -deviation) {
    _freq -= 1e3;
  };

  _freq = constrain(_freq, 100, MAX_freq);

  //предпологаем, что частота не выше 150кГц и следовательно разрешения в 9bit должно хватить
  //this->resolution = floor(log2(80e6/_freq));

  //Serial.print(this->ref_out);
  //Serial.println(this->t_p);
  ledcChangeFrequency(this->channel, _freq, this->resolution);
  this->freq = _freq;

  int _Duty = get_Duty(this->t_p, _freq);
  ledcWrite(this->channel, _Duty);
  this->Duty = _Duty;
};




//-------------------------------------------------------------------------------------
/*
void Gen_pulse::Set_Hyst(double up_window, double down_window, double _ref_out)
{
  //up_window down_window - в процентах от 0 до 1 от номинального выходного напряжения;
  //переводим в вольты, в реальные границы по напряжению
 
  double _up_window = up_window*_ref_out;
  this->up_window = _up_window;
  double _down_window = down_window*_ref_out;
  this->down_window = _down_window;
  pinMode(this->pin, OUTPUT);
};
void Gen_pulse::Change_Hyst(double discrepancy)
{
  if (discrepancy >=  this->up_window)
  {
    digitalWrite(this->pin, 1);
    Serial.print("Change_Hyst:");
    Serial.println("HIGH");
  }else if(discrepancy <=  this->down_window)
  {
    digitalWrite(this->pin, 0);
    Serial.print("Change_Hyst:");
    Serial.println("LOW");
  };
};
void COT_hyst(double window)
{
  //D = Uin/Uout = tp*f
  // f = const = func(Uout) = Uin/(Uout * tp)
  //=> tp(Uout) = Uin/(Uout * f) 
}
*/


void Gen_pulse::Set_Hyst(double up_window, double down_window, double _ref_out) {


  double _up_window = up_window;
  this->up_window = _up_window;

  double _down_window = down_window * (-1);
  this->down_window = _down_window;

  double _deviation = deviation;


  this->freq = 100e3;
  this->resolution = floor(log2(80e6 / this->freq));
  this->ref_out = _ref_out;

  ledcAttachPin(this->pin, this->channel);
  ledcSetup(this->channel, this->freq, this->resolution);
  this->Duty = 0;
  ledcWrite(this->channel, this->Duty);
};


void Gen_pulse::Change_Hyst(double discrepancy) {
  //up_window down_window - в процентах от 0 до 1 от номинального выходного напряжения;
  //переводим в вольты, в реальные границы по напряжению
  if (discrepancy > this->up_window) {
    //this->Duty= pow(2,this->resolution)*0.1;
    this->Duty = 200;
    //Serial.print("Change_Hyst:");
    //Serial.println("HIGH");
  } else if (discrepancy < this->down_window) {
    this->Duty = 0;
    //Serial.print("Change_Hyst:");
    //Serial.println("LOW");
  };
  ledcWrite(this->channel, this->Duty);
};