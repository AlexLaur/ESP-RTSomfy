/**
 * @file webServer.h
 * @author Laurette Alexandre
 * @brief Header for WebServer.
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

#include <ESPAsyncWebServer.h>

#include <config.h>
#include <remote.h>
#include <observer.h>
#include <controller.h>
#include <serializerAbs.h>

class WebServer : public Observer
{
  public:
  WebServer(const unsigned short port, Controller* controller, SerializerAbstract* serializer);
  ~WebServer();
  static WebServer* getInstance();

  void setup();
  void begin();

  void notified(const char* action, const Remote& remote); // from observer

  private:
  static WebServer* m_instance;
  AsyncWebServer* m_server = nullptr;
  Controller* m_controller = nullptr;
  SerializerAbstract* m_serializer;

  // API REST
  static void handleSystemRestart(AsyncWebServerRequest* request);
  static void handleFetchSystemInfos(AsyncWebServerRequest* request);
  static void handleFetchWifiNetworks(AsyncWebServerRequest* request);
  static void handleFetchWifiConfiguration(AsyncWebServerRequest* request);
  static void handleUpdateWifiConfiguration(AsyncWebServerRequest* request);
  static void handleFetchMQTTConfiguration(AsyncWebServerRequest* request);
  static void handleUpdateMQTTConfiguration(AsyncWebServerRequest* request);
  static void handleFetchAllRemotes(AsyncWebServerRequest* request);
  static void handleFetchRemote(AsyncWebServerRequest* request);
  static void handleCreateRemote(AsyncWebServerRequest* request);
  static void handleUpdateRemote(AsyncWebServerRequest* request);
  static void handleDeleteRemote(AsyncWebServerRequest* request);
  static void handleActionRemote(AsyncWebServerRequest* request);
  // HTML
  static void handleHTMLHomePage(AsyncWebServerRequest* request);
  static void handleHTMLNotFoundPage(AsyncWebServerRequest* request);
};