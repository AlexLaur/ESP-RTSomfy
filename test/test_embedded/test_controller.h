#pragma once

#include <result.h>
#include <networks.h>
#include <mqttConfig.h>
#include <systemInfos.h>
#include <databaseAbs.h>
#include <transmitterAbs.h>
#include <networkClientAbs.h>
#include <systemManagerAbs.h>

class FakeSystemManager : public SystemManagerAbstract
{
  public:
  // For tests
  static bool requestRestartCalled;

  void handleActions();
  void requestRestart();
};

class FakeDatabase : public DatabaseAbstract
{
  public:
  // For tests
  static bool shouldFailDeleteRemote;
  static bool shouldFailUpdateRemote;
  static bool shouldReturnEmptyRemote;
  static bool shouldFailCreateRemote;
  static bool shouldFailUpdateNetworkConfiguration;
  static bool shouldFailUpdateMQTTConfiguration;

  void init();
  bool migrate();

  SystemInfos getSystemInfos();

  NetworkConfiguration getNetworkConfiguration();
  bool setNetworkConfiguration(const NetworkConfiguration& networkConfig);
  void resetNetworkConfiguration();

  Remote createRemote(const char* name);
  void getAllRemotes(Remote remotes[]);
  Remote getRemote(const unsigned long& id);
  bool updateRemote(const Remote& remote);
  bool deleteRemote(const unsigned long& id);

  MQTTConfiguration getMQTTConfiguration();
  bool setMQTTConfiguration(const MQTTConfiguration& mqttConfig);
};

class FakeTransmitter : public TransmitterAbstract
{
  public:
  static bool sendUPCommandCalled;
  static bool sendSTOPCommandCalled;
  static bool sendDOWNCommandCalled;
  static bool sendPROGCommandCalled;

  bool sendUpCmd(const unsigned long remoteId, const unsigned int rollingCode);
  bool sendStopCmd(const unsigned long remoteId, const unsigned int rollingCode);
  bool sendDownCmd(const unsigned long remoteId, const unsigned int rollingCode);
  bool sendProgCmd(const unsigned long remoteId, const unsigned int rollingCode);
};

class FakeNetworkClient : public NetworkClientAbstract
{
  public:
  bool connect(const NetworkConfiguration& conf);
  bool connect(const char* ssid, const char* password);
  String getIP();
  String getMacAddress();
  bool isConnected();
  void scanNetworks();
  void getNetworks(Network networks[]);
};

// TEST controller

void RUN_CONTROLLER_TESTS(void);

void test_METHOD_fetchSystemInfos_SHOULD_return_systeminfos(void);

void test_METHOD_askSystemRestart_SHOULD_return_request_a_restart_AND_return_result_WITH_success_to_true(void);

void test_METHOD_fetchRemote_WITH_unspecified_id_SHOULD_return_result_WITH_success_to_false(void);
void test_METHOD_fetchRemote_WITH_remote_not_found_SHOULD_return_result_WITH_success_to_false(void);
void test_METHOD_fetchRemote_SHOULD_return_result_WITH_success_to_true(void);

void test_METHOD_fetchAllRemotes_SHOULD_return_result_WITH_success_to_true(void);

void test_METHOD_createRemote_WITH_null_name_SHOULD_return_result_WITH_success_to_false(void);
void test_METHOD_createRemote_WITH_empty_name_SHOULD_return_result_WITH_success_to_false(void);
void test_METHOD_createRemote_WITH_name_too_long_SHOULD_return_result_WITH_success_to_false(void);
void test_METHOD_createRemote_WITH_database_fail_SHOULD_return_result_WITH_success_to_false(void);
void test_METHOD_createRemote_SHOULD_return_result_WITH_success_to_true(void);

void test_METHOD_deleteRemote_WITH_empty_remote_id_SHOULD_return_result_WITH_success_to_false(void);
void test_METHOD_deleteRemote_WITH_database_fail_SHOULD_return_result_WITH_success_to_false(void);
void test_METHOD_deleteRemote_SHOULD_return_result_WITH_success_to_true(void);

void test_METHOD_updateRemote_WITH_empty_remote_id_SHOULD_return_result_WITH_success_to_false(void);
void test_METHOD_updateRemote_WITH_remote_not_found_SHOULD_return_result_WITH_success_to_false(
    void);
void test_METHOD_updateRemote_WITH_valid_remote_AND_name_too_long_SHOULD_return_result_WITH_success_to_false(
    void);
void test_METHOD_updateRemote_WITH_valid_remote_AND_null_name_SHOULD_return_result_WITH_success_to_true(
    void);
void test_METHOD_updateRemote_WITH_valid_remote_AND_valid_name_SHOULD_return_result_WITH_success_to_true(
    void);
void test_METHOD_updateRemote_WITH_valid_remote_AND_rolling_code_provided_SHOULD_return_result_WITH_success_to_true(
    void);
void test_METHOD_updateRemote_WITH_valid_remote_AND_valid_data_AND_database_fail_SHOULD_return_result_WITH_success_to_false(
    void);

void test_METHOD_operateRemote_WITH_empty_remote_id_SHOULD_return_result_WITH_success_to_false(
    void);
void test_METHOD_operateRemote_WITH_null_action_SHOULD_return_result_WITH_success_to_false(void);
void test_METHOD_operateRemote_WITH_empty_action_SHOULD_return_result_WITH_success_to_false(void);
void test_METHOD_operateRemote_WITH_not_found_remote_SHOULD_return_result_WITH_success_to_false(
    void);
void test_METHOD_operateRemote_WITH_unknown_action_SHOULD_return_result_WITH_success_to_false(void);
void test_METHOD_operateRemote_WITH_valide_remote_AND_up_action_SHOULD_return_result_WITH_success_to_true(
    void);
void test_METHOD_operateRemote_WITH_valide_remote_AND_stop_action_SHOULD_return_result_WITH_success_to_true(
    void);
void test_METHOD_operateRemote_WITH_valide_remote_AND_down_action_SHOULD_return_result_WITH_success_to_true(
    void);
void test_METHOD_operateRemote_WITH_valide_remote_AND_pair_action_SHOULD_return_result_WITH_success_to_true(
    void);
void test_METHOD_operateRemote_WITH_valide_remote_AND_reset_action_SHOULD_return_result_WITH_success_to_true(
    void);

void test_METHOD_fetchNetworkConfiguration_SHOULD_return_result_WITH_success_to_true(void);

void test_METHOD_updateNetworkConfiguration_WITH_valid_data_SHOULD_return_result_WITH_success_to_true(
    void);
void test_METHOD_updateNetworkConfiguration_WITH_valid_data_AND_empty_password_SHOULD_return_result_WITH_success_to_true(
    void);
void test_METHOD_updateNetworkConfiguration_WITH_null_SSID_SHOULD_return_result_WITH_success_to_false(
    void);
void test_METHOD_updateNetworkConfiguration_WITH_empty_SSID_SHOULD_return_result_WITH_success_to_false(
    void);
void test_METHOD_updateNetworkConfiguration_WITH_update_fail_SHOULD_return_result_WITH_success_to_false(
    void);

void test_METHOD_fetchMQTTConfiguration_SHOULD_return_result_WITH_success_to_true(void);

void test_METHOD_updateMQTTConfiguration_WITH_valid_data_SHOULD_return_result_WITH_success_to_true(
    void);
void test_METHOD_updateMQTTConfiguration_WITH_empty_port_SHOULD_return_result_WITH_success_to_false(
    void);
void test_METHOD_updateMQTTConfiguration_WITH_null_broker_SHOULD_return_result_WITH_success_to_true_AND_enabled_to_false(
    void);
void test_METHOD_updateMQTTConfiguration_WITH_null_username_SHOULD_return_result_WITH_success_to_true(
    void);
void test_METHOD_updateMQTTConfiguration_WITH_null_password_SHOULD_return_result_WITH_success_to_true(
    void);
void test_METHOD_updateMQTTConfiguration_WITH_update_fail_SHOULD_return_result_WITH_success_to_false(
    void);
