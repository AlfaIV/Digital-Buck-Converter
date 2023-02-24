#ifndef Control_Func_API_h
#define Control_Func_API_h

#include "Arduino.h"



class Control_Func_API
{
  public:

    typedef struct Control_data{
      //стуктура с данными для настройки регуляторов
      //expected that system do not have inital value
      double current_value = 0;//this value from ADC
      //значение с ADC
      double reference_value = 5;//output voltage
      //требуемое выходное напряжение
      //!! ---- нужно калибровать
      
      //double current_value_flt = 0;
      
      //параметры регуляторов
      double K_p = 1;
      double K_i = 0.1;
      double K_d = 0.01;
      double dt = 0.5;
      //!! ---- нужно калибровать

      //?? how take this value
      double integral = 0;
      double prev_err = reference_value - current_value;//
      
      //can change this value for protect
      double min_output = -13;
      double max_output = 13;
      //!! ---- нужно калибровать 

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