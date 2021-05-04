#define BLYNK_PRINT Serial

#include "deps.h"
#include "config.h"

BlynkTimer timer;

OneWire oneWireDevices[] = { D5, D6, D7 };
const size_t oneWireDevicesCount = sizeof(oneWireDevices) / sizeof(OneWire);

DallasTemperature sensors[oneWireDevicesCount];

SSD1306Wire display(0x3c, D2, D1, GEOMETRY_64_48);

void updateTemperatures();

void setup() {
  delay(100);

  Serial.begin(9600);

  for (size_t i = 0; i < oneWireDevicesCount; i++) {;
    sensors[i].setOneWire(&oneWireDevices[i]);
    sensors[i].setWaitForConversion(false);
    sensors[i].begin();
  }

  timer.setInterval(1000L, updateTemperatures);

  display.init();
  display.setContrast(255);
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.clear();
  display.drawString(0, 8, "Connecting");
  display.drawString(0, 24, "    to Wi-Fi...");
  display.display();

  for (uint8_t i = 0; i < WIFI_CONNTECTION_VARIANTS_COUNT; i++) {
    millis_time_t connectionStartTime = millis();
    WIFI_INFO wifiInfo = WIFI_CONNECTION_VARIANTS[i];

    WiFi.begin(wifiInfo.ssid, wifiInfo.pass);

    while ((WiFi.status() != WL_CONNECTED) && (millis() - connectionStartTime < 10000)) {
      delay(10);
    }

    if (WiFi.status() == WL_CONNECTED) {
      break;
    }

    WiFi.disconnect();
    delay(10);
  }

  if (WiFi.status() == WL_CONNECTED) {
    display.clear();
    display.drawString(0, 8, "Connecting");
    display.drawString(0, 24, "    to Blynk...");
    display.display();

    Blynk.config(BLYNK_AUTH_TOKEN, "blynk-cloud.com", 80);
    Blynk.connect(10000);
  }
}

void loop() {
  timer.run();

  if ((WiFi.status() == WL_CONNECTED) && Blynk.connected()) {
    Blynk.run();
  }
}

void updateTemperatures() {
  for (uint8_t i = 0; i < oneWireDevicesCount; i++) {
    sensors[i].requestTemperatures();
  }

  uint32_t conversionStatus = 0;
  uint32_t isConversionCompleteValue = pow(2, oneWireDevicesCount) - 1;

  millis_time_t startConversionTime = millis();

  do {
    for (uint8_t i = 0; i < oneWireDevicesCount; i++) {
      bitWrite(conversionStatus, i, sensors[i].isConversionComplete());
    }

    yield();
  } while ((conversionStatus != isConversionCompleteValue) && ((millis() - startConversionTime) < 750));

  display.clear();
  
  for (uint8_t i = 0; i < oneWireDevicesCount; i++) {
    double temp = sensors[i].getTempCByIndex(0);

    char str[16];

    if (temp != DEVICE_DISCONNECTED_C) {
      sprintf(str, "T%d: %.2f °C", i + 1, temp);

      if (Blynk.connected()) {
        Blynk.virtualWrite(i + 1, temp);
      }
    } else {
      sprintf(str, "T%d: ---- °C", i + 1);

      if (Blynk.connected()) {
        Blynk.virtualWrite(i + 1, "----");
      }
    }

    display.drawString(0, 4 + 16 * i, str);
  }

  display.display();
}
