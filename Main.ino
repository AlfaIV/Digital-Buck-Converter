/*

*/


#include "Control_Func_API.h"
#include "InputVoltageRead.h"
#include "Gen_pulse.h"

#include "math.h"

InputVoltageRead ADC;
Control_Func_API CtrlFunc;
Gen_pulse Gener;

void TestAnalogRead()
{ 
  //dont foget add to setup
  //Gener.Set_PWM();
  //заглушка для поддержания ШИМа на выходе
  Gener.Change_PWM(0.01,150);
  delay(50);


  Serial.print("ADC_read_data:");
  Serial.print(ADC.Read_data());
  Serial.print(",");
  
  Serial.print("ADC_filtered_data:");
  Serial.print(ADC.expRunningAverage());
  Serial.print(",");

  Serial.print("ADC_data_in_V:");
  Serial.print(ADC.Get_real_volt());
  Serial.print(",");

  Serial.print("ADC_filtered_data_Mediana:");
  Serial.print(ADC.findMedianN_optim());
  Serial.print(",");
  

  Serial.print("Volt_on_Devider:");
  Serial.print(ADC.Volt_on_Devider());
  //Serial.print(",");
  
  
  
  Serial.println();

};

void TestControlFunc()
{
  //dont foget add to setup
  //Gener.Set_PWM();
  //заглушка для поддержания ШИМа на выходе
  Gener.Change_PWM(0.01,150);
  delay(1000);

  double Read_Out_Volt = ADC.Volt_on_Devider();
  Serial.print("Volt_on_Devider:");
  Serial.print(Read_Out_Volt);
  Serial.print(",");
  

  Serial.print("Discrepancy_P_reg:");
  //double discrepancy = CtrlFunc.P_regulation(ADC.Volt_on_Devider());
  //double ref_val = CtrlFunc.PreDefined_control_data.reference_value;
  Serial.print(CtrlFunc.P_regulation(Read_Out_Volt));
  Serial.print(",");
  
  Serial.print("Discrepancy_PD_reg:");
  Serial.print(CtrlFunc.PD_regulation(Read_Out_Volt));
  Serial.print(",");
  
  Serial.print("Discrepancy_PID_reg:");
  Serial.print(CtrlFunc.PID_regulation(Read_Out_Volt));
  //Serial.print(",");
  
  
  Serial.println();
  

};

bool TetsPWM()
{
  int out_stb = 5;
  int out_max = 17;
  int _resolution = 9;//bit
  int st_Duty = (out_stb/out_max)*pow(2,_resolution);
  double discrepancy[4] = {1,-1,2,-2};

  Gener.Set_PWM(out_stb,out_max);
  Gener.Change_PWM(discrepancy[0]);
};

void setup() {
  Serial.begin(9600);
  Gener.Set_PWM();
  //Gener.Set_Hyst(0.05, -0.05);
  /*
  randomSeed(analogRead(35));
  float window = 0.1;
  Gener.Set_Hyst(window,window);
  */

};

void loop() {

  //TestAnalogRead();
  //TestControlFunc();



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
  /*delay(100);

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
  */
}

