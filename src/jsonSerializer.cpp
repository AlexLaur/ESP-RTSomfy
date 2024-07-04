/**
 * @file jsonSerializer.cpp
 * @author Laurette Alexandre
 * @brief Implementation for JSON serialization.
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
#include <ArduinoJson.h>

#include <remote.h>
#include <networks.h>
#include <mqttConfig.h>
#include <systemInfos.h>

#include <jsonSerializer.h>

String JSONSerializer::serializeMessage(const char* message)
{
  JsonDocument doc;
  JsonObject object = doc.to<JsonObject>();

  object["message"] = message;

  String output;
  serializeJson(doc, output);
  return output;
}

String JSONSerializer::serializeRemote(const Remote& remote)
{
  JsonDocument doc;
  JsonObject object = doc.to<JsonObject>();

  this->serializeRemote(object, remote);

  String output;
  serializeJson(doc, output);
  return output;
};

String JSONSerializer::serializeRemotes(const Remote remotes[], int size)
{
  JsonDocument doc;
  JsonArray array = doc.to<JsonArray>();

  for (int i = 0; i < size; i++)
  {
    if (remotes[i].id == 0)
    {
      // Empty remote
      continue;
    }
    JsonObject object = array.add<JsonObject>();
    this->serializeRemote(object, remotes[i]);
  }

  String output;
  serializeJson(doc, output);
  return output;
};

String JSONSerializer::serializeNetworkConfig(const NetworkConfiguration& networkConfig)
{
  JsonDocument doc;
  JsonObject object = doc.to<JsonObject>();

  object["ssid"] = networkConfig.ssid;
  object["password"] = networkConfig.password;

  String output;
  serializeJson(doc, output);
  return output;
};

String JSONSerializer::serializeNetworks(const Network networks[], int size)
{
  JsonDocument doc;
  JsonArray array = doc.to<JsonArray>();

  for (int i = 0; i < size; i++)
  {
    if (strcmp(networks[i].SSID, "") == 0)
    {
      // Empty remote
      continue;
    }
    JsonObject object = array.add<JsonObject>();
    object["ssid"] = networks[i].SSID;
    object["rssi"] = networks[i].RSSI;
  }

  String output;
  serializeJson(doc, output);
  return output;
};

String JSONSerializer::serializeSystemInfos(const SystemInfos& infos)
{
  JsonDocument doc;
  JsonObject object = doc.to<JsonObject>();

  object["version"] = infos.version;

  String output;
  serializeJson(doc, output);
  return output;
}

String JSONSerializer::serializeSystemInfos(const SystemInfosExtended& infos)
{
  JsonDocument doc;
  JsonObject object = doc.to<JsonObject>();

  object["version"] = infos.version;
  object["mac"] = infos.macAddress;
  object["ip"] = infos.ipAddress;

  String output;
  serializeJson(doc, output);
  return output;
}

String JSONSerializer::serializeMQTTConfig(const MQTTConfiguration& mqttConfig)
{
  JsonDocument doc;
  JsonObject object = doc.to<JsonObject>();

  object["enabled"] = mqttConfig.enabled;
  object["broker"] = mqttConfig.broker;
  object["port"] = mqttConfig.port;
  object["username"] = mqttConfig.username;
  object["password"] = mqttConfig.password;

  String output;
  serializeJson(doc, output);
  return output;
}

// PRIVATE

void JSONSerializer::serializeRemote(JsonObject object, const Remote& remote)
{
  object["id"] = remote.id;
  object["rolling_code"] = remote.rollingCode;
  object["name"] = remote.name;
};