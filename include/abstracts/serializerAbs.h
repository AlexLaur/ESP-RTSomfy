/**
 * @file serializer.h
 * @author Laurette Alexandre
 * @brief Header of Serializer abstraction.
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
#include <remote.h>
#include <networks.h>
#include <mqttConfig.h>
#include <systemInfos.h>

class SerializerAbstract
{
  public:
  virtual String serializeMessage(const char* message) = 0;
  virtual String serializeRemote(const Remote& remote) = 0;
  virtual String serializeRemotes(const Remote remotes[], int size) = 0;
  virtual String serializeNetworkConfig(const NetworkConfiguration& networkConfig) = 0;
  virtual String serializeNetworks(const Network networks[], int size) = 0;
  virtual String serializeSystemInfos(const SystemInfos& infos) = 0;
  virtual String serializeSystemInfos(const SystemInfosExtended& infos) = 0;
  virtual String serializeMQTTConfig(const MQTTConfiguration& mqttConfig) = 0;
};