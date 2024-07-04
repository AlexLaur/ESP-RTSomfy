/**
 * @file controller.cpp
 * @author Laurette Alexandre
 * @brief Implementation of the controller of the application.
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

#include <controller.h>

#include <config.h>
#include <remote.h>
#include <result.h>
#include <networks.h>
#include <systemInfos.h>
#include <databaseAbs.h>
#include <serializerAbs.h>
#include <transmitterAbs.h>
#include <networkClientAbs.h>

Controller::Controller(DatabaseAbstract* database, NetworkClientAbstract* networkClient,
    TransmitterAbstract* transmitter, SystemManagerAbstract* systemManager)
    : m_database(database)
    , m_networkClient(networkClient)
    , m_transmitter(transmitter)
    , m_systemManager(systemManager)
{
}

Result<SystemInfosExtended> Controller::fetchSystemInfos()
{
  LOG_DEBUG("Fetching System informations...");
  Result<SystemInfosExtended> result;

  SystemInfos infos = this->m_database->getSystemInfos();
  String macAddress = this->m_networkClient->getMacAddress();
  String ipAddress = this->m_networkClient->getIP();

  SystemInfosExtended infosExtended;
  infosExtended.macAddress = macAddress;
  infosExtended.ipAddress = ipAddress;
  strcpy(infosExtended.version, infos.version);

  result.isSuccess = true;
  result.data = infosExtended;
  return result;
}

Result<String> Controller::askSystemRestart()
{
  LOG_DEBUG("Asking to restart system...");

  Result<String> result = { "Restart requested.", String(), true };

  this->m_systemManager->requestRestart();

  return result;
}

Result<Remote> Controller::fetchRemote(const unsigned long id)
{
  LOG_DEBUG("Fetching Remote...");
  Result<Remote> result;
  result.data = Remote { 0, 0, "" };
  if (id == 0)
  {
    LOG_ERROR("The remote id is not specified.");
    result.errorMsg = "The remote id is not specified.";
    return result;
  }

  Remote remote = this->m_database->getRemote(id);

  if (remote.id == 0)
  {
    LOG_ERROR("This remote doesn't exists.");
    result.errorMsg = "This remote doesn't exists.";
    return result;
  }

  result.isSuccess = true;
  result.data = remote;
  LOG_DEBUG("Remote fetched.");
  return result;
}

Result<Remote[MAX_REMOTES]> Controller::fetchAllRemotes()
{
  LOG_DEBUG("Fetching all remotes...");
  Remote remotes[MAX_REMOTES];
  this->m_database->getAllRemotes(remotes);

  Result<Remote[MAX_REMOTES]> result;
  for (unsigned short i = 0; i < MAX_REMOTES; ++i)
  {
    result.data[i].id = remotes[i].id;
    strcpy(result.data[i].name, remotes[i].name);
    result.data[i].rollingCode = remotes[i].rollingCode;
  }
  result.errorMsg = "";
  result.isSuccess = true;

  LOG_DEBUG("All Remotes fetched.");
  return result;
}

Result<Remote> Controller::createRemote(const char* name)
{
  LOG_DEBUG("Creating a new Remote...");
  Result<Remote> result;
  result.data = Remote { 0, 0, "" };
  if (name == nullptr)
  {
    LOG_ERROR("The name of the remote should be specified.");
    result.errorMsg = "The name of the remote should be specified.";
    return result;
  }

  if (strlen(name) == 0)
  {
    LOG_ERROR("The name of the remote cannot be empty.");
    result.errorMsg = "The name of the remote cannot be empty.";
    return result;
  }

  if (strlen(name) > MAX_REMOTE_NAME_LENGTH)
  {
    LOG_ERROR("The name is too long.");
    String error = "The name is too long. It can contain only " + String(MAX_REMOTE_NAME_LENGTH - 1)
        + " chars.";
    result.errorMsg = error;
    return result;
  }

  Remote remote = this->m_database->createRemote(name);

  if (remote.id == 0)
  {
    LOG_ERROR(
        "The created remote is an empty remote. No space left on the device for a new remote.");
    result.errorMsg = "No space left on the device for a new remote.";
    return result;
  }

  result.isSuccess = true;
  result.data = remote;

  this->notify("remote-create", remote);

  LOG_DEBUG("Remote created.");
  return result;
}

Result<Remote> Controller::deleteRemote(const unsigned long id)
{
  LOG_DEBUG("Deleting Remote...");
  Result<Remote> result;
  result.data = Remote { 0, 0, "" };
  if (id == 0)
  {
    LOG_ERROR("The remote id is not specified.");
    result.errorMsg = "The remote id is not specified.";
    return result;
  }

  Remote remote = this->m_database->getRemote(id);
  if (remote.id == 0)
  {
    LOG_ERROR("The given remote doesn't exist in the database.");
    result.errorMsg = "The given remote doesn't exist in the database.";
    return result;
  }

  bool isDeleted = this->m_database->deleteRemote(id);

  if (!isDeleted)
  {
    LOG_ERROR("The given remote doesn't exist in the database.");
    result.errorMsg = "The given remote doesn't exist in the database.";
    return result;
  }

  result.isSuccess = true;
  result.data = remote;

  this->notify("remote-delete", remote);

  LOG_DEBUG("Remote deleted.");
  return result;
}

Result<Remote> Controller::updateRemote(
    const unsigned long id, const char* name, const unsigned int rollingCode)
{
  LOG_DEBUG("Updating Remote...");
  Result<Remote> result;
  result.data = Remote { 0, 0, "" };
  if (id == 0)
  {
    LOG_ERROR("The remote id should be specified.");
    result.errorMsg = "The remote id should be specified.";
    return result;
  }

  Remote remote = this->m_database->getRemote(id);

  if (remote.id == 0)
  {
    LOG_ERROR("The remote doesn't exist. It cannot be updated.");
    result.errorMsg = "The remote doesn't exist. It cannot be updated.";
    return result;
  }

  if (name != nullptr)
  {
    if (strlen(name) > MAX_REMOTE_NAME_LENGTH)
    {
      LOG_ERROR("The name is too long.");
      String error = "The name is too long. It can contain only "
          + String(MAX_REMOTE_NAME_LENGTH - 1) + " chars.";
      result.errorMsg = error;
      return result;
    }
    if (strlen(name) != 0)
    {
      LOG_DEBUG("The name of the remote will be updated.");
      strcpy(remote.name, name);
    }
  }
  else
  {
    LOG_DEBUG("The name of the remote is not specified. It will not change.");
  }

  if (rollingCode != 0)
  {
    LOG_DEBUG("The rolling code of the remote will be updated.");
    // TODO. We want this ?
    LOG_WARN("The rolling code cannot be updated yet.");
  }
  else
  {
    LOG_DEBUG("The rolling code of the remote is not specified. It will not change.");
  }

  bool isUpdated = this->m_database->updateRemote(remote);

  if (!isUpdated)
  {
    LOG_ERROR("Failed to update the remote.");
    result.errorMsg = "Something went wrong while updating the remote.";
    return result;
  }

  result.isSuccess = true;
  result.data = remote;

  this->notify("remote-update", remote);

  LOG_DEBUG("Remote updated.");
  return result;
}

Result<String> Controller::operateRemote(const unsigned long id, const char* action)
{
  LOG_INFO("Operating a command with the Remote", id);
  Result<String> result;
  if (id == 0)
  {
    LOG_ERROR("The remote id should be specified.");
    result.errorMsg = "The remote id should be specified.";
    return result;
  }

  if (action == nullptr)
  {
    LOG_ERROR("The action should be specified. Allowed actions: up, down, stop, pair, reset.");
    result.errorMsg
        = "The action should be specified. Allowed actions: up, down, stop, pair, reset.";
    return result;
  }

  if (strlen(action) == 0)
  {
    LOG_ERROR("The action should be specified. Allowed actions: up, down, stop, pair, reset.");
    result.errorMsg
        = "The action should be specified. Allowed actions: up, down, stop, pair, reset.";
    return result;
  }

  Remote remote = this->m_database->getRemote(id);

  if (remote.id == 0)
  {
    LOG_ERROR("The remote doesn't exist. It cannot be operate.");
    result.errorMsg = "The remote doesn't exist. It cannot be operate.";
    return result;
  }

  if (strcmp(action, "up") == 0)
  {
    LOG_INFO("Operate 'UP'.");
    this->m_transmitter->sendUpCmd(remote.id, remote.rollingCode);
    this->notify("remote-up", remote);
    result.data = "Command UP sent.";
  }
  else if (strcmp(action, "stop") == 0)
  {
    LOG_INFO("Operate 'STOP'.");
    this->m_transmitter->sendStopCmd(remote.id, remote.rollingCode);
    this->notify("remote-stop", remote);
    result.data = "Command STOP sent.";
  }
  else if (strcmp(action, "down") == 0)
  {
    LOG_INFO("Operate 'DOWN'.");
    this->m_transmitter->sendDownCmd(remote.id, remote.rollingCode);
    this->notify("remote-down", remote);
    result.data = "Command DOWN sent.";
  }
  else if (strcmp(action, "pair") == 0)
  {
    LOG_INFO("Operate 'PAIR'.");
    this->m_transmitter->sendProgCmd(remote.id, remote.rollingCode);
    this->notify("remote-pair", remote);
    result.data = "Command PAIR sent.";
  }
  else if (strcmp(action, "reset") == 0)
  {
    LOG_INFO("Operate 'RESET'.");
    remote.rollingCode = 0;
    this->notify("remote-reset", remote);
    this->m_database->updateRemote(remote);
    this->notify("remote-update", remote);
    result.isSuccess = true;
    result.data = "Rolling code reseted.";
    return result;
  }
  else
  {
    LOG_WARN("The action is not valid.");
    result.errorMsg = "The action is not valid. Allowed actions: up, down, stop, pair, reset.";
    return result;
  }

  result.isSuccess = true;
  remote.rollingCode += 1; // increment rollingCode
  this->m_database->updateRemote(remote);

  this->notify("remote-update", remote);

  LOG_INFO("Command sent through the remote", remote.id);
  return result;
}

Result<Network[MAX_NETWORK_SCAN]> Controller::fetchScannedNetworks()
{
  LOG_DEBUG("Fetching scanned Networks...");
  Result<Network[MAX_NETWORK_SCAN]> result;

  this->m_networkClient->getNetworks(result.data);
  result.isSuccess = true;

  return result;
}

Result<NetworkConfiguration> Controller::fetchNetworkConfiguration()
{
  LOG_DEBUG("Fetching Network Configuration...");
  Result<NetworkConfiguration> result;

  result.data = this->m_database->getNetworkConfiguration();
  result.isSuccess = true;

  return result;
}

Result<NetworkConfiguration> Controller::updateNetworkConfiguration(
    const char* ssid, const char* password)
{
  LOG_DEBUG("Updating Network Configuration...");
  Result<NetworkConfiguration> result;
  strcpy(result.data.ssid, "");
  strcpy(result.data.password, "");

  if (ssid == nullptr)
  {
    LOG_ERROR("The ssid should be specified.");
    result.errorMsg = "The ssid should be specified.";
    return result;
  }

  if (strlen(ssid) == 0)
  {
    LOG_ERROR("The ssid cannot be empty.");
    result.errorMsg = "The ssid cannot be empty.";
    return result;
  }

  strcpy(result.data.ssid, ssid);

  if (password == nullptr)
  {
    LOG_DEBUG("No password provided. Probably a free hotspot.");
  }
  else
  {
    strcpy(result.data.password, password);
  }

  bool isUpdated = this->m_database->setNetworkConfiguration(result.data);

  if (!isUpdated)
  {
    LOG_ERROR("Something went wrong while updating the Network Configuration");
    result.errorMsg = "Something went wrong while updating the Network Configuration";
    return result;
  }

  result.isSuccess = true;
  LOG_DEBUG("Network Configuration updated.");
  return result;
}

Result<MQTTConfiguration> Controller::fetchMQTTConfiguration()
{
  LOG_DEBUG("Fetching MQTT Configuration...");
  Result<MQTTConfiguration> result;

  result.data = this->m_database->getMQTTConfiguration();
  result.isSuccess = true;

  return result;
}

Result<MQTTConfiguration> Controller::updateMQTTConfiguration(const bool& enabled,
    const char* broker, const unsigned short& port, const char* username, const char* password)
{
  LOG_DEBUG("Updating MQTT Configuration...");
  Result<MQTTConfiguration> result;
  result.data = MQTTConfiguration { false, "", 0, "", "" };

  if (broker == nullptr)
  {
    LOG_ERROR("The broker should be specified.");
    result.errorMsg = "The broker should be specified.";
    return result;
  }

  if (port == 0)
  {
    LOG_ERROR("The port should be specified. It cannot be equal to 0.");
    result.errorMsg = "The broker should be specified. It cannot be equal to 0.";
    return result;
  }

  result.data.enabled = enabled;

  if (strlen(broker) == 0)
  {
    LOG_WARN("The broker is empty. The MQTT will be disabled.");
    result.data.enabled = false;
  }

  strcpy(result.data.broker, broker);
  result.data.port = port;

  if (username == nullptr)
  {
    LOG_DEBUG("No username provided.");
  }
  else
  {
    strcpy(result.data.username, username);
  }

  if (password == nullptr)
  {
    LOG_DEBUG("No username provided.");
  }
  else
  {
    strcpy(result.data.password, password);
  }

  bool isUpdated = this->m_database->setMQTTConfiguration(result.data);
  if (!isUpdated)
  {
    LOG_ERROR("Something went wrong while updating the MQTT Configuration");
    result.errorMsg = "Something went wrong while updating the MQTT Configuration";
    return result;
  }

  result.isSuccess = true;
  LOG_DEBUG("MQTT Configuration updated.");
  return result;
}
