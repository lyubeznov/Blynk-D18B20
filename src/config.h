#ifndef H_CONFIG
#define H_CONFIG

#include "deps.h"

#include "wifi_info.h"

const char BLYNK_AUTH_TOKEN[] = "tokentokentokentokentoken";

const WIFI_INFO WIFI_CONNECTION_VARIANTS[] = {
  {
    "SSID", // SSID
    "Passphrase" // Passphrase
  }
};

const size_t WIFI_CONNTECTION_VARIANTS_COUNT = sizeof(WIFI_CONNECTION_VARIANTS) / sizeof(WIFI_INFO);

#endif
