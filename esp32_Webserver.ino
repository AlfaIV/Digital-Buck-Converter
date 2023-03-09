#include "WiFi.h"
#include "ESPAsyncWebSrv.h"
#include <ESPmDNS.h>
#include "SPIFFS.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <string>
#include "Arduino.h"
#include "Stabilazer.h"

using std::string;

StabilizerState current_state;  //!!!!!!объект состояния отсылаемый каждый цикл на фронт

const char *ssid = "GalaxyS21";
const char *password = "bhar3375";

// const char *ssid = "RedmI";
// const char *password = "00123987";


AsyncWebServer server(80);
AsyncWebSocket ws("/api/websocket");

///----------------------------------------------------------------------
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

void setup() {
  Serial.begin(115200);

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  WiFi.mode(WIFI_STA);
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

  server.on("/main.95939ab52b0e100a90a3.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/main.95939ab52b0e100a90a3.js", "text/javascript");
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
        notifyClients(current_state);
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
      notifyClients(current_state);
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
      notifyClients(current_state);
      stop(current_state);
      stop(current_state);
      request->send(200, "text/plain", "");
      notifyClients(current_state);
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
        notifyClients(current_state);
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
        notifyClients(current_state);
        start(current_state);

        request->send(200, "text/plain", "");
        notifyClients(current_state);
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
        notifyClients(current_state);
        start(current_state);

        request->send(200, "text/plain", "");
        notifyClients(current_state);
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
        notifyClients(current_state);
        start(current_state);

        request->send(200, "text/plain", "");
        notifyClients(current_state);
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
        notifyClients(current_state);
        start(current_state);

        request->send(200, "text/plain", "");
        notifyClients(current_state);
      } else {
        request->send(404, "text/plain", "Invalid input value");
      }
    });

  server.begin();
  ADC.setup_adc();
  start(current_state);
}



unsigned long prev_time = 0;
void loop() {

  if (millis() - prev_time > 600) {
    notifyClients(current_state);
    // ws.cleanupClients();
    prev_time = millis();
  }

  //TestAnalogRead();

  StabilizerTread(current_state);
}