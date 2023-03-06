#ifndef Constants_H

#define Constants_H
#define K_devider 6.28
#define ADC_in_MAX 3.3 * K_devider//??

//ADC parametrs
#define ADC_pin 39//CHANGE
//39
#define ADC_bit_with 12
//filters parametrs
#define K_filter 0.05
#define NUM_READ 30 // порядок медианы
//-------------------------------------------------------------
#define def_channel 0
#define def_pin 27
#define def_resolution 9 //bit

#define Initial_Freq 100e3
//не на ADC а на делителе
#define refer_out 5//везде и так передаем
#define deviation 0.15//можно поменять, точность
//#define ref_Duty 5/15

#define Initial_t_p 8e-6 //8 us
#define MAX_freq 150.0e3 //Hz
#endif