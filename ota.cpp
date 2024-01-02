#include <Arduino.h>
#include <WiFi.h>


void OTAUpdate(Network network, String firmware_version) {
  debugln("\n[UPDATES]");
  t_httpUpdate_return ret = httpUpdate.update(*network.CLIENT, network.HOST.toString(),
                            static_cast<uint16_t>(Ports::UPDATEPORT), "/", firmware_version); 
  switch (ret) {
    case HTTP_UPDATE_FAILED:
      debugf("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      debugln("HTTP_UPDATE_NO_UPDATES");
      break;

    case HTTP_UPDATE_OK:
      debugln("HTTP_UPDATE_OK");
      break;
  }
}