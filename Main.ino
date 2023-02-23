/*

*/



#include "Control_Func_API.h"
#include "InputVoltageRead.h"
#include "Gen_pulse.h"

#include "math.h"

InputVoltageRead ADC;
Control_Func_API CtrlFunc;
Gen_pulse Gener;

using std::string;

typedef struct StabilizerState {
  string mode = "PFM";  //default "none"
  double voltage = 5;
  //params for mode = "PWM"
  double duty = 0;
  double pwm_freq = 100e3;
  string law_reg = "П";
  //params for mode = "PFM"
  double pfm_freq = 0;
  double pulse_duration = 8e-6;
  //params for mode = "hysteresis"
  double hyster_window = 0.1;
  //
  bool is_work = true;
} StabilizerState;

StabilizerState current_state;  //!!!!!!объект состояния отсылаемый каждый цикл на фронт

//-------------------------------------------------------------------
void start(StabilizerState& state) {
  if (state.is_work) {
    if (state.mode == "PWM") {
      Serial.println("PWM");
      //double ref_in = 17;//volt
      Gener.Set_PWM(state.voltage, state.pwm_freq );
    } else if (state.mode == "PFM") {
      Serial.println("PFM");
      Gener.Set_PFM(state.pulse_duration, state.voltage);
    } else if (state.mode == "hysteresis") {
      Serial.println("hysteresis");
      Gener.Set_Hyst(state.hyster_window, -state.hyster_window, state.voltage);
    };
  };
}
void stop(StabilizerState& state) {
  if (state.is_work == false) {
    int _channel = 0;
    ledcWrite(_channel, 0);
  };
}

//крутиться в лупе, иначе стабилизатор не будет работать
void StabilizerTread(StabilizerState& state) {
  if (state.is_work) {
    double out_volt = ADC.Volt_on_Devider();
    Serial.print("Volt_on_Devider: ");
    Serial.println(out_volt);
    double discrepancy;
    if (state.mode == "PWM")
    {
      Serial.println("PWM");
      if (state.law_reg == "П")
      {
        Serial.println("P reg");
        discrepancy = CtrlFunc.P_regulation(out_volt);
      } else if (state.law_reg == "ПИ")
      {
        Serial.println("PI reg");
        Serial.print("Integral: ");
        Serial.println(CtrlFunc.PreDefined_control_data.integral);
        discrepancy = CtrlFunc.PI_regulation(out_volt);
        delay(10);
      } else if (state.law_reg == "ПИД")
      {
        Serial.println("PID reg");
        discrepancy = CtrlFunc.PID_regulation(out_volt);
        delay(10);
      };
      Serial.print("discrepancy: ");
      Serial.println(discrepancy);
      Gener.Change_PWM(discrepancy);
      //обнавляем Duty
      state.duty = Gener.Duty;
      //Serial.print("Duty: ");
      //Serial.println(state.duty);
    }else if (state.mode == "PFM")
    {
      Serial.println("PFM");
      Serial.println("P reg");
      discrepancy = CtrlFunc.P_regulation(out_volt);
      Gener.Change_PFM(discrepancy);
      Serial.print("discrepancy: ");
      Serial.println(discrepancy);
      //обнавляем Duty и Freq
      state.duty = Gener.Duty;
      state.pfm_freq = Gener.freq;
      Serial.print("Duty: ");
      Serial.println(state.duty);
      Serial.print("Freq: ");
      Serial.println(state.pfm_freq);
    }else if (state.mode == "hysteresis")
    {
      Serial.println("hysteresis");
      Serial.println("P reg");
      Gener.Change_Hyst(CtrlFunc.P_regulation(out_volt));
    };
  };
}
//--------------------------------------------------------------------

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
  Gener.Change_PWM(0.01, 150);
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
  Serial.print(CtrlFunc.PI_regulation(Read_Out_Volt));
  Serial.print(",");

  Serial.print("Discrepancy_PID_reg:");
  Serial.print(CtrlFunc.PID_regulation(Read_Out_Volt));
  //Serial.print(",");


  Serial.println();


};
int out_stb = 5;
int out_max = 15;
int _resolution = 9;//bit


bool TestPWM()
{


  //int st_Duty = (out_stb/out_max)*pow(2,_resolution);
  auto Count_Duty = [& out_stb, & out_max, & _resolution](double discrepancy)->int
  {
    int Duty = floor(((out_stb + discrepancy) / out_max) * pow(2, _resolution));
    //Serial.println("Count_Duty");
    //Serial.print("For discrepancy: ");
    //Serial.println(discrepancy);
    Serial.print(" counted Duty: ");
    Serial.println(Duty);
    return Duty;
  };

  double discrepancy[7] = {0, 0.5, -0.5 , 1, -1, 2, -2};

  //Gener.Set_PWM();
  //Serial.print("Start Test:");
  //Serial.println(Count_Duty(0) == Gener.Duty);
  //delay(1e3);
  //Gener.Change_PWM(discrepancy[0]);
  for (int i = 0; i < 5; i++)
  {
    Serial.println("========================");
    int CountedDuty = Count_Duty(discrepancy[i]);
    Gener.Change_PWM(discrepancy[i]);
    Serial.print("PWM test");
    Serial.print(i);
    Serial.print(": ");
    Serial.println( CountedDuty == Gener.Duty);
    Serial.print("Gener.Duty:");
    Serial.println(Gener.Duty);
    //Serial.println("========================");
    delay(10);
  };

  return 1;
};

bool TetsPFM()
{
  //D = Uin/Uout = tp*f
  //f = Uin/(Uout + tp)

  
  auto CountFreq = [&current_state](double discrepancy)->int
  { 
    int freq = ((current_state.voltage + discrepancy)/(ADC_in_MAX*current_state.pulse_duration));
    return (freq/1e3)*1e3;
  };

  auto CountDuty = [&current_state](int freq)->int
  { 
    return freq*current_state.pulse_duration;
  };

  int len = 7;
  double TestIn[len] = {0,1,-1,2,-2,6,-6};

  for (int i = 0; i < len; i++)
  {
    Serial.println("========================");
    int CountedFreq = CountFreq(TestIn[i]);
    int CountedDuty = CountDuty(CountedFreq*pow(2, Gener.resolution));


    Gener.Change_PFM(TestIn[i]);
    Serial.print("PFM test");
    Serial.print(i);
    Serial.print(": fr");
    Serial.print( CountedFreq == Gener.freq);
    Serial.print("  du");
    Serial.println( CountedDuty == Gener.Duty);
    Serial.print("Gener.freq: ");
    Serial.println(Gener.freq);
    Serial.print("CountFreq: ");
    Serial.println(CountedFreq);
    Serial.print("Gener.Duty: ");
    Serial.println(Gener.Duty);
    Serial.print("CountedDuty: ");
    Serial.println(CountedDuty);
    //Serial.println("========================");
    delay(10);
  };
  
  //CountFreq(0);

  return 1;
};


void SomeControlTest()
{
  //Gener.Change_PWM(CtrlFunc.P_regulation(ADC.Volt_on_Devider()),out_max);

  /*
    Gener.Change_PWM(CtrlFunc.PD_regulation(ADC.Volt_on_Devider()),out_max);
    Serial.print("integral:");
    Serial.println(CtrlFunc.PreDefined_control_data.integral);
    delay(100);
  */

  Gener.Change_PWM(CtrlFunc.PID_regulation(ADC.Volt_on_Devider()), out_max);
  //Serial.print("integral:");
  //Serial.println(CtrlFunc.PreDefined_control_data.integral);
  //Serial.print("integral:");
  //Serial.println(CtrlFunc.PreDefined_control_data.integral);
  delay(100);

};



void setup() {
  Serial.begin(9600);
  //Gener.Set_PWM(out_stb,out_max);
  //TestPWM();
  //Gener.Set_Hyst(0.05, -0.05);
  /*
    randomSeed(analogRead(35));
    float window = 0.1;
    Gener.Set_Hyst(window,window);
  */
  start(current_state);
  //TestPWM();
  //TetsPFM();
  
  //delay(10);

};

void loop() {

  StabilizerTread(current_state);
  //TestAnalogRead();

  //TestAnalogRead();
  //TestControlFunc();
  //SomeControlTest();

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

