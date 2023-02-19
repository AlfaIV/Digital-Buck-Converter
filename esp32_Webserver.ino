#include "WiFi.h"
#include "ESPAsyncWebSrv.h"
#include "SPIFFS.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <string>

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
  double pulse_duration = 2;
  //params for mode = "hysteresis"
  double hyster_window = 0.1;
} StabilizerState;

StabilizerState current_state;  //!!!!!!объект состояния отсылаемый каждый цикл на фронт
//json--------------------------------------------------------------------

StaticJsonDocument<250> jsonDocument;
char buffer[250];

size_t create_json_pwm_state(double voltage, double duty, double pwm_freq, string law_reg) {
  jsonDocument.clear();
  jsonDocument["mode"] = "PWM";
  jsonDocument["duty"] = duty;
  jsonDocument["pwm_freq"] = pwm_freq;
  jsonDocument["law_reg"] = law_reg;
  jsonDocument["voltage"] = voltage;
  size_t len = serializeJson(jsonDocument, buffer);
  return len;
}

size_t create_json_pfm_state(double voltage, double pfm_freq, double pulse_duration) {
  jsonDocument.clear();
  jsonDocument["mode"] = "PFM";
  jsonDocument["pfm_freq"] = pfm_freq;
  jsonDocument["pulse_duration"] = pulse_duration;
  jsonDocument["voltage"] = voltage;
  size_t len = serializeJson(jsonDocument, buffer);
  return len;
}

size_t create_json_hyster_state(double voltage, double hyster_window) {
  jsonDocument.clear();
  jsonDocument["mode"] = "hysteresis";
  jsonDocument["hyster_window"] = hyster_window;
  jsonDocument["voltage"] = voltage;
  size_t len = serializeJson(jsonDocument, buffer);
  return len;
}

//websocket--------------------------------------------------------------------
void notifyClients(StabilizerState &state) {
  if (state.mode == "PWM") {
    size_t len = create_json_pwm_state(state.voltage, state.duty, state.pwm_freq, state.law_reg);
    ws.textAll(buffer, len);
  }
  if (state.mode == "PFM") {
    size_t len = create_json_pfm_state(state.voltage, state.pfm_freq, state.pulse_duration);
    ws.textAll(buffer, len);
  }
  if (state.mode == "hysteresis") {
    size_t len = create_json_hyster_state(state.voltage, state.hyster_window);
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

//--------------------------------------------------------------------

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

  initWebSocket();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", String(), false);
  });

  server.on("/main.990844fbb5976fcc3d57.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/main.990844fbb5976fcc3d57.css", "text/css");
  });

  server.on("/main.d53cb32032a7b77b8bfd.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/main.d53cb32032a7b77b8bfd.js", "text/javascript");
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
        Serial.println((const char *)jsonDocument["mode"]);
        /*TODO setMode((const char *)jsonDocument["mode"]) //возможные значения "PWM","PFM","hysteresis"
        сама функция void setMode(string mode) {...}
        */
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
        Serial.println((const char *)jsonDocument["voltage"]);
        /*TODO setOutVoltage((const char *)jsonDocument["voltage"]) 
        сама функция void setOutVoltage(string voltage) {...}
        */
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
        Serial.println((const char *)jsonDocument["pwm_freq"]);
        /*TODO setPwmFrequency((const char *)jsonDocument["pwm_freq"]) 
        сама функция void setPwmFrequency(string pwm_freq) {...}
        может принять pwm_freq даже если сейчас не PWM мод
        */
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
        Serial.println((const char *)jsonDocument["law_reg"]);
        /*TODO setLawReg((const char *)jsonDocument["law_reg"]) 
        сама функция void setLawReg(string law_reg) {...}
        может принять law_reg даже если сейчас не PWM мод
        */
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
        Serial.println((const char *)jsonDocument["pulse_duration"]);
        /*TODO setPulseDuration((const char *)jsonDocument["pulse_duration"]) 
        сама функция void setPulseDuration(string pulse_duration) {...}
        может принять pulse_duration даже если сейчас не PFM мод
        */
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
        Serial.println((const char *)jsonDocument["hyster_window"]);
        /*TODO setHysterWindow((const char *)jsonDocument["hyster_window"]) 
        сама функция void setHysterWindow(string hyster_window) {...}
        может принять hyster_window даже если сейчас не hysteresis мод
        */
        request->send(200, "text/plain", "");
      } else {
        request->send(404, "text/plain", "Invalid input value");
      }
    });

  server.begin();
}

void loop() {
  /*TODO updateState(current_state)
    сама функция void updateState(StabilizerState& state) {...}
    смотрит на текущий mode в current_state, и в зависимости от него обновляет нужные поля
    !!!!!!!!напряжение voltage не реальное, а то что мы сами задаём!!!!!!!!!!!!!!
  */
  notifyClients(current_state);
  // ws.cleanupClients();
}