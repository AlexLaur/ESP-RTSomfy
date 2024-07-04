/**
 * @file controller.h
 * @author Laurette Alexandre
 * @brief Header of the controller of the application.
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

#include <result.h>
#include <observer.h>
#include <databaseAbs.h>
#include <serializerAbs.h>
#include <transmitterAbs.h>
#include <systemManagerAbs.h>
#include <networkClientAbs.h>

class Controller : public Subject
{
  public:
  Controller(DatabaseAbstract* database, NetworkClientAbstract* networkClient, TransmitterAbstract* transmitter, SystemManagerAbstract* systemManager);

  Result<SystemInfosExtended> fetchSystemInfos();
  Result<String> askSystemRestart();

  Result<Remote> fetchRemote(const unsigned long id);
  Result<Remote[MAX_REMOTES]> fetchAllRemotes();
  Result<Remote> createRemote(const char* name);
  Result<Remote> deleteRemote(const unsigned long id);
  Result<Remote> updateRemote(const unsigned long id, const char* name, const unsigned int rollingCode);
  Result<String> operateRemote(const unsigned long id, const char* action);

  Result<Network[MAX_NETWORK_SCAN]> fetchScannedNetworks();
  Result<NetworkConfiguration> fetchNetworkConfiguration();
  Result<NetworkConfiguration> updateNetworkConfiguration(const char* ssid, const char* password);

  Result<MQTTConfiguration> fetchMQTTConfiguration();
  Result<MQTTConfiguration> updateMQTTConfiguration(const bool& enabled, const char* broker, const unsigned short& port, const char* username, const char* password);

  private:
  DatabaseAbstract* m_database;
  NetworkClientAbstract* m_networkClient;
  TransmitterAbstract* m_transmitter;
  SystemManagerAbstract* m_systemManager;
};