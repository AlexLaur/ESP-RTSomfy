/**
 * @file mqttClient.cpp
 * @author Laurette Alexandre
 * @brief Implementation for MQTT client.
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
#include <regex>
#include <Arduino.h>
#include <DebugLog.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <config.h>
#include <result.h>
#include <remote.h>
#include <controller.h>
#include <mqttClient.h>
#include <mqttConfig.h>
#include <serializerAbs.h>

WiFiClient espClient;
PubSubClient pubSubClient(espClient);

MQTTClient* MQTTClient::m_instance = nullptr;

MQTTClient::MQTTClient(Controller* controller, SerializerAbstract* serializer)
    : m_controller(controller)
    , m_serializer(serializer)
{
  this->m_instance = this;
}

MQTTClient* MQTTClient::getInstance() { return MQTTClient::m_instance; }

bool MQTTClient::connect(const MQTTConfiguration& conf)
{
  pubSubClient.setServer(conf.broker, conf.port);
  String clientId = this->getClientIdentifier();

  if (pubSubClient.connect(clientId.c_str(), conf.username, conf.password))
  {
    LOG_INFO("MQTT client started.");

    LOG_DEBUG("Publishing basic informations...");
    Result<SystemInfosExtended> resultInfos = this->m_controller->fetchSystemInfos();
    pubSubClient.publish("esprtsomfy/system/infos/version", resultInfos.data.version);
    pubSubClient.publish("esprtsomfy/system/infos/mac", resultInfos.data.macAddress.c_str());
    pubSubClient.publish("esprtsomfy/system/infos/ip", resultInfos.data.ipAddress.c_str());

    Result<Remote[MAX_REMOTES]> resultRemotes = this->m_controller->fetchAllRemotes();
    char topic[50];
    for (unsigned short i = 0; i < MAX_REMOTES; ++i)
    {
      if (resultRemotes.data[i].id == 0)
      {
        // Skip empty remotes
        continue;
      }
      sprintf(topic, "esprtsomfy/remotes/%lu/rolling_code", resultRemotes.data[i].id);
      pubSubClient.publish(topic, String(resultRemotes.data[i].rollingCode).c_str());
      sprintf(topic, "esprtsomfy/remotes/%lu/name", resultRemotes.data[i].id);
      pubSubClient.publish(topic, resultRemotes.data[i].name);
    }

    LOG_DEBUG("Subscribing to topics...");
    pubSubClient.setCallback(MQTTClient::receive);
    pubSubClient.subscribe("esprtsomfy/remotes/+/set/name");
    pubSubClient.subscribe("esprtsomfy/remotes/+/set/action");
  }
  else
  {
    LOG_ERROR("Cannot connect to the MQTT broker.");
    return false;
  }
  return true;
}

void MQTTClient::handleMessages()
{
  if (!pubSubClient.connected())
  {
    return;
  }
  pubSubClient.loop();
}

bool MQTTClient::isConnected() { return pubSubClient.connected(); }

void MQTTClient::notified(const char* action, const Remote& remote)
{
  if (!this->isConnected())
  {
    LOG_ERROR("MQTT Client is not connected. Nothing can be published.");
    return;
  }
  char topic[50];
  if (strcmp(action, "remote-update") == 0 || strcmp(action, "remote-create") == 0)
  {
    LOG_INFO("Remote create/update catched.");
    sprintf(topic, "esprtsomfy/remotes/%lu/rolling_code", remote.id);
    pubSubClient.publish(topic, String(remote.rollingCode).c_str());
    sprintf(topic, "esprtsomfy/remotes/%lu/name", remote.id);
    pubSubClient.publish(topic, remote.name);
    return;
  }

  if (strcmp(action, "remote-up") == 0 || strcmp(action, "remote-stop") == 0
      || strcmp(action, "remote-down") == 0 || strcmp(action, "remote-pair") == 0
      || strcmp(action, "remote-reset") == 0)
  {
    LOG_INFO("Remote command catched.");
    sprintf(topic, "esprtsomfy/remotes/%lu/last_action", remote.id);
    String command = String(action).substring(7);
    pubSubClient.publish(topic, command.c_str());
    return;
  }

  if (strcmp(action, "remote-delete") == 0)
  {
    sprintf(topic, "esprtsomfy/remotes/%lu/rolling_code", remote.id);
    pubSubClient.publish(topic, "NA");
    sprintf(topic, "esprtsomfy/remotes/%lu/name", remote.id);
    pubSubClient.publish(topic, "NA");
    sprintf(topic, "esprtsomfy/remotes/%lu/last_action", remote.id);
    pubSubClient.publish(topic, "NA");
  }
}

// PRIVATE

/**
 * @brief Called when a message arrived. TODO refacto this part. Some code is
 * ugly, but this part was sooo boring.
 *
 * @param topic The topic
 * @param payloadByte The payload received
 * @param length Lenght of the payload
 */
void MQTTClient::receive(const char* topic, byte* payloadByte, uint32_t length)
{
  LOG_DEBUG("Message arrived on topic: ", topic);

  MQTTClient* instance = MQTTClient::getInstance();

  // Inspired by:
  // https://github.com/me-no-dev/ESPAsyncWebServer/blob/7f3753454b1f176c4b6d6bcd1587a135d95ca63c/src/WebHandlerImpl.h#L94
  std::regex remoteIdPattern(R"(/(\d+)/)");
  std::smatch matches;
  std::string s(topic);
  if (!std::regex_search(s, matches, remoteIdPattern))
  {
    LOG_ERROR("Cannot extract remote ID.");
    return;
  }

  // Assume that we only have one match. We should never trust users ?!
  unsigned long remoteId = strtoul(matches[1].str().c_str(), nullptr, 10);

  // Get payload from payloadByte
  String payload = "";
  for (unsigned int i = 0; i < length; i++)
  {
    payload += (char)payloadByte[i];
  }

  // Get endpoint
  String endpoint = String(topic);
  String lastElement;
  int lastSlashPos = endpoint.lastIndexOf('/');
  if (lastSlashPos != -1)
  {
    lastElement = endpoint.substring(lastSlashPos + 1);
  }
  else
  {
    LOG_ERROR("Something went wrong. The topic is ambigous.");
    return;
  }

  if (lastElement == "action")
  {
    // Perform a command
    Result<String> result = instance->m_controller->operateRemote(remoteId, payload.c_str());
    if (!result.isSuccess)
    {
      LOG_ERROR(result.errorMsg);
      return;
    }
    LOG_INFO(result.data);
  }

  else if (lastElement == "name")
  {
    // Change name
    Result<Remote> result = instance->m_controller->updateRemote(remoteId, payload.c_str(), 0);
    if (!result.isSuccess)
    {
      LOG_ERROR(result.errorMsg);
      return;
    }
  }
}

String MQTTClient::getClientIdentifier()
{
  String clientId = String(APP_NAME);
  clientId += String(random(0xffff), HEX);
  return clientId;
}