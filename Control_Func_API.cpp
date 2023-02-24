//#include "HardwareSerial.h"
#include "Arduino.h"
#include "Control_Func_API.h"


//----------------------------------------------------------------
//P regulator
double Control_Func_API::P_regulation(Control_data* Current_control_data)
{
  double err = Current_control_data->reference_value - Current_control_data->current_value;
  //Serial.print("Current_control_data->K_d*err: ");
  //Serial.println(Current_control_data->K_p*err);
  
  return constrain(Current_control_data->K_p*err,
  Current_control_data->min_output,
  Current_control_data->max_output);
  
  //return Current_control_data->K_p*err;
}


double Control_Func_API::P_regulation()
{
    return P_regulation(&this->PreDefined_control_data);
}


double Control_Func_API::P_regulation(double current_value)
{
  this->PreDefined_control_data.current_value = current_value;
  //Serial.print(PreDefined_control_data.current_value);
  return P_regulation(&(this->PreDefined_control_data));
}


//----------------------------------------------------------------
//PI regulator

double Control_Func_API::PI_regulation(Control_data* Current_control_data)
{
  double err = Current_control_data->reference_value - Current_control_data->current_value;
  //int c = a > b ? a - b : a + b;
  double _integral = (Current_control_data->integral + err * Current_control_data->dt);
  //Current_control_data->integral =  _integral > Current_control_data->max_output ? Current_control_data->max_output : _integral ;
  //Current_control_data->integral =  _integral < Current_control_data->min_output ? Current_control_data->min_output : _integral ;
  Current_control_data->integral = constrain(Current_control_data->integral + err * Current_control_data->dt,
  Current_control_data->min_output,
  Current_control_data->max_output);
  
  
  return constrain(Current_control_data->K_p*err + Current_control_data->K_i*Current_control_data->integral,
  Current_control_data->min_output,
  Current_control_data->max_output);
  

  //return Current_control_data->K_p*err + Current_control_data->K_i*Current_control_data->integral;
}

double Control_Func_API::PI_regulation()
{
  return PI_regulation(&(this->PreDefined_control_data));
}

double Control_Func_API::PI_regulation(double current_value)
{
  this->PreDefined_control_data.current_value = current_value;
  return PI_regulation(&(this->PreDefined_control_data));
}


//----------------------------------------------------------------
//PID regulator
double Control_Func_API::PID_regulation(Control_data* Current_control_data)
{
  double err = Current_control_data->reference_value - Current_control_data->current_value;
  Current_control_data->integral = constrain(Current_control_data->integral + err * Current_control_data->dt,
  Current_control_data->min_output,
  Current_control_data->max_output);
  double differential = (err - Current_control_data->prev_err) / Current_control_data->dt;
  Current_control_data->prev_err = err;
  return constrain(Current_control_data->K_p*err + Current_control_data->K_i*Current_control_data->integral + Current_control_data->K_d*differential,
  Current_control_data->min_output,
  Current_control_data->max_output);
}


double Control_Func_API::PID_regulation()
{
    return PID_regulation(&(this->PreDefined_control_data));
}

double Control_Func_API::PID_regulation(double current_value)
{
    this->PreDefined_control_data.current_value = current_value;
    return PID_regulation(&(this->PreDefined_control_data));
}


//----------------------------------------------------------------
