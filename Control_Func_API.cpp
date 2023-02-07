#include "Arduino.h"
#include "Control_Func_API.h"

//----------------------------------------------------------------
//P regulator
double Control_Func_API::P_regulation(Control_data* Current_control_data)
{
  double err = Current_control_data->reference_value - Current_control_data->current_value;
  return (Current_control_data->K_d*err);
}


double Control_Func_API::P_regulation()
{
    return P_regulation(&PreDefined_control_data);
}


double Control_Func_API::P_regulation(double current_value)
{
  PreDefined_control_data.current_value = current_value;
  //Serial.print(PreDefined_control_data.current_value);
  return P_regulation(&PreDefined_control_data);
}


//----------------------------------------------------------------
//PI regulator

double Control_Func_API::PI_regulation(Control_data* Current_control_data)
{
  double err = Current_control_data->reference_value - Current_control_data->current_value;
  Current_control_data->integral = Current_control_data->integral + err * Current_control_data->dt;
  return (Current_control_data->K_d*err + Current_control_data->K_i*Current_control_data->integral);
}

double Control_Func_API::PI_regulation()
{
    return PI_regulation(&PreDefined_control_data);
}

double Control_Func_API::PI_regulation(double current_value)
{
    PreDefined_control_data.current_value = current_value;
    return PI_regulation(&PreDefined_control_data);
}


//----------------------------------------------------------------
//PID regulator
double Control_Func_API::PID_regulation(Control_data* Current_control_data)
{
  double err = Current_control_data->reference_value - Current_control_data->current_value;
  Current_control_data->integral = (Current_control_data->integral + err * Current_control_data->dt);
  double differential = (err - Current_control_data->prev_err) / Current_control_data->dt;
  Current_control_data->prev_err = err;
  return (Current_control_data->K_d*err + Current_control_data->K_i*Current_control_data->integral + Current_control_data->K_d*differential);
}


double Control_Func_API::PID_regulation()
{
    return PID_regulation(&PreDefined_control_data);
}

double Control_Func_API::PID_regulation(double current_value)
{
    PreDefined_control_data.current_value = current_value;
    return PID_regulation(&PreDefined_control_data);
}


//----------------------------------------------------------------
