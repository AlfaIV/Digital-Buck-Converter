#ifndef Control_Func_API_h
#define Control_Func_API_h

#include "Arduino.h"

class Control_Func_API
{
  public:

    typedef struct Control_data{
      //expected that system do not have inital value
      double current_value = 0;//this value from ADC
      double reference_value = 1.5;//output voltage
      double current_value_flt = 0;

      int K_p = 1;
      int K_i = 1;
      int K_d = 1;

      int dt = 1;//?? how take this value
      double integral = 0;
      double prev_err = reference_value - current_value;//
      
      unsigned int min_output = 0;
      unsigned int max_output = 15;

    } Control_data;
    
    Control_data PreDefined_control_data;

    //void Control_Func_API();//we can add consructor for generate 
    //undependent configered exanples
    
    double P_regulation(Control_data * Current_control_data);
    double P_regulation();
    double P_regulation(double current_value);
  
    
    double PI_regulation(Control_data * Current_control_data);
    double PI_regulation();
    double PI_regulation(double current_value);
    
    double PID_regulation(Control_data * Current_control_data);
    double PID_regulation();
    double PID_regulation(double current_value);

};


#endif