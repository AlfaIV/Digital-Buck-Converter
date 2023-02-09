/*

*/


#include "Control_Func_API.h"
#include "InputVoltageRead.h"
#include "Gen_pulse.h"

#include "math.h"

InputVoltageRead ADC;
Control_Func_API CtrlFunc;
Gen_pulse Gener;

void setup() {
  Serial.begin(9600);
  //Gener.Set_PWM();
  Gener.Set_Hyst(0.05, -0.05);
  /*
  randomSeed(analogRead(35));
  float window = 0.1;
  Gener.Set_Hyst(window,window);
  */

}

void loop() {
  /*
  //Проверка гистерезисного управления
  Serial.print("ADC_read_data:");
  Serial.print(ADC.Read_data());
  Serial.print(",");

  Serial.print("ADC_data_in_V:");
  Serial.print(ADC.Get_real_volt());
  Serial.print(",");

  Serial.print("ADC_filtered_data:");
  double Input_Data = ADC.expRunningAverage();
  Serial.print(Input_Data);
  Serial.print(",");

  Serial.print("Discrepancy:");
  double discrepancy = CtrlFunc.P_regulation(Input_Data);
  double ref_val = CtrlFunc.PreDefined_control_data.reference_value;
  Serial.println(discrepancy);
  
  Gener.Change_PWM(discrepancy);
  delay(50);
  */

  /*
  //Проверка ЧИМ управления
  delay(100);

  Serial.print("ADC_read_data:");
  Serial.print(ADC.Read_data());
  Serial.print(",");

  Serial.print("ADC_data_in_V:");
  Serial.print(ADC.Get_real_volt());
  Serial.print(",");

  Serial.print("ADC_filtered_data:");
  double Input_Data = ADC.expRunningAverage();
  Serial.print(Input_Data);
  Serial.print(",");

  Serial.print("Discrepancy:");
  double discrepancy = CtrlFunc.P_regulation(Input_Data);
  double ref_val = CtrlFunc.PreDefined_control_data.reference_value;
  Serial.println(discrepancy);
  Gener.Change_PFM(discrepancy);
  */

  /*
  //Проверка гистерезисного управления
  //генерация рандомных входных данных проводилась для проверки скорости
  Serial.print("ADC_filtered_data:");
  double Input_Data = random(50,250)/1e2;
  Serial.print(Input_Data);
  Serial.print(",");

  Serial.print("Discrepancy:");
  double discrepancy = CtrlFunc.P_regulation(Input_Data);
  double ref_val = CtrlFunc.PreDefined_control_data.reference_value;
  Serial.println(discrepancy);
  //Gener.Change_PWM(discrepancy);
  Gener.Change_Hyst(discrepancy);
  */


  //Проверка гистерезиса управления
  delay(100);

  Serial.print("ADC_read_data:");
  Serial.print(ADC.Read_data());
  Serial.print(",");

  Serial.print("ADC_data_in_V:");
  Serial.print(ADC.Get_real_volt());
  Serial.print(",");

  Serial.print("ADC_filtered_data:");
  double Input_Data = ADC.expRunningAverage();
  Serial.print(Input_Data);
  Serial.print(",");

  Serial.print("Discrepancy:");
  double discrepancy = CtrlFunc.P_regulation(Input_Data);
  double ref_val = CtrlFunc.PreDefined_control_data.reference_value;
  Serial.println(discrepancy);
  Gener.Change_Hyst(discrepancy);
  //delay(100);
}

