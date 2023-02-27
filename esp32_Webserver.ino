#include "WiFi.h"
#include "ESPAsyncWebSrv.h"
#include <ESPmDNS.h>
#include "SPIFFS.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <string>
#include "Arduino.h"

#include "Control_Func_API.h"
#include "InputVoltageRead.h"
#include "Gen_pulse.h"


InputVoltageRead ADC;
Control_Func_API CtrlFunc;
Gen_pulse Gener;

using std::string;

const char *ssid = "ZyXEL NBG-418N v2";
const char *password = "TEKKP46444";

AsyncWebServer server(80);
AsyncWebSocket ws("/api/websocket");

typedef struct StabilizerState {
  string mode = "none";  //default "none"
  double voltage = 0;
  //params for mode = "PWM"
  double duty = 0;
  double pwm_freq = 0;
  string law_reg = "П";
  //params for mode = "PFM"
  double pfm_freq = 0;
  double pulse_duration = 1e-6;
  //params for mode = "hysteresis"
  double hyster_window = 0.1;
  //
  bool is_work = false;
} StabilizerState;

StabilizerState current_state;  //!!!!!!объект состояния отсылаемый каждый цикл на фронт
//json--------------------------------------------------------------------

StaticJsonDocument<250> jsonDocument;
char buffer[250];

size_t create_json_pwm_state(double voltage, double duty, double pwm_freq, string law_reg, bool is_work) {
  jsonDocument.clear();
  jsonDocument["mode"] = "PWM";
  jsonDocument["duty"] = duty;
  jsonDocument["pwm_freq"] = pwm_freq;
  jsonDocument["law_reg"] = law_reg;
  jsonDocument["voltage"] = voltage;
  jsonDocument["is_work"] = is_work;
  size_t len = serializeJson(jsonDocument, buffer);
  return len;
}

size_t create_json_pfm_state(double voltage, double pfm_freq, double pulse_duration, bool is_work) {
  jsonDocument.clear();
  jsonDocument["mode"] = "PFM";
  jsonDocument["pfm_freq"] = pfm_freq;
  jsonDocument["pulse_duration"] = pulse_duration;
  jsonDocument["voltage"] = voltage;
  jsonDocument["is_work"] = is_work;
  size_t len = serializeJson(jsonDocument, buffer);
  return len;
}

size_t create_json_hyster_state(double voltage, double hyster_window, bool is_work) {
  jsonDocument.clear();
  jsonDocument["mode"] = "hysteresis";
  jsonDocument["hyster_window"] = hyster_window;
  jsonDocument["voltage"] = voltage;
  jsonDocument["is_work"] = is_work;
  size_t len = serializeJson(jsonDocument, buffer);
  return len;
}

//websocket--------------------------------------------------------------------
void notifyClients(StabilizerState &state) {
  if (state.mode == "PWM") {
    size_t len = create_json_pwm_state(state.voltage, state.duty, state.pwm_freq, state.law_reg, state.is_work);
    ws.textAll(buffer, len);
  }
  if (state.mode == "PFM") {
    size_t len = create_json_pfm_state(state.voltage, state.pfm_freq, state.pulse_duration, state.is_work);
    ws.textAll(buffer, len);
  }
  if (state.mode == "hysteresis") {
    size_t len = create_json_hyster_state(state.voltage, state.hyster_window, state.is_work);
    ws.textAll(buffer, len);
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      Serial.printf("WebSocket client send data\n");
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}
//-------------------------------------------------------------------
void start(StabilizerState &state) {
  if (state.is_work) {
    if (state.mode == "PWM") {
      Gener.Set_PWM(state.voltage, state.pwm_freq);
    } else if (state.mode == "PFM") {
      Gener.Set_PFM(state.pulse_duration, state.voltage);
    } else if (state.mode == "hysteresis") {
      Gener.Set_Hyst(state.hyster_window, -state.hyster_window, state.voltage);
    };
    CtrlFunc.PreDefined_control_data.reference_value = state.voltage;
    //иначе функции П ПИ и ПИД не будут работать
  };
}
void stop(StabilizerState &state) {
  if (state.is_work == false) {
    int _channel = 0;
    ledcWrite(_channel, 0);
    current_state.duty = 0;
    current_state.pfm_freq = 0;
  };
}

//крутиться в лупе, иначе стабилизатор не будет работать
void StabilizerTread(StabilizerState &state) {
  if (state.is_work) {
    double discrepancy;
    CtrlFunc.PreDefined_control_data.reference_value = state.voltage;

    if (state.mode == "PWM") {
      double out_volt = ADC.Volt_on_Devider();
      if (state.law_reg == "П") {
        discrepancy = CtrlFunc.P_regulation(out_volt);
      } else if (state.law_reg == "ПИ") {
        discrepancy = CtrlFunc.PI_regulation(out_volt);
      } else if (state.law_reg == "ПИД") {
        discrepancy = CtrlFunc.PID_regulation(out_volt);
      };

      Gener.Change_PWM(discrepancy);
      //обнавляем Duty
      state.duty = ((Gener.Duty) / pow(2, Gener.resolution)) * 100;
    } else if (state.mode == "PFM") {
      double out_volt = ADC.Volt_on_Devider();

      discrepancy = CtrlFunc.P_regulation(out_volt);
      Gener.Change_PFM(discrepancy);

      //обнавляем Duty и Freq
      state.duty = ((Gener.Duty) / pow(2, Gener.resolution)) * 100;
      state.pfm_freq = Gener.freq;
    } else if (state.mode == "hysteresis") {
      double out_volt = ADC.Volt_on_Devider(ADC.Get_real_volt(ADC.Read_data()));

      discrepancy = CtrlFunc.P_regulation(out_volt);

      Gener.Change_Hyst(discrepancy);
    };
  };
}
//--------------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println(WiFi.localIP());

  if (!MDNS.begin("stabilazer")) {
    Serial.println("Error starting mDNS");
    return;
  }

  initWebSocket();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", String(), false);
  });

  server.on("/main.4c1393cbee59ec7826dd.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/main.4c1393cbee59ec7826dd.css", "text/css");
  });

  server.on("/main.e0b2e141cbcdf730345a.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/main.e0b2e141cbcdf730345a.js", "text/javascript");
  });

  //PWM
  server.on("/PWM", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", String(), false);
  });

  //PFM
  server.on("/PFM", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", String(), false);
  });

  //hysteresis
  server.on("/hysteresis", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", String(), false);
  });

  // Обработчики
  server.on(
    "/api/switch/mode", HTTP_POST, [](AsyncWebServerRequest *request) {
      //nothing and dont remove it
    },
    NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      DeserializationError root = deserializeJson(jsonDocument, (const char *)data);
      if (root.c_str() == "Ok") {
        /*???? setMode((const char *)jsonDocument["mode"]) //возможные значения "PWM","PFM","hysteresis"
        сама функция void setMode(string mode) {...}
        */
        current_state.mode = (const char *)jsonDocument["mode"];
        start(current_state);

        request->send(200, "text/plain", "");
      } else {
        request->send(404, "text/plain", "Invalid input value");
      }
    });

  server.on(
    "/api/switch/start", HTTP_POST, [](AsyncWebServerRequest *request) {
      //nothing and dont remove it
    },
    NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      /*TODO start() 
        сама функция bool setMode(string mode) {...}
        если true то дальше request->send(200, "text/plain", "");
        если false то дальше request->send(404, "text/plain", "Invalid input value");
        Запускает стабилизатор с теми параметрами что в current_state
        */
      current_state.is_work = true;

      start(current_state);
      request->send(200, "text/plain", "");

      //request->send(404, "text/plain", "");
    });

  server.on(
    "/api/switch/stop", HTTP_POST, [](AsyncWebServerRequest *request) {
      //nothing and dont remove it
    },
    NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      /*TODO stop() 
        сама функция bool setMode(string mode) {...}
        если true то дальше request->send(200, "text/plain", "");
        если false то дальше request->send(404, "text/plain", "Invalid input value");
        собственно останавливает, при этом параметры в current_state можно принимать(не лочь!)
        */
      current_state.is_work = false;
      stop(current_state);
      delay(10);
      stop(current_state);
      request->send(200, "text/plain", "");

      //request->send(404, "text/plain", "");
    });

  server.on(
    "/api/set-voltage", HTTP_POST, [](AsyncWebServerRequest *request) {
      //nothing and dont remove it
    },
    NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      DeserializationError root = deserializeJson(jsonDocument, (const char *)data);
      if (root.c_str() == "Ok") {
        /*TODO setOutVoltage((const char *)jsonDocument["voltage"]) 
        сама функция void setOutVoltage(string voltage) {...}
        */
        current_state.voltage = std::stod((const char *)jsonDocument["voltage"]);
        start(current_state);

        //не надо его включать start(current_state);
        request->send(200, "text/plain", "");
      } else {
        request->send(404, "text/plain", "Invalid input value");
      }
    });

  server.on(
    "/api/pwm/set-frequency", HTTP_POST, [](AsyncWebServerRequest *request) {
      //nothing and dont remove it
    },
    NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      DeserializationError root = deserializeJson(jsonDocument, (const char *)data);
      if (root.c_str() == "Ok") {
        /*TODO setPwmFrequency((const char *)jsonDocument["pwm_freq"]) 
        сама функция void setPwmFrequency(string pwm_freq) {...}
        может принять pwm_freq даже если сейчас не PWM мод
        */
        current_state.pwm_freq = std::stod((const char *)jsonDocument["pwm_freq"]);
        start(current_state);

        request->send(200, "text/plain", "");
      } else {
        request->send(404, "text/plain", "Invalid input value");
      }
    });

  server.on(
    "/api/pwm/set-law-reg", HTTP_POST, [](AsyncWebServerRequest *request) {
      //nothing and dont remove it
    },
    NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      DeserializationError root = deserializeJson(jsonDocument, (const char *)data);
      if (root.c_str() == "Ok") {
        /*TODO setLawReg((const char *)jsonDocument["law_reg"]) 
        сама функция void setLawReg(string law_reg) {...}
        может принять law_reg даже если сейчас не PWM мод
        */
        //просто переписываем данные в структуре, регулятор в потоке сам подхватит измения
        current_state.law_reg = (const char *)jsonDocument["law_reg"];
        start(current_state);

        request->send(200, "text/plain", "");
      } else {
        request->send(404, "text/plain", "Invalid input value");
      }
    });

  server.on(
    "/api/pfm/set-pulse-duration", HTTP_POST, [](AsyncWebServerRequest *request) {
      //nothing and dont remove it
    },
    NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      DeserializationError root = deserializeJson(jsonDocument, (const char *)data);
      if (root.c_str() == "Ok") {
        /*TODO setPulseDuration((const char *)jsonDocument["pulse_duration"]) 
        сама функция void setPulseDuration(string pulse_duration) {...}
        может принять pulse_duration даже если сейчас не PFM мод
        */
        current_state.pulse_duration = std::stod((const char *)jsonDocument["pulse_duration"]);
        start(current_state);

        request->send(200, "text/plain", "");
      } else {
        request->send(404, "text/plain", "Invalid input value");
      }
    });

  server.on(
    "/api/hyster/set-hyster-window", HTTP_POST, [](AsyncWebServerRequest *request) {
      //nothing and dont remove it
    },
    NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      DeserializationError root = deserializeJson(jsonDocument, (const char *)data);
      if (root.c_str() == "Ok") {
        /*TODO setHysterWindow((const char *)jsonDocument["hyster_window"]) 
        сама функция void setHysterWindow(string hyster_window) {...}
        может принять hyster_window даже если сейчас не hysteresis мод
        */
        current_state.hyster_window = std::stod((const char *)jsonDocument["hyster_window"]);
        start(current_state);

        request->send(200, "text/plain", "");
      } else {
        request->send(404, "text/plain", "Invalid input value");
      }
    });

  server.begin();
}

// int countNoyify = 0;
// int countNoyifyMax = 100000;
unsigned long prev_time = 0;
void loop() {
  // if (current_state.is_work) {
  //   countNoyifyMax = 5000;
  // } else {
  //   countNoyifyMax = 100000;
  // }
  // countNoyify++;
  // if (countNoyify > countNoyifyMax) {
  //   notifyClients(current_state);
  //   ws.cleanupClients();
  //   countNoyify = 0;
  // }
  if (millis() - prev_time > 600) {
    notifyClients(current_state);
    ws.cleanupClients();
    prev_time = millis();
  }

  /*TODO StabilizerTread(current_state)
    сама функция void updateState(StabilizerState& state) {...}
    смотрит на текущий mode в current_state, и в зависимости от него обновляет нужные поля
    !!!!!!!!напряжение voltage не реальное, а то что мы сами задаём!!!!!!!!!!!!!!
  */
  StabilizerTread(current_state);
}