/**
 * @file eepromDatabase.h
 * @author Laurette Alexandre
 * @brief Header of EEPROM database storage system.
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

#include <networks.h>
#include <remote.h>
#include <mqttConfig.h>
#include <systemInfos.h>
#include <databaseAbs.h>

class EEPROMDatabase : public DatabaseAbstract
{
  public:
  EEPROMDatabase();
  EEPROMDatabase(unsigned long remoteBaseAddress);
  void init();
  void fixIntegrity();

  // SystemInfos
  SystemInfos getSystemInfos();

  // Network Configuration
  NetworkConfiguration getNetworkConfiguration();
  bool setNetworkConfiguration(const NetworkConfiguration& networkConfig);
  void resetNetworkConfiguration();

  // Remotes CRUD
  Remote createRemote(const char* name);
  void getAllRemotes(Remote remotes[]);
  Remote getRemote(const unsigned long& id);
  bool updateRemote(const Remote& remote);
  bool deleteRemote(const unsigned long& id);

  // MQTT Configuration
  MQTTConfiguration getMQTTConfiguration();
  bool setMQTTConfiguration(const MQTTConfiguration& mqttConfig);

  private:
  int m_lastSystemInfosAddressStart = 0;
  int m_networkConfigAddressStart = sizeof(SystemInfos);
  int m_mqttConfigAddressStart = sizeof(SystemInfos) + sizeof(NetworkConfiguration);
  int m_remotesAddressStart = sizeof(SystemInfos) + sizeof(NetworkConfiguration) + sizeof(MQTTConfiguration);
  unsigned long m_remoteBaseAddress = REMOTE_BASE_ADDRESS;

  bool migrate();
  bool stringIsAscii(const char* data);
  int getRemoteIndex(const unsigned long& id);

  // Migrations
  void applyUpdate_2_1_0();
};