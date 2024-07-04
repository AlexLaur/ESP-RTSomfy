/**
 * @file mqttClient.h
 * @author Laurette Alexandre
 * @brief Header for MQTT.
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
#pragma once

#include <Arduino.h>
#include <PubSubClient.h>

#include <remote.h>
#include <observer.h>
#include <controller.h>
#include <mqttConfig.h>
#include <serializerAbs.h>

void callback(char* topic, byte* payload, unsigned int length);

class MQTTClient : public Observer
{
  public:
  MQTTClient(Controller* controller, SerializerAbstract* serializer);
  static MQTTClient* getInstance();
  bool connect(const MQTTConfiguration& conf);
  void handleMessages();
  bool isConnected();

  void notified(const char* action, const Remote& remote); // from observer

  private:
  static MQTTClient* m_instance;
  Controller* m_controller = nullptr;
  SerializerAbstract* m_serializer;

  static void receive(const char* topic, byte* payload, uint32_t length);
  String getClientIdentifier();
};