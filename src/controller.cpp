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
    SerializerAbstract* serializer, TransmitterAbstract* transmitter,
    SystemManagerAbstract* systemManager)
    : m_database(database)
    , m_networkClient(networkClient)
    , m_serializer(serializer)
    , m_transmitter(transmitter)
    , m_systemManager(systemManager)
{
}

Result Controller::fetchSystemInfos()
{
  LOG_DEBUG("Fetching System informations...");
  Result result;

  SystemInfos infos = this->m_database->getSystemInfos();
  String macAddress = this->m_networkClient->getMacAddress();

  SystemInfosExtended infosExtended;
  infosExtended.macAddress = macAddress;
  strcpy(infosExtended.version, infos.version);

  result.isSuccess = true;
  String serialized = this->m_serializer->serializeSystemInfos(infosExtended);
  result.data = serialized;

  return result;
}

Result Controller::askSystemRestart()
{
  LOG_DEBUG("Asking to restart system...");

  String serialized = this->m_serializer->serializeMessage("Restart requested.");

  Result result = { serialized, String(), true };

  this->m_systemManager->requestRestart();

  return result;
}

Result Controller::fetchRemote(const unsigned long id)
{
  LOG_DEBUG("Fetching Remote...");
  Result result;
  if (id == 0)
  {
    LOG_ERROR("The remote id is not specified.");
    result.error = "The remote id is not specified.";
    return result;
  }

  Remote remote = this->m_database->getRemote(id);

  if (remote.id == 0)
  {
    LOG_ERROR("This remote doesn't exists.");
    result.error = "This remote doesn't exists.";
    return result;
  }

  result.isSuccess = true;
  String serialized = this->m_serializer->serializeRemote(remote);
  result.data = serialized;
  LOG_DEBUG("Remote fetched.");
  return result;
}

Result Controller::fetchAllRemotes()
{
  LOG_DEBUG("Fetching all remotes...");
  Remote remotes[MAX_REMOTES];
  this->m_database->getAllRemotes(remotes);

  String serialized = this->m_serializer->serializeRemotes(remotes, MAX_REMOTES);

  Result result = { serialized, "", true };

  LOG_DEBUG("All Remotes fetched.");
  return result;
}

Result Controller::createRemote(const char* name)
{
  LOG_DEBUG("Creating a new Remote...");
  Result result;
  if (name == nullptr)
  {
    LOG_ERROR("The name of the remote should be specified.");
    result.error = "The name of the remote should be specified.";
    return result;
  }

  if (strlen(name) == 0)
  {
    LOG_ERROR("The name of the remote cannot be empty.");
    result.error = "The name of the remote cannot be empty.";
    return result;
  }

  if (strlen(name) > MAX_REMOTE_NAME_LENGTH)
  {
    LOG_ERROR("The name is too long.");
    String error = "The name is too long. It can contain only " + String(MAX_REMOTE_NAME_LENGTH - 1)
        + " chars.";
    result.error = error;
    return result;
  }

  Remote remote = this->m_database->createRemote(name);

  if (remote.id == 0)
  {
    LOG_ERROR(
        "The created remote is an empty remote. No space left on the device for a new remote.");
    result.error = "No space left on the device for a new remote.";
    return result;
  }

  result.isSuccess = true;
  String serialized = this->m_serializer->serializeRemote(remote);
  result.data = serialized;

  this->notify("remote-create", serialized.c_str());

  LOG_DEBUG("Remote created.");
  return result;
}

Result Controller::deleteRemote(const unsigned long id)
{
  LOG_DEBUG("Deleting Remote...");
  Result result;
  if (id == 0)
  {
    LOG_ERROR("The remote id is not specified.");
    result.error = "The remote id is not specified.";
    return result;
  }

  Remote remote = this->m_database->getRemote(id);
  if (remote.id == 0)
  {
    LOG_ERROR("The given remote doesn't exist in the database.");
    result.error = "The given remote doesn't exist in the database.";
    return result;
  }

  bool isDeleted = this->m_database->deleteRemote(id);

  if (!isDeleted)
  {
    LOG_ERROR("The given remote doesn't exist in the database.");
    result.error = "The given remote doesn't exist in the database.";
    return result;
  }

  result.isSuccess = true;
  String serialized = this->m_serializer->serializeRemote(remote);
  result.data = serialized;

  this->notify("remote-delete", serialized.c_str());

  LOG_DEBUG("Remote deleted.");
  return result;
}

Result Controller::updateRemote(
    const unsigned long id, const char* name, const unsigned int rollingCode)
{
  LOG_DEBUG("Updating Remote...");
  Result result;
  if (id == 0)
  {
    LOG_ERROR("The remote id should be specified.");
    result.error = "The remote id should be specified.";
    return result;
  }

  Remote remote = this->m_database->getRemote(id);

  if (remote.id == 0)
  {
    LOG_ERROR("The remote doesn't exist. It cannot be updated.");
    result.error = "The remote doesn't exist. It cannot be updated.";
    return result;
  }

  if (name != nullptr)
  {
    if (strlen(name) > MAX_REMOTE_NAME_LENGTH)
    {
      LOG_ERROR("The name is too long.");
      String error = "The name is too long. It can contain only "
          + String(MAX_REMOTE_NAME_LENGTH - 1) + " chars.";
      result.error = error;
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
    result.error = "Something went wrong while updating the remote.";
    return result;
  }

  result.isSuccess = true;
  String serialized = this->m_serializer->serializeRemote(remote);
  result.data = serialized;

  this->notify("remote-update", serialized.c_str());

  LOG_DEBUG("Remote updated.");
  return result;
}

Result Controller::operateRemote(const unsigned long id, const char* action)
{
  LOG_INFO("Operating a command with the Remote", id);
  Result result;
  if (id == 0)
  {
    LOG_ERROR("The remote id should be specified.");
    result.error = "The remote id should be specified.";
    return result;
  }

  if (action == nullptr)
  {
    LOG_ERROR("The action should be specified. Allowed actions: up, down, stop, pair, reset.");
    result.error = "The action should be specified. Allowed actions: up, down, stop, pair, reset.";
    return result;
  }

  if (strlen(action) == 0)
  {
    LOG_ERROR("The action should be specified. Allowed actions: up, down, stop, pair, reset.");
    result.error = "The action should be specified. Allowed actions: up, down, stop, pair, reset.";
    return result;
  }

  Remote remote = this->m_database->getRemote(id);

  if (remote.id == 0)
  {
    LOG_ERROR("The remote doesn't exist. It cannot be operate.");
    result.error = "The remote doesn't exist. It cannot be operate.";
    return result;
  }

  String serialized = this->m_serializer->serializeRemote(remote);

  if (strcmp(action, "up") == 0)
  {
    LOG_INFO("Operate 'UP'.");
    this->m_transmitter->sendUpCmd(remote.id, remote.rollingCode);
    this->notify("remote-up", serialized.c_str());
    result.data = "Command UP sent.";
  }
  else if (strcmp(action, "stop") == 0)
  {
    LOG_INFO("Operate 'STOP'.");
    this->m_transmitter->sendStopCmd(remote.id, remote.rollingCode);
    this->notify("remote-stop", serialized.c_str());
    result.data = "Command STOP sent.";
  }
  else if (strcmp(action, "down") == 0)
  {
    LOG_INFO("Operate 'DOWN'.");
    this->m_transmitter->sendDownCmd(remote.id, remote.rollingCode);
    this->notify("remote-down", serialized.c_str());
    result.data = "Command DOWN sent.";
  }
  else if (strcmp(action, "pair") == 0)
  {
    LOG_INFO("Operate 'PAIR'.");
    this->m_transmitter->sendProgCmd(remote.id, remote.rollingCode);
    this->notify("remote-pair", serialized.c_str());
    result.data = "Command PAIR sent.";
  }
  else if (strcmp(action, "reset") == 0)
  {
    LOG_INFO("Operate 'RESET'.");
    remote.rollingCode = 0;
    this->m_database->updateRemote(remote);
    result.isSuccess = true;
    result.data = "Rolling code reseted.";
    this->notify("remote-reset", serialized.c_str());
    return result;
  }
  else
  {
    LOG_WARN("The action is not valid.");
    result.error = "The action is not valid. Allowed actions: up, down, stop, pair, reset.";
    return result;
  }

  result.isSuccess = true;
  remote.rollingCode += 1; // increment rollingCode
  this->m_database->updateRemote(remote);

  serialized = this->m_serializer->serializeRemote(remote);
  this->notify("remote-update", serialized.c_str());

  LOG_INFO("Command sent through the remote", remote.id);
  return result;
}

Result Controller::fetchNetworkConfiguration()
{
  LOG_DEBUG("Fetching Network Configuration...");
  Result result;

  NetworkConfiguration networkConfig = this->m_database->getNetworkConfiguration();

  result.isSuccess = true;
  String serialized = this->m_serializer->serializeNetworkConfig(networkConfig);
  result.data = serialized;
  return result;
}

Result Controller::updateNetworkConfiguration(const char* ssid, const char* password)
{
  LOG_DEBUG("Updating Network Configuration...");
  Result result;
  if (ssid == nullptr)
  {
    LOG_ERROR("The ssid should be specified.");
    result.error = "The ssid should be specified.";
    return result;
  }

  if (strlen(ssid) == 0)
  {
    LOG_ERROR("The ssid cannot be empty.");
    result.error = "The ssid cannot be empty.";
    return result;
  }

  NetworkConfiguration networkConfig;
  strcpy(networkConfig.ssid, ssid);

  if (password == nullptr)
  {
    LOG_DEBUG("No password provided. Probably a free hotspot.");
  }
  else
  {
    strcpy(networkConfig.password, password);
  }

  bool isUpdated = this->m_database->setNetworkConfiguration(networkConfig);

  if (!isUpdated)
  {
    LOG_ERROR("Something went wrong while updating the Network Configuration");
    result.error = "Something went wrong while updating the Network Configuration";
    return result;
  }

  result.isSuccess = true;
  String serialized = this->m_serializer->serializeNetworkConfig(networkConfig);
  result.data = serialized;
  LOG_DEBUG("Network Configuration updated.");
  return result;
}

Result Controller::fetchMQTTConfiguration()
{
  LOG_DEBUG("Fetching MQTT Configuration...");
  Result result;

  MQTTConfiguration mqttConfig = this->m_database->getMQTTConfiguration();

  result.isSuccess = true;
  String serialized = this->m_serializer->serializeMQTTConfig(mqttConfig);
  result.data = serialized;

  return result;
}

Result Controller::updateMQTTConfiguration(const bool& enabled, const char* broker,
    const unsigned short& port, const char* username, const char* password)
{
  LOG_DEBUG("Updating MQTT Configuration...");
  Result result;

  if (broker == nullptr)
  {
    LOG_ERROR("The broker should be specified.");
    result.error = "The broker should be specified.";
    return result;
  }

  if (port == 0)
  {
    LOG_ERROR("The port should be specified. It cannot be equal to 0.");
    result.error = "The broker should be specified. It cannot be equal to 0.";
    return result;
  }

  MQTTConfiguration mqttConfig;
  mqttConfig.enabled = enabled;

  if (strlen(broker) == 0)
  {
    LOG_WARN("The broker is empty. The MQTT will be disabled.");
    mqttConfig.enabled = false;
  }

  strcpy(mqttConfig.broker, broker);
  mqttConfig.port = port;

  if (username == nullptr)
  {
    LOG_DEBUG("No username provided.");
  }
  else
  {
    strcpy(mqttConfig.username, username);
  }

  if (password == nullptr)
  {
    LOG_DEBUG("No username provided.");
  }
  else
  {
    strcpy(mqttConfig.password, password);
  }

  bool isUpdated = this->m_database->setMQTTConfiguration(mqttConfig);
  if (!isUpdated)
  {
    LOG_ERROR("Something went wrong while updating the MQTT Configuration");
    result.error = "Something went wrong while updating the MQTT Configuration";
    return result;
  }

  result.isSuccess = true;
  String serialized = this->m_serializer->serializeMQTTConfig(mqttConfig);
  result.data = serialized;
  LOG_DEBUG("MQTT Configuration updated.");
  return result;
}
