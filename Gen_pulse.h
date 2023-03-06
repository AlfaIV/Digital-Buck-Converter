//Gen_pulse.h

#ifndef Gen_pulse_h

#define Gen_pulse_h

#include "Arduino.h"
#include "math.h"
#include "Constants.h"

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
