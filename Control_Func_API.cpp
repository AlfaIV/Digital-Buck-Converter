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

double Control_Func_API::P_regulation_hyst(double current_value)
{
  this->PreDefined_control_data.current_value = current_value;
  Control_data* Current_control_data = &this->PreDefined_control_data;




  //подменяем текущее значение другим дабы гистерезис работал
  double err = Current_control_data->reference_value * 1.712 - 3.796 - Current_control_data->current_value;




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
  //micros() – микросекунды, тип unsigned long, от 4 до 4 294 967 295 мкс (~70 минут), разрешение 4 мкс.
  //После “переполнения” отсчёт начинается с нуля.
  /*
  Serial.print("dt: ");
  Serial.print(micros() - Current_control_data->previous_time );
  Serial.println(" us");
  */

  //считаем, что этого хватит, нужно еще проверку на переполнение сделать
  //надо подумать, что делать если переменная переполниться, хотя для millis этогот не должно быть
  //millis() – миллисекунды, тип unsigned long, от 1 до 4 294 967 295 мс (~50 суток), разрешение 1 мс.
  //После “переполнения” отсчёт начинается с нуля.

  /*
  Serial.print("dt: ");
  Serial.print(millis() - Current_control_data->previous_time );
  Serial.println(" ms");
  */
  
  double dt = micros() - Current_control_data->previous_time;

  // Serial.print("dt(us):");
  // Serial.print(dt);
  // Serial.print(",");
  // Serial.print("dt(s):");
  // Serial.print(dt*1e-6);
  // Serial.print(",");


  if (dt >= 0)
  {
    Current_control_data->dt = dt*1e-6;
    //из мкс в с для интеграла в вольтах
  }
  



  double _integral = (Current_control_data->integral + err * Current_control_data->dt);
  //Current_control_data->integral =  _integral > Current_control_data->max_output ? Current_control_data->max_output : _integral ;
  //Current_control_data->integral =  _integral < Current_control_data->min_output ? Current_control_data->min_output : _integral ;
  Current_control_data->integral = constrain(Current_control_data->integral + err * Current_control_data->dt,
  Current_control_data->min_output,
  Current_control_data->max_output);
  

  //Current_control_data->previous_time = millis();
  Current_control_data->previous_time = micros();
  //сохраняем текущее время

  return constrain(Current_control_data->K_p*err + 10*Current_control_data->K_i*Current_control_data->integral,
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
  double dt = micros() - Current_control_data->previous_time;
  //расчет времени для интегрирования и диференцирования

  // Serial.print("dt(us):");
  // Serial.print(dt);
  // Serial.print(",");
  // Serial.print("dt(s):");
  // Serial.print(dt*1e-6);
  // Serial.print(",");

  Current_control_data->dt = dt*1e-6;
    //из мкс в с для интеграла в вольтах
  //линейная ошибка

  Current_control_data->integral = constrain(Current_control_data->integral + err * Current_control_data->dt,
  Current_control_data->min_output,
  Current_control_data->max_output);
  //расчет интеграла

  double differential = (err - Current_control_data->prev_err) / Current_control_data->dt;
  Current_control_data->prev_err = err;
  //расчет дифференциала

  Current_control_data->previous_time = micros();
  //сохраняем текущее время
  
  return constrain(Current_control_data->K_p*err + 
  10*Current_control_data->K_i*Current_control_data->integral + 
  0.1*Current_control_data->K_d*differential,
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