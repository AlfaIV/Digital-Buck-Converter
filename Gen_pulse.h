//Gen_pulse.h

#ifndef Gen_pulse_h

#define Gen_pulse_h

#define def_channel 0
#define def_pin 27
#define def_resolution 9 //bit

#define Initial_Freq 100e3
#define ADC_in_MAX 17.955//это нужно менять, калибровочное значение
//не на ADC а на делителе
#define refer_out 5//везде и так передаем
#define deviation 0.05//можно поменять, точность
//#define ref_Duty 5/15

#define Initial_t_p 8e-6 //8 us
#define MAX_freq 150.0e3 //Hz

#include "Arduino.h"
#include "math.h"

class Gen_pulse
{
  public:
    int freq = Initial_Freq;
    int Duty;
    double t_p = Initial_t_p;
    int ref_Duty;
    double ref_out;

    double up_window;
    double down_window;

    int channel = def_channel;
    int pin = def_pin;
    int resolution = def_resolution;
    //Gen_pulse(int _pin = def_pin, int _channel = def_channel, int _resolution = def_resolution);
    
    void Set_PWM(double _ref_out = refer_out, int _freq = Initial_Freq);
    //задаем выходное напряжение и меняем частоту
    void Change_PWM(double discrepancy);

    int get_Duty(double _t_p, int _freq);
    void Set_PFM(double _t_p = Initial_t_p, double _ref_out = refer_out);
    void Change_PFM(double discrepancy);

    void Set_Hyst(double up_window, double down_window, double _ref_out = refer_out);
    void Change_Hyst(double discrepancy);

    void COT_hyst(double window);
};

#endif
