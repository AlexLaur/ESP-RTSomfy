/**
 * @file WifiClient.cpp
 * @author Laurette Alexandre
 * @brief Implementation for wifi client.
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
#include <DebugLog.h>
#include <ESP8266WiFi.h>

#include <config.h>
#include <networks.h>
#include <wifiClient.h>

/**
 * @brief Connect this device to a network
 *
 * @param conf The NetworkConfiguration to use
 * @return true, if the device is connected to the network
 * @return false, otherwise
 */
bool NetworkWifiClient::connect(const NetworkConfiguration& conf)
{
  return this->connect(conf.ssid, conf.password);
}

/**
 * @brief Connect this device to a network
 *
 * @param ssid The SSID of the network to connect to
 * @param password The password of the network
 * @return true, if the device is connected to the network
 * @return false, otherwise
 */
bool NetworkWifiClient::connect(const char* ssid, const char* password)
{
  LOG_DEBUG("Trying to connect to the WiFi...");
  LOG_DEBUG("SSID provided: ", ssid);
  LOG_DEBUG("Password provided: ", password);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED && millis() < 15000)
  {
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    LOG_DEBUG(".");
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    LOG_INFO("WiFi connected.");
    return true;
  }
  else
  {
    LOG_ERROR("Failed to connect to the Wifi. Canceling...");
    WiFi.disconnect();
    return false;
  }
}

/**
 * @brief Get the IP Address of this device
 *
 * @return String
 */
String NetworkWifiClient::getIP() { return WiFi.localIP().toString(); }

/**
 * @brief Get the address MAC of this device
 *
 * @return String
 */
String NetworkWifiClient::getMacAddress() { return WiFi.macAddress(); }

/**
 * @brief Is Connected to a network ?
 *
 * @return true, if connected
 * @return false, otherwise
 */
bool NetworkWifiClient::isConnected() { return (WiFi.status() == WL_CONNECTED); }

/**
 * @brief Scan networks
 *
 */
void NetworkWifiClient::scanNetworks()
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  int count = WiFi.scanNetworks(false);
  if (count == 0)
  {
    LOG_WARN("No Wifi Networks detected.");
  }
  else
  {
    for (int i = 0; i < count && i < MAX_NETWORK_SCAN; ++i)
    {
      // Get SSID and RSSI for each network found
      strcpy(this->m_networks[i].SSID, WiFi.SSID(i).c_str());
      this->m_networks[i].RSSI = WiFi.RSSI(i); // Signal strength in dBm
    }
    for (int i = count; i < MAX_NETWORK_SCAN; ++i)
    {
      strcpy(this->m_networks[i].SSID, "");
      this->m_networks[i].RSSI = -255;
    }
  }
}

/**
 * @brief Return the list of Network found
 *
 * @param networks Array of Networks.
 */
void NetworkWifiClient::getNetworks(Network networks[])
{
  for (unsigned short i = 0; i < MAX_NETWORK_SCAN; ++i)
  {
    networks[i].RSSI = this->m_networks[i].RSSI;
    strcpy(networks[i].SSID, this->m_networks[i].SSID);
  }
}
