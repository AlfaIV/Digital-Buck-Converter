#include "HardwareSerial.h"
#include "InputVoltageRead.h"

esp_adc_cal_characteristics_t *adc_chars = new esp_adc_cal_characteristics_t;

void InputVoltageRead::setup_adc() {
  analogReadResolution(11);
  esp_adc_cal_value_t val_type =
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_11, REF_VOLTAGE, adc_chars);
}
// InputVoltageRead.cpp
int analogRead_cal(uint8_t channel, adc_atten_t attenuation) {
  adc1_channel_t channelNum;

  /*
     Set number of cycles per sample
     Default is 8 and seems to do well
     Range is 1 - 255
   * */
  // analogSetCycles(uint8_t cycles);

  /*
     Set number of samples in the range.
     Default is 1
     Range is 1 - 255
     This setting splits the range into
     "samples" pieces, which could look
     like the sensitivity has been multiplied
     that many times
   * */
  // analogSetSamples(uint8_t samples);

  switch (channel) {
    case (36):
      channelNum = ADC1_CHANNEL_0;
      break;

    case (39):
      channelNum = ADC1_CHANNEL_3;
      break;

    case (34):
      channelNum = ADC1_CHANNEL_6;
      break;

    case (35):
      channelNum = ADC1_CHANNEL_7;
      break;

    case (32):
      channelNum = ADC1_CHANNEL_4;
      break;

    case (33):
      channelNum = ADC1_CHANNEL_5;
      break;
  }

  adc1_config_channel_atten(channelNum, attenuation);
  return esp_adc_cal_raw_to_voltage(analogRead(channel), adc_chars);
};

//------------------------------------
// бегущее среднее
int InputVoltageRead::expRunningAverage(int newVal, double k) {
  // double k = 0.1;
  //  коэффициент фильтрации, 0.0-1.0
  //???double systematic_error = 0.05;

  filVal += (newVal - filVal) * k;
  return filVal;
  // return 0;
};

// облегчённый вариант медианы для N значений
// предложен Виталием Емельяновым, доработан AlexGyver
// возвращает медиану по последним NUM_READ вызовам
// НАВЕРНОЕ ЛУЧШИЙ ВАРИАНТ!
// медиана на N значений со своим буфером, ускоренный вариант
int InputVoltageRead::findMedianN_optim(int newVal) {
  static int buffer[NUM_READ];  // статический буфер
  static byte count = 0;
  buffer[count] = newVal;
  if ((count < NUM_READ - 1) and (buffer[count] > buffer[count + 1])) {
    for (int i = count; i < NUM_READ - 1; i++) {
      if (buffer[i] > buffer[i + 1]) {
        int buff = buffer[i];
        buffer[i] = buffer[i + 1];
        buffer[i + 1] = buff;
      }
    }
  } else {
    if ((count > 0) and (buffer[count - 1] > buffer[count])) {
      for (int i = count; i > 0; i--) {
        if (buffer[i] < buffer[i - 1]) {
          int buff = buffer[i];
          buffer[i] = buffer[i - 1];
          buffer[i - 1] = buff;
        }
      }
    }
  }
  if (++count >= NUM_READ) count = 0;
  return buffer[(int)NUM_READ / 2];
}

double InputVoltageRead::Volt_on_Devider(bool isExpRunningAverage) {
  /// double Data = this->expRunningAverage();
  // Serial.print(Data);
  readValue = analogRead_cal(ADC_pin, ADC_ATTEN_DB_11);

  if (readValue < 143) {
    readValue = 0;
    // return double(this->expRunningAverage(readValue, K_filter)) * K_devider;
  }

  if (isExpRunningAverage) {
    returnValue = this->expRunningAverage(readValue) * K_devider;
  } else {
    returnValue = findMedianN_optim(readValue) * K_devider;
  }

  // returnValue += 0.0533 - returnValue * 0.0133;
  if (returnValue > 5) {
    returnValue -= 0.02 * returnValue - 0.1;
  }
  return returnValue;
  // return Data;
}