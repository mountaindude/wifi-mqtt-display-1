
#include <ESP8266WiFi.h>
#include "CMMC_OTA.h"

#include <TM1638.h>
#include <InvertedTM1638.h>

#ifndef WIFI_SSID
  #define WIFI_SSID       ""
  #define WIFI_PASSPHRASE ""
#endif

CMMC_OTA ota;


// define a module on data pin 8, clock pin 9 and strobe pin 7
// define a module on data pin D3, clock pin D2 and strobe pin D4
// TM1638 module(8, 9, 7);
TM1638 module(D3, D2, D4);
// InvertedTM1638 module(D3, D2, D4);

unsigned long startTime;
byte mode;

void init_wifi_hardware()
{
  module.setDisplayToString("Get IP...");

  WiFi.disconnect(true);
  Serial.begin(115200);
  Serial.flush();
  Serial.println();
  Serial.println();
  Serial.println("Starting wifi...");
  delay(100);

  WiFi.begin(WIFI_SSID, WIFI_PASSPHRASE);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf("connecting %s:%s \r\n", WIFI_SSID, WIFI_PASSPHRASE);
    delay(1000);
  }

  Serial.print("READY!! IP address: ");

  Serial.println(WiFi.localIP());
}


void setup() {
  module.setupDisplay(true, 7);

  init_wifi_hardware();

  ArduinoOTA.setPort(8266);
  ArduinoOTA.setHostname("ledmatrix-1");

  // No authentication by default
  ArduinoOTA.setPassword((const char *)"mypwd");

  ota.init();

  delay(3000);

  startTime = millis();
  mode = 0;
}


void update(TM1638* module, byte* mode) {
  byte buttons = module->getButtons();
  unsigned long runningSecs = (millis() - startTime) / 1000;

  // button pressed - change mode
  if (buttons != 0) {
    *mode = buttons >> 1;
    module->clearDisplay();
    module->setLEDs(0);
  }

  switch (*mode) {
    case 0:
      module->setDisplayToDecNumber(runningSecs, 1 << 7);
      break;
    case 1:
      module->setDisplayToDecNumber(runningSecs, 1 << 6, false);
      break;
    case 2:
      module->setDisplayToHexNumber(runningSecs, 1 << 5);
      break;
    case 4:
      module->setDisplayToHexNumber(runningSecs, 1 << 4, false);
      break;
    case 8:
      module->setDisplayToBinNumber(runningSecs, 1 << 3);
      break;
    case 16:
      module->clearDisplayDigit((runningSecs - 1) % 8, 0);
      module->setDisplayDigit(runningSecs % 8, runningSecs % 8, 0);
      break;
    case 32:
      char s[8];
      sprintf(s, "Secs %03d", runningSecs % 999);
      module->setDisplayToString(s);
      break;
    case 64:
      if (runningSecs % 2 == 0) {
        module->setDisplayToString("TM1638  ");
      } else {
        module->setDisplayToString("LIBRARY ");
      }

      module->setLED(0, (runningSecs - 1) % 8);
      module->setLED(1 + runningSecs % 3, runningSecs % 8);
      break;
    case 65:
      module->setDisplayToError();
      break;

  }
}


void loop() {
  ota.loop();

  update(&module, &mode);
}
