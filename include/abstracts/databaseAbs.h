/**
 * @file database.h
 * @author Laurette Alexandre
 * @brief Header of database abstraction.
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

#include <remote.h>
#include <networks.h>
#include <mqttConfig.h>
#include <systemInfos.h>

class DatabaseAbstract
{
  public:
  virtual void init() = 0;

  virtual SystemInfos getSystemInfos() = 0;

  virtual NetworkConfiguration getNetworkConfiguration() = 0;
  virtual bool setNetworkConfiguration(const NetworkConfiguration& networkConfig) = 0;
  virtual void resetNetworkConfiguration() = 0;

  // CRUD methods for remote
  virtual Remote createRemote(const char* name) = 0;
  virtual void getAllRemotes(Remote remotes[]) = 0;
  virtual Remote getRemote(const unsigned long& id) = 0;
  virtual bool updateRemote(const Remote& remote) = 0;
  virtual bool deleteRemote(const unsigned long& id) = 0;

  virtual MQTTConfiguration getMQTTConfiguration() = 0;
  virtual bool setMQTTConfiguration(const MQTTConfiguration& mqttConfig) = 0;
};