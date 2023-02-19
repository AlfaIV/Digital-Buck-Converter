#ifndef Control_Func_API_h
#define Control_Func_API_h

#include "Arduino.h"

//change
#define out_value 5

class Control_Func_API
{
  public:

    typedef struct Control_data{
      //стуктура с данными для настройки регуляторов
      //expected that system do not have inital value
      double current_value = 0;//this value from ADC
      //значение с ADC
      double reference_value = out_value;//output voltage
      //требуемое выходное напряжение
      
      double current_value_flt = 0;
      
      //параметры регуляторов
      double K_p = 1;
      double K_i = 0.1;
      double K_d = 1;

      int dt = 1;
      //?? how take this value
      double integral = 0;
      double prev_err = reference_value - current_value;//
      
      unsigned int min_output = 0;
      unsigned int max_output = 10;

    } Control_data;
    
    Control_data PreDefined_control_data;

    //void Control_Func_API();//we can add consructor for generate 
    //undependent configered exanples
    
    double P_regulation(Control_data * Current_control_data);
    double P_regulation();
    double P_regulation(double current_value);
  
    
    double PD_regulation(Control_data * Current_control_data);
    double PD_regulation();
    double PD_regulation(double current_value);
    
    double PID_regulation(Control_data * Current_control_data);
    double PID_regulation();
    double PID_regulation(double current_value);

};


#endif