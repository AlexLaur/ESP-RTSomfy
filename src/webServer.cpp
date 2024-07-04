/**
 * @file webServer.cpp
 * @author Laurette Alexandre
 * @brief Implementation of WebServer.
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
#include <LittleFS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <result.h>
#include <remote.h>
#include <controller.h>
#include <webServer.h>
#include <serializerAbs.h>

WebServer* WebServer::m_instance = nullptr;

WebServer::WebServer(
    const unsigned short port, Controller* controller, SerializerAbstract* serializer)
    : m_controller(controller)
    , m_serializer(serializer)
{
  this->m_server = new AsyncWebServer(port);
  this->m_instance = this;
}

WebServer::~WebServer() { delete this->m_server; }

WebServer* WebServer::getInstance() { return WebServer::m_instance; }

void WebServer::setup()
{
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  this->m_server->serveStatic("/", LittleFS, "/");
  this->m_server->on("/", HTTP_GET, handleHTMLHomePage);
  this->m_server->onNotFound(handleHTMLNotFoundPage);

  this->m_server->on("/api/v1/system/restart", HTTP_POST, WebServer::handleSystemRestart);
  this->m_server->on("/api/v1/system/infos", HTTP_GET, WebServer::handleFetchSystemInfos);
  this->m_server->on("/api/v1/wifi/networks", HTTP_GET, WebServer::handleFetchWifiNetworks);
  this->m_server->on("/api/v1/wifi/config", HTTP_GET, WebServer::handleFetchWifiConfiguration);
  this->m_server->on("/api/v1/wifi/config", HTTP_POST, WebServer::handleUpdateWifiConfiguration);
  this->m_server->on("/api/v1/mqtt/config", HTTP_GET, WebServer::handleFetchMQTTConfiguration);
  this->m_server->on("/api/v1/mqtt/config", HTTP_POST, WebServer::handleUpdateMQTTConfiguration);
  this->m_server->on("^\\/api/v1/remotes$", HTTP_GET, WebServer::handleFetchAllRemotes);
  this->m_server->on("^\\/api/v1/remotes$", HTTP_POST, WebServer::handleCreateRemote);
  this->m_server->on("^\\/api/v1/remotes\\/([0-9]+)$", HTTP_GET, WebServer::handleFetchRemote);
  this->m_server->on("^\\/api/v1/remotes\\/([0-9]+)$", HTTP_PATCH, WebServer::handleUpdateRemote);
  this->m_server->on("^\\/api/v1/remotes\\/([0-9]+)$", HTTP_DELETE, WebServer::handleDeleteRemote);
  this->m_server->on(
      "^\\/api/v1/remotes\\/([0-9]+)\\/action$", HTTP_POST, WebServer::handleActionRemote);
  LOG_INFO("Webserver setuped.");
}

void WebServer::begin()
{
  this->m_server->begin();
  LOG_INFO("WebServer started.");
}

void WebServer::notified(const char* action, const Remote& remote)
{
  // Not implemented yet. Not necessary.
}

// ============================================================================
// WEBSERVER CALLBACKS RESTAPI
// ============================================================================
void WebServer::handleSystemRestart(AsyncWebServerRequest* request)
{
  LOG_INFO("Endpoint to restart module reached.");

  WebServer* instance = WebServer::getInstance();
  Result<String> result = instance->m_controller->askSystemRestart();

  if (!result.isSuccess)
  {
    request->send(400, "application/json", "{\"message\":\"" + result.errorMsg + "\"}");
    return;
  }
  request->send(200, "application/json", "{\"message\":\"" + result.data + "\"}");
}

void WebServer::handleFetchSystemInfos(AsyncWebServerRequest* request)
{
  LOG_INFO("Endpoint to fetch system informations reached.");

  WebServer* instance = WebServer::getInstance();
  Result<SystemInfosExtended> result = instance->m_controller->fetchSystemInfos();

  if (!result.isSuccess)
  {
    request->send(400, "application/json", "{\"message\":\"" + result.errorMsg + "\"}");
    return;
  }
  String serialized = instance->m_serializer->serializeSystemInfos(result.data);
  request->send(200, "application/json", serialized);
}

void WebServer::handleFetchWifiNetworks(AsyncWebServerRequest* request)
{
  LOG_INFO("Endpoint to fetch Wifi Networks reached.");

  WebServer* instance = WebServer::getInstance();
  Result<Network[MAX_NETWORK_SCAN]> result = instance->m_controller->fetchScannedNetworks();

  if (!result.isSuccess)
  {
    request->send(400, "application/json", "{\"message\":\"" + result.errorMsg + "\"}");
    return;
  }
  String serialized = instance->m_serializer->serializeNetworks(result.data, MAX_NETWORK_SCAN);
  request->send(200, "application/json", serialized);
}

void WebServer::handleFetchWifiConfiguration(AsyncWebServerRequest* request)
{
  LOG_INFO("Endpoint to fetch Network Configuration reached.");

  WebServer* instance = WebServer::getInstance();
  Result<NetworkConfiguration> result = instance->m_controller->fetchNetworkConfiguration();

  if (!result.isSuccess)
  {
    request->send(400, "application/json", "{\"message\":\"" + result.errorMsg + "\"}");
    return;
  }
  String serialized = instance->m_serializer->serializeNetworkConfig(result.data);
  request->send(200, "application/json", serialized);
}

void WebServer::handleUpdateWifiConfiguration(AsyncWebServerRequest* request)
{
  LOG_INFO("Endpoint to update the Wifi Configuration reached.");

  String ssid;
  String password;
  if (request->hasParam("ssid", true))
  {
    AsyncWebParameter* p = request->getParam("ssid", true);
    ssid = p->value();
  }
  if (request->hasParam("password", true))
  {
    AsyncWebParameter* p = request->getParam("password", true);
    password = p->value();
  }

  WebServer* instance = WebServer::getInstance();
  Result<NetworkConfiguration> result
      = instance->m_controller->updateNetworkConfiguration(ssid.c_str(), password.c_str());

  if (!result.isSuccess)
  {
    request->send(400, "application/json", "{\"message\":\"" + result.errorMsg + "\"}");
    return;
  }
  String serialized = instance->m_serializer->serializeNetworkConfig(result.data);
  request->send(200, "application/json", serialized);
}

void WebServer::handleFetchMQTTConfiguration(AsyncWebServerRequest* request)
{
  LOG_INFO("Endpoint to fetch MQTT Configuration reached.");

  WebServer* instance = WebServer::getInstance();
  Result<MQTTConfiguration> result = instance->m_controller->fetchMQTTConfiguration();

  if (!result.isSuccess)
  {
    request->send(400, "application/json", "{\"message\":\"" + result.errorMsg + "\"}");
    return;
  }
  String serialized = instance->m_serializer->serializeMQTTConfig(result.data);
  request->send(200, "application/json", serialized);
}

void WebServer::handleUpdateMQTTConfiguration(AsyncWebServerRequest* request)
{
  LOG_INFO("Endpoint to update the MQTT Wonfiguration reached.");

  bool enabled = false;
  String broker;
  unsigned short port = 0;
  String username;
  String password;

  if (request->hasParam("broker", true))
  {
    AsyncWebParameter* p = request->getParam("broker", true);
    broker = p->value();
  }
  if (request->hasParam("username", true))
  {
    AsyncWebParameter* p = request->getParam("username", true);
    username = p->value();
  }
  if (request->hasParam("password", true))
  {
    AsyncWebParameter* p = request->getParam("password", true);
    password = p->value();
  }
  if (request->hasParam("port", true))
  {
    AsyncWebParameter* p = request->getParam("port", true);
    port = p->value().toInt();
  }
  if (request->hasParam("enabled", true))
  {
    AsyncWebParameter* p = request->getParam("enabled", true);
    if (p->value() == "true" || p->value() == "True")
    {
      enabled = true;
    }
  }

  WebServer* instance = WebServer::getInstance();
  Result<MQTTConfiguration> result = instance->m_controller->updateMQTTConfiguration(
      enabled, broker.c_str(), port, username.c_str(), password.c_str());

  if (!result.isSuccess)
  {
    request->send(400, "application/json", "{\"message\":\"" + result.errorMsg + "\"}");
    return;
  }
  String serialized = instance->m_serializer->serializeMQTTConfig(result.data);
  request->send(200, "application/json", serialized);
}

void WebServer::handleFetchAllRemotes(AsyncWebServerRequest* request)
{
  LOG_INFO("Endpoint to fetch all remotes reached.");

  WebServer* instance = WebServer::getInstance();
  Result<Remote[MAX_REMOTES]> result = instance->m_controller->fetchAllRemotes();

  if (!result.isSuccess)
  {
    request->send(400, "application/json", "{\"message\":\"" + result.errorMsg + "\"}");
    return;
  }
  String serialized = instance->m_serializer->serializeRemotes(result.data, MAX_REMOTES);
  request->send(200, "application/json", serialized);
}

void WebServer::handleFetchRemote(AsyncWebServerRequest* request)
{
  LOG_INFO("Endpoint to fetch a remote reached.");

  unsigned long remoteId = strtoul(request->pathArg(0).c_str(), nullptr, 10);

  WebServer* instance = WebServer::getInstance();
  Result<Remote> result = instance->m_controller->fetchRemote(remoteId);

  if (!result.isSuccess)
  {
    request->send(400, "application/json", "{\"message\":\"" + result.errorMsg + "\"}");
    return;
  }
  String serialized = instance->m_serializer->serializeRemote(result.data);
  request->send(200, "application/json", serialized);
}

void WebServer::handleCreateRemote(AsyncWebServerRequest* request)
{
  LOG_INFO("Endpoint to create a remote reached.");

  String name;
  if (request->hasParam("name", true))
  {
    AsyncWebParameter* p = request->getParam("name", true);
    name = p->value();
  }

  WebServer* instance = WebServer::getInstance();
  Result<Remote> result = instance->m_controller->createRemote(name.c_str());

  if (!result.isSuccess)
  {
    request->send(400, "application/json", "{\"message\":\"" + result.errorMsg + "\"}");
    return;
  }
  String serialized = instance->m_serializer->serializeRemote(result.data);
  request->send(200, "application/json", serialized);
}

void WebServer::handleUpdateRemote(AsyncWebServerRequest* request)
{
  LOG_INFO("Endpoint to update a remote reached.");

  unsigned long remoteId = strtoul(request->pathArg(0).c_str(), nullptr, 10);

  String name;
  unsigned int rollingCode = 0;

  if (request->hasParam("name", true))
  {
    AsyncWebParameter* p = request->getParam("name", true);
    name = p->value();
  }

  if (request->hasParam("rolling_code", true))
  {
    AsyncWebParameter* p = request->getParam("rolling_code", true);
    rollingCode = int(p->value().toInt());
  }

  WebServer* instance = WebServer::getInstance();
  Result<Remote> result = instance->m_controller->updateRemote(remoteId, name.c_str(), rollingCode);

  if (!result.isSuccess)
  {
    request->send(400, "application/json", "{\"message\":\"" + result.errorMsg + "\"}");
    return;
  }
  String serialized = instance->m_serializer->serializeRemote(result.data);
  request->send(200, "application/json", serialized);
}

void WebServer::handleDeleteRemote(AsyncWebServerRequest* request)
{
  LOG_INFO("Endpoint to delete a remote reached.");

  unsigned long remoteId = strtoul(request->pathArg(0).c_str(), nullptr, 10);

  WebServer* instance = WebServer::getInstance();
  Result<Remote> result = instance->m_controller->deleteRemote(remoteId);

  if (!result.isSuccess)
  {
    request->send(400, "application/json", "{\"message\":\"" + result.errorMsg + "\"}");
    return;
  }
  String serialized = instance->m_serializer->serializeRemote(result.data);
  request->send(200, "application/json", serialized);
}

void WebServer::handleActionRemote(AsyncWebServerRequest* request)
{
  LOG_INFO("Endpoint to operate an action on a remote reached.");

  unsigned long remoteId = strtoul(request->pathArg(0).c_str(), nullptr, 10);

  String action;
  if (request->hasParam("action", true))
  {
    AsyncWebParameter* p = request->getParam("action", true);
    action = p->value();
  }

  WebServer* instance = WebServer::getInstance();
  Result<String> result = instance->m_controller->operateRemote(remoteId, action.c_str());

  if (!result.isSuccess)
  {
    request->send(400, "application/json", "{\"message\":\"" + result.errorMsg + "\"}");
    return;
  }
  request->send(200, "application/json", "{\"message\":\"" + result.data + "\"}");
}

void WebServer::handleHTMLHomePage(AsyncWebServerRequest* request)
{
  LOG_INFO("HTML home page reached.");

  request->send(LittleFS, "/index.html", String());
}

void WebServer::handleHTMLNotFoundPage(AsyncWebServerRequest* request)
{
  LOG_INFO("HTML 404 Not Found reached.");

  request->send(LittleFS, "/404.html", String());
}