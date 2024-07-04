/**
 * @file main.cpp
 * @author Laurette Alexandre
 * @brief Control SOMFY blinds.
 * @version 2.1.0
 * @date 2024-06-06
 *
 * @copyright (c) 2024 Laurette Alexandre
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <Arduino.h>
#include <DebugLog.h>
#include <LittleFS.h>
#include <ESP8266WiFi.h>

#include <utils.h>
#include <config.h>
#include <remote.h>
#include <networks.h>
#include <webServer.h>
#include <controller.h>
#include <wifiClient.h>
#include <mqttClient.h>
#include <systemManager.h>
#include <wifiAccessPoint.h>
#include <RTSTransmitter.h>
#include <eepromDatabase.h>
#include <jsonSerializer.h>

WifiAccessPoint wifiAP;
RTSTransmitter transmitter;
SystemManager systemManager;
NetworkWifiClient wifiClient;
EEPROMDatabase database(macToLong(wifiClient.getMacAddress().c_str()));

Network networks[MAX_NETWORK_SCAN];
Controller controller(&database, &wifiClient, &transmitter, &systemManager);

JSONSerializer serializer;
MQTTClient mqttClient(&controller, &serializer);
WebServer server(SERVER_PORT, &controller, &serializer);

// ============================================================================
// SETUP
// ============================================================================
#ifndef PIO_UNIT_TESTING
void setup()
{
  Serial.begin(115200);
  while (!Serial)
    continue;
  // Wait one second to avoid bad chars in serial
  delay(2000);

  // Open the output for 433.42MHz and 433.92MHz transmitter
  LOG_INFO("Initializing pin for transmitter...");
  transmitter.init();

  // Database Setup
  LOG_INFO("Initializing database...");
  database.init();

  // SPIFFS Setup
  LOG_INFO("Setuping SPIFFS...");
  if (!LittleFS.begin())
  {
    LOG_ERROR("An Error has occurred while mounting SPIFFS.");
    return;
  }
  else
  {
    LOG_INFO("SPIFFS setup done.");
  }

  // WIFI Setup
  LOG_INFO("Scanning all wifi networks...");
  wifiClient.scanNetworks();

  LOG_INFO("Trying WiFi connection...");
  NetworkConfiguration networkConfig = database.getNetworkConfiguration();
  if (strlen(networkConfig.ssid) == 0)
  {
    LOG_WARN("No wifi configuration found.");
    LOG_INFO("Configuring access point (AP)...");
    wifiAP.startAccessPoint(AP_SSID, AP_PASSWORD);
    LOG_INFO("AP IP address:", wifiAP.getIP());
  }
  else
  {
    bool wifiConnected = wifiClient.connect(networkConfig.ssid, networkConfig.password);
    if (!wifiConnected)
    {
      LOG_ERROR("Failed to connect to the WiFi.");
      // Not connected to wifi, starting the AP mode
      LOG_INFO("Configuring access point (AP)...");
      wifiAP.startAccessPoint(AP_SSID, AP_PASSWORD);
      LOG_INFO("AP IP address:", wifiAP.getIP());
    }
    else
    {
      LOG_INFO("WiFi IP address:", wifiClient.getIP());
    }
  }

  // MQTT setup
  LOG_INFO("Setuping MQTT...");
  MQTTConfiguration mqttConfig = database.getMQTTConfiguration();
  if (mqttConfig.enabled)
  {
    if (wifiClient.isConnected())
    {
      mqttClient.connect(mqttConfig);
      controller.attach(&mqttClient); // Connect mqttClient to the observer
    }
    else
    {
      LOG_WARN("Not connected to a network. MQTT client cannot be started.");
    }
  }
  else
  {
    LOG_INFO("MQTT is not enabled. Nothing to do.");
  }

  // SERVER setup
  LOG_INFO("Setuping WebServer...");
  server.setup();
  server.begin();
  controller.attach(&server);
}

void loop()
{
  // put your main code here, to run repeatedly:
  mqttClient.handleMessages();
  systemManager.handleActions();
}
#endif // PIO_UNIT_TESTING
