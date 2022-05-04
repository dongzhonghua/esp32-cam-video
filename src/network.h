#ifndef NETWORK_H
#define NETWORK_H

#include "Arduino.h"
#include "WiFi.h"
#include "WiFiMulti.h"
#include "HTTPClient.h"

#define WIFINAME "dzh"
#define WIFIPW "dzhyx123"

class Network
{
private:

public:
    void init(String ssid, String password);
    unsigned int getBilibiliFans(String url);

};

#endif