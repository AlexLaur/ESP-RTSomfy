#include <unity.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <remote.h>
#include <result.h>
#include <networks.h>
#include <systemInfos.h>
#include <controller.h>
#include "./test_controller.h"

// Fake SystemManager
bool FakeSystemManager::requestRestartCalled = false;
void FakeSystemManager::handleActions() { }

void FakeSystemManager::requestRestart() { this->requestRestartCalled = true; }

// Fake Database
bool FakeDatabase::shouldFailDeleteRemote = false;
bool FakeDatabase::shouldFailUpdateRemote = false;
bool FakeDatabase::shouldReturnEmptyRemote = false;
bool FakeDatabase::shouldFailCreateRemote = false;
bool FakeDatabase::shouldFailUpdateNetworkConfiguration = false;
bool FakeDatabase::shouldFailUpdateMQTTConfiguration = false;

void FakeDatabase::init() { }

bool FakeDatabase::migrate() { return true; }

SystemInfos FakeDatabase::getSystemInfos()
{
  SystemInfos infos = { "1.0.0" };
  return infos;
}

NetworkConfiguration FakeDatabase::getNetworkConfiguration()
{
  NetworkConfiguration conf = { "foo", "bar" };
  return conf;
}

bool FakeDatabase::setNetworkConfiguration(const NetworkConfiguration& networkConfig)
{
  if (this->shouldFailUpdateNetworkConfiguration)
  {
    return false;
  }
  return true;
}

void FakeDatabase::resetNetworkConfiguration() { }

Remote FakeDatabase::createRemote(const char* name)
{
  Remote remote = { 0, 0, "" };
  if (this->shouldFailCreateRemote)
  {
    return remote;
  }
  remote.id = 1;
  strcpy(remote.name, name);
  return remote;
}

void FakeDatabase::getAllRemotes(Remote remotes[]) { }

Remote FakeDatabase::getRemote(const unsigned long& id)
{
  Remote remote = { 0, 0, "" };
  if (this->shouldReturnEmptyRemote)
  {
    return remote;
  }
  remote.id = 1;
  remote.rollingCode = 42;
  strcpy(remote.name, "foo");
  return remote;
}

bool FakeDatabase::updateRemote(const Remote& remote)
{
  if (this->shouldFailUpdateRemote)
  {
    return false;
  }
  return true;
}

bool FakeDatabase::deleteRemote(const unsigned long& id)
{
  if (this->shouldFailDeleteRemote)
  {
    return false;
  }
  return true;
}

MQTTConfiguration FakeDatabase::getMQTTConfiguration()
{
  MQTTConfiguration conf = { true, "foo.foo", 1234, "foo", "bar" };
  return conf;
}

bool FakeDatabase::setMQTTConfiguration(const MQTTConfiguration& mqttConfig)
{
  if (this->shouldFailUpdateMQTTConfiguration)
  {
    return false;
  }
  return true;
}

// Fake Transmitter
bool FakeTransmitter::sendUPCommandCalled = false;
bool FakeTransmitter::sendSTOPCommandCalled = false;
bool FakeTransmitter::sendDOWNCommandCalled = false;
bool FakeTransmitter::sendPROGCommandCalled = false;

bool FakeTransmitter::sendUpCmd(const unsigned long remoteId, const unsigned int rollingCode)
{
  this->sendUPCommandCalled = true;
  return true;
};
bool FakeTransmitter::sendStopCmd(const unsigned long remoteId, const unsigned int rollingCode)
{
  this->sendSTOPCommandCalled = true;
  return true;
}
bool FakeTransmitter::sendDownCmd(const unsigned long remoteId, const unsigned int rollingCode)
{
  this->sendDOWNCommandCalled = true;
  return true;
}
bool FakeTransmitter::sendProgCmd(const unsigned long remoteId, const unsigned int rollingCode)
{
  this->sendPROGCommandCalled = true;
  return true;
}

// Fake NetworkClient
bool FakeNetworkClient::connect(const NetworkConfiguration& conf) { return true; };
bool FakeNetworkClient::connect(const char* ssid, const char* password) { return true; };
String FakeNetworkClient::getIP() { return String("255.255.255.255"); };
String FakeNetworkClient::getMacAddress() { return String("FF:FF:FF:FF:FF:FF"); }
bool FakeNetworkClient::isConnected() { return true; };
void FakeNetworkClient::getNetworks(Network networks[]) {};
void FakeNetworkClient::scanNetworks() {};

// TEST CONTROLLER
// ############################################################################

FakeDatabase databaseFake;
FakeNetworkClient networkClientFake;
FakeTransmitter transmitterFake;
FakeSystemManager fakeSystemManager;

Controller controllerTest(&databaseFake, &networkClientFake, &transmitterFake, &fakeSystemManager);

void RUN_CONTROLLER_TESTS(void)
{
  RUN_TEST(test_METHOD_fetchSystemInfos_SHOULD_return_systeminfos);
  RUN_TEST(
      test_METHOD_askSystemRestart_SHOULD_return_request_a_restart_AND_return_result_WITH_success_to_true);
  RUN_TEST(test_METHOD_fetchRemote_WITH_unspecified_id_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(
      test_METHOD_fetchRemote_WITH_remote_not_found_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(test_METHOD_fetchRemote_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(test_METHOD_createRemote_WITH_null_name_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(test_METHOD_createRemote_WITH_empty_name_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(test_METHOD_createRemote_WITH_name_too_long_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(test_METHOD_createRemote_WITH_database_fail_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(test_METHOD_createRemote_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(
      test_METHOD_deleteRemote_WITH_empty_remote_id_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(test_METHOD_deleteRemote_WITH_database_fail_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(test_METHOD_deleteRemote_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(
      test_METHOD_updateRemote_WITH_empty_remote_id_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(
      test_METHOD_updateRemote_WITH_remote_not_found_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(
      test_METHOD_updateRemote_WITH_valid_remote_AND_name_too_long_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(
      test_METHOD_updateRemote_WITH_valid_remote_AND_null_name_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(
      test_METHOD_updateRemote_WITH_valid_remote_AND_valid_name_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(
      test_METHOD_updateRemote_WITH_valid_remote_AND_rolling_code_provided_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(
      test_METHOD_updateRemote_WITH_valid_remote_AND_valid_data_AND_database_fail_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(
      test_METHOD_operateRemote_WITH_empty_remote_id_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(test_METHOD_operateRemote_WITH_null_action_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(test_METHOD_operateRemote_WITH_empty_action_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(
      test_METHOD_operateRemote_WITH_not_found_remote_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(
      test_METHOD_operateRemote_WITH_unknown_action_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(
      test_METHOD_operateRemote_WITH_valide_remote_AND_up_action_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(
      test_METHOD_operateRemote_WITH_valide_remote_AND_stop_action_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(
      test_METHOD_operateRemote_WITH_valide_remote_AND_down_action_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(
      test_METHOD_operateRemote_WITH_valide_remote_AND_pair_action_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(
      test_METHOD_operateRemote_WITH_valide_remote_AND_reset_action_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(test_METHOD_fetchNetworkConfiguration_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(
      test_METHOD_updateNetworkConfiguration_WITH_valid_data_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(
      test_METHOD_updateNetworkConfiguration_WITH_valid_data_AND_empty_password_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(
      test_METHOD_updateNetworkConfiguration_WITH_null_SSID_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(
      test_METHOD_updateNetworkConfiguration_WITH_empty_SSID_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(
      test_METHOD_updateNetworkConfiguration_WITH_update_fail_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(
      test_METHOD_updateMQTTConfiguration_WITH_valid_data_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(
      test_METHOD_updateMQTTConfiguration_WITH_empty_port_SHOULD_return_result_WITH_success_to_false);
  RUN_TEST(
      test_METHOD_updateMQTTConfiguration_WITH_null_broker_SHOULD_return_result_WITH_success_to_true_AND_enabled_to_false);
  RUN_TEST(
      test_METHOD_updateMQTTConfiguration_WITH_null_username_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(
      test_METHOD_updateMQTTConfiguration_WITH_null_password_SHOULD_return_result_WITH_success_to_true);
  RUN_TEST(
      test_METHOD_updateMQTTConfiguration_WITH_update_fail_SHOULD_return_result_WITH_success_to_false);
}

void test_METHOD_fetchSystemInfos_SHOULD_return_systeminfos(void)
{
  Result<SystemInfosExtended> result = controllerTest.fetchSystemInfos();

  TEST_ASSERT_TRUE(result.isSuccess);
  TEST_ASSERT_EQUAL_STRING("1.0.0", result.data.version);
  TEST_ASSERT_EQUAL_STRING("FF:FF:FF:FF:FF:FF", result.data.macAddress.c_str());
  TEST_ASSERT_EQUAL_STRING("255.255.255.255", result.data.ipAddress.c_str());
  TEST_ASSERT_EQUAL_STRING_LEN("", result.errorMsg.c_str(), 0);
}

void test_METHOD_askSystemRestart_SHOULD_return_request_a_restart_AND_return_result_WITH_success_to_true(
    void)
{
  Result<String> result = controllerTest.askSystemRestart();

  TEST_ASSERT_TRUE(result.isSuccess);
  TEST_ASSERT_TRUE(FakeSystemManager::requestRestartCalled);
  TEST_ASSERT_EQUAL_STRING_LEN("", result.errorMsg.c_str(), 0);
}

void test_METHOD_fetchRemote_WITH_unspecified_id_SHOULD_return_result_WITH_success_to_false(void)
{
  Result<Remote> result = controllerTest.fetchRemote(0);

  TEST_ASSERT_EQUAL(0, result.data.id); // an empty remote
  TEST_ASSERT_FALSE(result.isSuccess);
  TEST_ASSERT_GREATER_OR_EQUAL(1, result.errorMsg.length());
}

void test_METHOD_fetchRemote_WITH_remote_not_found_SHOULD_return_result_WITH_success_to_false(void)
{
  FakeDatabase::shouldReturnEmptyRemote = true;

  Result<Remote> result = controllerTest.fetchRemote(1);

  TEST_ASSERT_EQUAL(0, result.data.id); // an empty remote
  TEST_ASSERT_FALSE(result.isSuccess);
  TEST_ASSERT_GREATER_OR_EQUAL(1, result.errorMsg.length());
}

void test_METHOD_fetchRemote_SHOULD_return_result_WITH_success_to_true(void)
{
  Result<Remote> result = controllerTest.fetchRemote(1);

  TEST_ASSERT_EQUAL(1, result.data.id);
  TEST_ASSERT_EQUAL(42, result.data.rollingCode);
  TEST_ASSERT_EQUAL_STRING("foo", result.data.name);
  TEST_ASSERT_TRUE(result.isSuccess);
  TEST_ASSERT_EQUAL_STRING_LEN("", result.errorMsg.c_str(), 0);
}

void test_METHOD_fetchAllRemotes_SHOULD_return_result_WITH_success_to_true(void)
{
  Result<Remote[MAX_REMOTES]> result = controllerTest.fetchAllRemotes();

  // TODO assert result.data == empty array (as in fakeDatabase) ?
  TEST_ASSERT_TRUE(result.isSuccess);
  TEST_ASSERT_EQUAL_STRING_LEN("", result.errorMsg.c_str(), 0);
}

void test_METHOD_createRemote_WITH_null_name_SHOULD_return_result_WITH_success_to_false(void)
{
  Result<Remote> result = controllerTest.createRemote(nullptr);

  TEST_ASSERT_EQUAL(0, result.data.id); // an empty remote
  TEST_ASSERT_FALSE(result.isSuccess);
  TEST_ASSERT_GREATER_OR_EQUAL(1, result.errorMsg.length());
}

void test_METHOD_createRemote_WITH_empty_name_SHOULD_return_result_WITH_success_to_false(void)
{
  Result<Remote> result = controllerTest.createRemote("");

  TEST_ASSERT_EQUAL(0, result.data.id); // an empty remote
  TEST_ASSERT_FALSE(result.isSuccess);
  TEST_ASSERT_GREATER_OR_EQUAL(1, result.errorMsg.length());
}

void test_METHOD_createRemote_WITH_name_too_long_SHOULD_return_result_WITH_success_to_false(void)
{
  Result<Remote> result = controllerTest.createRemote("foo bar baz foo bar baz foo");

  TEST_ASSERT_EQUAL(0, result.data.id); // an empty remote
  TEST_ASSERT_FALSE(result.isSuccess);
  TEST_ASSERT_GREATER_OR_EQUAL(1, result.errorMsg.length());
}

void test_METHOD_createRemote_WITH_database_fail_SHOULD_return_result_WITH_success_to_false(void)
{
  FakeDatabase::shouldFailCreateRemote = true;
  Result<Remote> result = controllerTest.createRemote("foo");

  TEST_ASSERT_EQUAL(0, result.data.id); // an empty remote
  TEST_ASSERT_FALSE(result.isSuccess);
  TEST_ASSERT_GREATER_OR_EQUAL(1, result.errorMsg.length());
}

void test_METHOD_createRemote_SHOULD_return_result_WITH_success_to_true(void)
{
  Result<Remote> result = controllerTest.createRemote("foo");

  TEST_ASSERT_EQUAL(1, result.data.id);
  TEST_ASSERT_EQUAL(0, result.data.rollingCode);
  TEST_ASSERT_EQUAL_STRING("foo", result.data.name);
  TEST_ASSERT_TRUE(result.isSuccess);
  TEST_ASSERT_EQUAL_STRING_LEN("", result.errorMsg.c_str(), 0);
}

void test_METHOD_deleteRemote_WITH_empty_remote_id_SHOULD_return_result_WITH_success_to_false(void)
{
  Result<Remote> result = controllerTest.deleteRemote(0);

  TEST_ASSERT_EQUAL(0, result.data.id); // an empty remote
  TEST_ASSERT_FALSE(result.isSuccess);
  TEST_ASSERT_GREATER_OR_EQUAL(1, result.errorMsg.length());
}

void test_METHOD_deleteRemote_WITH_database_fail_SHOULD_return_result_WITH_success_to_false(void)
{
  FakeDatabase::shouldFailDeleteRemote = true;

  Result<Remote> result = controllerTest.deleteRemote(1);

  TEST_ASSERT_EQUAL(0, result.data.id); // an empty remote
  TEST_ASSERT_FALSE(result.isSuccess);
  TEST_ASSERT_GREATER_OR_EQUAL(1, result.errorMsg.length());
}

void test_METHOD_deleteRemote_SHOULD_return_result_WITH_success_to_true(void)
{
  Result<Remote> result = controllerTest.deleteRemote(1);

  TEST_ASSERT_TRUE(result.isSuccess);
  TEST_ASSERT_EQUAL_STRING_LEN("", result.errorMsg.c_str(), 0);
}

void test_METHOD_updateRemote_WITH_empty_remote_id_SHOULD_return_result_WITH_success_to_false(void)
{
  Result<Remote> result = controllerTest.updateRemote(0, nullptr, 0);

  TEST_ASSERT_EQUAL(0, result.data.id); // an empty remote
  TEST_ASSERT_FALSE(result.isSuccess);
  TEST_ASSERT_GREATER_OR_EQUAL(1, result.errorMsg.length());
}

void test_METHOD_updateRemote_WITH_remote_not_found_SHOULD_return_result_WITH_success_to_false(void)
{
  FakeDatabase::shouldReturnEmptyRemote = true;

  Result<Remote> result = controllerTest.updateRemote(1, nullptr, 0);

  TEST_ASSERT_EQUAL(0, result.data.id); // an empty remote
  TEST_ASSERT_FALSE(result.isSuccess);
  TEST_ASSERT_GREATER_OR_EQUAL(1, result.errorMsg.length());
}

void test_METHOD_updateRemote_WITH_valid_remote_AND_name_too_long_SHOULD_return_result_WITH_success_to_false(
    void)
{
  Result<Remote> result = controllerTest.updateRemote(1, "foo bar baz foo bar baz", 0);

  TEST_ASSERT_EQUAL(0, result.data.id); // an empty remote
  TEST_ASSERT_FALSE(result.isSuccess);
  TEST_ASSERT_GREATER_OR_EQUAL(1, result.errorMsg.length());
}

void test_METHOD_updateRemote_WITH_valid_remote_AND_null_name_SHOULD_return_result_WITH_success_to_true(
    void)
{
  Result<Remote> result = controllerTest.updateRemote(1, nullptr, 0);

  TEST_ASSERT_EQUAL(1, result.data.id);
  TEST_ASSERT_EQUAL(42, result.data.rollingCode); // Update rolling code is not authorized yet.
  TEST_ASSERT_EQUAL_STRING("foo", result.data.name);
  TEST_ASSERT_TRUE(result.isSuccess);
  TEST_ASSERT_EQUAL_STRING_LEN("", result.errorMsg.c_str(), 0);
}

void test_METHOD_updateRemote_WITH_valid_remote_AND_valid_name_SHOULD_return_result_WITH_success_to_true(
    void)
{
  Result<Remote> result = controllerTest.updateRemote(1, "bar", 0);

  TEST_ASSERT_EQUAL(1, result.data.id);
  TEST_ASSERT_EQUAL(42, result.data.rollingCode); // Update rolling code is not authorized yet.
  TEST_ASSERT_EQUAL_STRING("bar", result.data.name);
  TEST_ASSERT_TRUE(result.isSuccess);
  TEST_ASSERT_EQUAL_STRING_LEN("", result.errorMsg.c_str(), 0);
}

void test_METHOD_updateRemote_WITH_valid_remote_AND_rolling_code_provided_SHOULD_return_result_WITH_success_to_true(
    void)
{
  Result<Remote> result = controllerTest.updateRemote(1, nullptr, 42);

  TEST_ASSERT_EQUAL(1, result.data.id);
  TEST_ASSERT_EQUAL(42, result.data.rollingCode);
  TEST_ASSERT_EQUAL_STRING("foo", result.data.name);
  TEST_ASSERT_TRUE(result.isSuccess);
  TEST_ASSERT_EQUAL_STRING_LEN("", result.errorMsg.c_str(), 0);
}

void test_METHOD_updateRemote_WITH_valid_remote_AND_valid_data_AND_database_fail_SHOULD_return_result_WITH_success_to_false(
    void)
{
  FakeDatabase::shouldFailUpdateRemote = true;

  Result<Remote> result = controllerTest.updateRemote(1, nullptr, 0);

  TEST_ASSERT_EQUAL(0, result.data.id); // an empty remote
  TEST_ASSERT_FALSE(result.isSuccess);
  TEST_ASSERT_GREATER_OR_EQUAL(1, result.errorMsg.length());
}

void test_METHOD_operateRemote_WITH_empty_remote_id_SHOULD_return_result_WITH_success_to_false(void)
{
  Result<String> result = controllerTest.operateRemote(0, "up");

  TEST_ASSERT_EQUAL_STRING_LEN("", result.data.c_str(), 0);
  TEST_ASSERT_FALSE(result.isSuccess);
  TEST_ASSERT_GREATER_OR_EQUAL(1, result.errorMsg.length());
}

void test_METHOD_operateRemote_WITH_null_action_SHOULD_return_result_WITH_success_to_false(void)
{
  Result<String> result = controllerTest.operateRemote(1, nullptr);

  TEST_ASSERT_EQUAL_STRING_LEN("", result.data.c_str(), 0);
  TEST_ASSERT_FALSE(result.isSuccess);
  TEST_ASSERT_GREATER_OR_EQUAL(1, result.errorMsg.length());
}

void test_METHOD_operateRemote_WITH_empty_action_SHOULD_return_result_WITH_success_to_false(void)
{
  Result<String> result = controllerTest.operateRemote(1, "");

  TEST_ASSERT_EQUAL_STRING_LEN("", result.data.c_str(), 0);
  TEST_ASSERT_FALSE(result.isSuccess);
  TEST_ASSERT_GREATER_OR_EQUAL(1, result.errorMsg.length());
}

void test_METHOD_operateRemote_WITH_not_found_remote_SHOULD_return_result_WITH_success_to_false(
    void)
{
  FakeDatabase::shouldReturnEmptyRemote = true;

  Result<String> result = controllerTest.operateRemote(1, "up");

  TEST_ASSERT_EQUAL_STRING_LEN("", result.data.c_str(), 0);
  TEST_ASSERT_FALSE(result.isSuccess);
  TEST_ASSERT_GREATER_OR_EQUAL(1, result.errorMsg.length());
}

void test_METHOD_operateRemote_WITH_unknown_action_SHOULD_return_result_WITH_success_to_false(void)
{
  Result<String> result = controllerTest.operateRemote(1, "foo");

  TEST_ASSERT_EQUAL_STRING_LEN("", result.data.c_str(), 0);
  TEST_ASSERT_FALSE(result.isSuccess);
  TEST_ASSERT_GREATER_OR_EQUAL(1, result.errorMsg.length());
}

void test_METHOD_operateRemote_WITH_valide_remote_AND_up_action_SHOULD_return_result_WITH_success_to_true(
    void)
{
  Result<String> result = controllerTest.operateRemote(1, "up");

  TEST_ASSERT_EQUAL_STRING("Command UP sent.", result.data.c_str());
  TEST_ASSERT_TRUE(result.isSuccess);
  TEST_ASSERT_EQUAL_STRING_LEN("", result.errorMsg.c_str(), 0);
  TEST_ASSERT_TRUE(FakeTransmitter::sendUPCommandCalled);
}

void test_METHOD_operateRemote_WITH_valide_remote_AND_stop_action_SHOULD_return_result_WITH_success_to_true(
    void)
{
  Result<String> result = controllerTest.operateRemote(1, "stop");

  TEST_ASSERT_EQUAL_STRING("Command STOP sent.", result.data.c_str());
  TEST_ASSERT_TRUE(result.isSuccess);
  TEST_ASSERT_EQUAL_STRING_LEN("", result.errorMsg.c_str(), 0);
  TEST_ASSERT_TRUE(FakeTransmitter::sendSTOPCommandCalled);
}

void test_METHOD_operateRemote_WITH_valide_remote_AND_down_action_SHOULD_return_result_WITH_success_to_true(
    void)
{
  Result<String> result = controllerTest.operateRemote(1, "down");

  TEST_ASSERT_EQUAL_STRING("Command DOWN sent.", result.data.c_str());
  TEST_ASSERT_TRUE(result.isSuccess);
  TEST_ASSERT_EQUAL_STRING_LEN("", result.errorMsg.c_str(), 0);
  TEST_ASSERT_TRUE(FakeTransmitter::sendDOWNCommandCalled);
}

void test_METHOD_operateRemote_WITH_valide_remote_AND_pair_action_SHOULD_return_result_WITH_success_to_true(
    void)
{
  Result<String> result = controllerTest.operateRemote(1, "pair");

  TEST_ASSERT_EQUAL_STRING("Command PAIR sent.", result.data.c_str());
  TEST_ASSERT_TRUE(result.isSuccess);
  TEST_ASSERT_EQUAL_STRING_LEN("", result.errorMsg.c_str(), 0);
  TEST_ASSERT_TRUE(FakeTransmitter::sendPROGCommandCalled);
}

void test_METHOD_operateRemote_WITH_valide_remote_AND_reset_action_SHOULD_return_result_WITH_success_to_true(
    void)
{
  Result<String> result = controllerTest.operateRemote(1, "reset");

  TEST_ASSERT_EQUAL_STRING("Rolling code reseted.", result.data.c_str());
  TEST_ASSERT_TRUE(result.isSuccess);
  TEST_ASSERT_EQUAL_STRING_LEN("", result.errorMsg.c_str(), 0);
}

void test_METHOD_fetchNetworkConfiguration_SHOULD_return_result_WITH_success_to_true(void)
{
  Result<NetworkConfiguration> result = controllerTest.fetchNetworkConfiguration();

  TEST_ASSERT_EQUAL_STRING("foo", result.data.ssid);
  TEST_ASSERT_EQUAL_STRING("bar", result.data.password);
  TEST_ASSERT_TRUE(result.isSuccess);
  TEST_ASSERT_EQUAL_STRING_LEN("", result.errorMsg.c_str(), 0);
}

void test_METHOD_updateNetworkConfiguration_WITH_valid_data_SHOULD_return_result_WITH_success_to_true(
    void)
{
  Result<NetworkConfiguration> result = controllerTest.updateNetworkConfiguration("foo", "bar");

  TEST_ASSERT_EQUAL_STRING("foo", result.data.ssid);
  TEST_ASSERT_EQUAL_STRING("bar", result.data.password);
  TEST_ASSERT_TRUE(result.isSuccess);
  TEST_ASSERT_EQUAL_STRING_LEN("", result.errorMsg.c_str(), 0);
}

void test_METHOD_updateNetworkConfiguration_WITH_valid_data_AND_empty_password_SHOULD_return_result_WITH_success_to_true(
    void)
{
  Result<NetworkConfiguration> result = controllerTest.updateNetworkConfiguration("foo", nullptr);

  TEST_ASSERT_TRUE(result.isSuccess);
  TEST_ASSERT_EQUAL_STRING_LEN("", result.errorMsg.c_str(), 0);
}

void test_METHOD_updateNetworkConfiguration_WITH_null_SSID_SHOULD_return_result_WITH_success_to_false(
    void)
{
  Result<NetworkConfiguration> result = controllerTest.updateNetworkConfiguration(nullptr, nullptr);

  TEST_ASSERT_EQUAL_STRING("", result.data.ssid);
  TEST_ASSERT_EQUAL_STRING("", result.data.password);
  TEST_ASSERT_FALSE(result.isSuccess);
  TEST_ASSERT_GREATER_OR_EQUAL(1, result.errorMsg.length());
}

void test_METHOD_updateNetworkConfiguration_WITH_empty_SSID_SHOULD_return_result_WITH_success_to_false(
    void)
{
  Result<NetworkConfiguration> result = controllerTest.updateNetworkConfiguration("", nullptr);

  TEST_ASSERT_EQUAL_STRING("", result.data.ssid);
  TEST_ASSERT_EQUAL_STRING("", result.data.password);
  TEST_ASSERT_FALSE(result.isSuccess);
  TEST_ASSERT_GREATER_OR_EQUAL(1, result.errorMsg.length());
}

void test_METHOD_updateNetworkConfiguration_WITH_update_fail_SHOULD_return_result_WITH_success_to_false(
    void)
{
  FakeDatabase::shouldFailUpdateNetworkConfiguration = true;

  Result<NetworkConfiguration> result = controllerTest.updateNetworkConfiguration("foo", nullptr);

  TEST_ASSERT_EQUAL_STRING("foo", result.data.ssid);
  TEST_ASSERT_EQUAL_STRING("", result.data.password);
  TEST_ASSERT_FALSE(result.isSuccess);
  TEST_ASSERT_GREATER_OR_EQUAL(1, result.errorMsg.length());
}

void test_METHOD_fetchMQTTConfiguration_SHOULD_return_result_WITH_success_to_true(void)
{
  Result<MQTTConfiguration> result = controllerTest.fetchMQTTConfiguration();

  TEST_ASSERT_TRUE(result.data.enabled);
  TEST_ASSERT_EQUAL_STRING("foo.foo", result.data.broker);
  TEST_ASSERT_EQUAL_STRING("foo", result.data.username);
  TEST_ASSERT_EQUAL_STRING("bar", result.data.password);
  TEST_ASSERT_EQUAL(1234, result.data.port);
  TEST_ASSERT_TRUE(result.isSuccess);
  TEST_ASSERT_EQUAL_STRING_LEN("", result.errorMsg.c_str(), 0);
}

void test_METHOD_updateMQTTConfiguration_WITH_valid_data_SHOULD_return_result_WITH_success_to_true(
    void)
{
  Result<MQTTConfiguration> result
      = controllerTest.updateMQTTConfiguration(true, "foo", 42, "bar", "baz");

  TEST_ASSERT_TRUE(result.data.enabled);
  TEST_ASSERT_EQUAL_STRING("foo", result.data.broker);
  TEST_ASSERT_EQUAL_STRING("bar", result.data.username);
  TEST_ASSERT_EQUAL_STRING("baz", result.data.password);
  TEST_ASSERT_EQUAL(42, result.data.port);
  TEST_ASSERT_TRUE(result.isSuccess);
  TEST_ASSERT_EQUAL_STRING_LEN("", result.errorMsg.c_str(), 0);
}

void test_METHOD_updateMQTTConfiguration_WITH_empty_port_SHOULD_return_result_WITH_success_to_false(
    void)
{
  Result<MQTTConfiguration> result
      = controllerTest.updateMQTTConfiguration(true, "foo", 0, "bar", "baz");

  TEST_ASSERT_FALSE(result.isSuccess);
  TEST_ASSERT_GREATER_OR_EQUAL(1, result.errorMsg.length());
}

void test_METHOD_updateMQTTConfiguration_WITH_null_broker_SHOULD_return_result_WITH_success_to_true_AND_enabled_to_false(
    void)
{
  Result<MQTTConfiguration> result
      = controllerTest.updateMQTTConfiguration(true, nullptr, 42, "bar", "baz");

  TEST_ASSERT_FALSE(result.isSuccess);
  TEST_ASSERT_GREATER_OR_EQUAL(1, result.errorMsg.length());
}

void test_METHOD_updateMQTTConfiguration_WITH_null_username_SHOULD_return_result_WITH_success_to_true(
    void)
{
  Result<MQTTConfiguration> result
      = controllerTest.updateMQTTConfiguration(true, "foo", 42, nullptr, "bar");

  TEST_ASSERT_TRUE(result.data.enabled);
  TEST_ASSERT_EQUAL_STRING("foo", result.data.broker);
  TEST_ASSERT_EQUAL_STRING("", result.data.username);
  TEST_ASSERT_EQUAL_STRING("bar", result.data.password);
  TEST_ASSERT_EQUAL(42, result.data.port);
  TEST_ASSERT_TRUE(result.isSuccess);
  TEST_ASSERT_EQUAL_STRING_LEN("", result.errorMsg.c_str(), 0);
}

void test_METHOD_updateMQTTConfiguration_WITH_null_password_SHOULD_return_result_WITH_success_to_true(
    void)
{
  Result<MQTTConfiguration> result
      = controllerTest.updateMQTTConfiguration(true, "foo", 42, "baz", nullptr);

  TEST_ASSERT_TRUE(result.data.enabled);
  TEST_ASSERT_EQUAL_STRING("foo", result.data.broker);
  TEST_ASSERT_EQUAL_STRING("baz", result.data.username);
  TEST_ASSERT_EQUAL_STRING("", result.data.password);
  TEST_ASSERT_EQUAL(42, result.data.port);
  TEST_ASSERT_TRUE(result.isSuccess);
  TEST_ASSERT_EQUAL_STRING_LEN("", result.errorMsg.c_str(), 0);
}

void test_METHOD_updateMQTTConfiguration_WITH_update_fail_SHOULD_return_result_WITH_success_to_false(
    void)
{
  FakeDatabase::shouldFailUpdateMQTTConfiguration = true;

  Result<MQTTConfiguration> result
      = controllerTest.updateMQTTConfiguration(true, "foo", 42, "bar", "baz");

  TEST_ASSERT_FALSE(result.isSuccess);
  TEST_ASSERT_GREATER_OR_EQUAL(1, result.errorMsg.length());
}