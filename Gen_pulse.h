//Gen_pulse.h

#ifndef Gen_pulse_h

#define Gen_pulse_h

#define def_channel 0
#define def_pin 13//CHANGE
#define def_resolution 9 //bit

#define Initial_Freq 100e3
#define ADC_in_MAX 3.3
#define ref_out 1.5
#define deviation 0.01
//#define ref_Duty 5/15

#define Initial_t_p 8e-6 //8 us
#define MAX_freq 150e3 //Hz

#include "Arduino.h"
#include "math.h"

class Gen_pulse
{
  public:
    int freq = Initial_Freq;
    int Duty;
    int t_p = Initial_t_p;
    int ref_Duty;

    double up_window;
    double down_window;

    int channel = def_channel;
    int pin = def_pin;
    int resolution = def_resolution;
    //Gen_pulse(int _pin = def_pin, int _channel = def_channel, int _resolution = def_resolution);
    
    void Set_PWM(double _ref_out = ref_out, double _ref_out_max = ADC_in_MAX);
    void Change_PWM(double discrepancy, int _Duty = NULL);

    int get_Duty(float _t_p, int _freq);
    void Set_PFM(int _t_p = Initial_t_p);
    void Change_PFM(double discrepancy, int _channel = def_channel, int _resolution = def_resolution);

    void Set_Hyst(double up_window, double down_window, int PIN = def_pin);
    void Change_Hyst(double discrepancy, int PIN = def_pin);
};

#endif