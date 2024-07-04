#include <Arduino.h>
#include <unity.h>

#include <remote.h>
#include <networks.h>
#include <mqttConfig.h>
#include <systemInfos.h>
#include <jsonSerializer.h>

#include "./test_jsonSerializer.h"

JSONSerializer serializerTest;

void RUN_JSONSERIALIZER_TESTS(void)
{
  RUN_TEST(test_MEHTOD_serializeMessage_WITH_message_SHOULD_return_string);
  RUN_TEST(test_METHOD_serializeRemote_WITH_remote_SHOULD_return_string);
  RUN_TEST(test_METHOD_serializeRemotes_WITH_two_remotes_SHOULD_return_string);
  RUN_TEST(test_METHOD_serializeRemotes_WITH_one_remote_SHOULD_return_string);
  RUN_TEST(test_METHOD_serializeNetworkConfig_WITH_config_SHOULD_return_string);
  RUN_TEST(test_METHOD_serializeSystemInfos_WITH_info_SHOULD_return_string);
  RUN_TEST(test_METHOD_serializeSystemInfos_WITH_info_extended_SHOULD_return_string);
  RUN_TEST(test_METHOD_serializeNetworks_WITH_one_network_SHOULD_return_string);
  RUN_TEST(test_METHOD_serializeNetworks_WITH_two_networks_SHOULD_return_string);
  RUN_TEST(test_METHOD_serializeMQTTConfig_WITH_config_SHOULD_return_string);
}

void test_MEHTOD_serializeMessage_WITH_message_SHOULD_return_string(void)
{
  char messageA[] = "foo";
  String serializedA = serializerTest.serializeMessage(messageA);
  String expectedA = "{\"message\":\"foo\"}";

  TEST_ASSERT_EQUAL_STRING(expectedA.c_str(), serializedA.c_str());

  char messageB[] = "Hello World";
  String serializedB = serializerTest.serializeMessage(messageB);
  String expectedB = "{\"message\":\"Hello World\"}";

  TEST_ASSERT_EQUAL_STRING(expectedB.c_str(), serializedB.c_str());
}

void test_METHOD_serializeRemote_WITH_remote_SHOULD_return_string(void)
{
  Remote remoteA = { 1, 0, "foo" };
  String serializedA = serializerTest.serializeRemote(remoteA);
  String expectedA = "{\"id\":1,\"rolling_code\":0,\"name\":\"foo\"}";

  TEST_ASSERT_EQUAL_STRING(expectedA.c_str(), serializedA.c_str());

  Remote remoteB = { 42, 42, "bar" };
  String serializedB = serializerTest.serializeRemote(remoteB);
  String expectedB = "{\"id\":42,\"rolling_code\":42,\"name\":\"bar\"}";

  TEST_ASSERT_EQUAL_STRING(expectedB.c_str(), serializedB.c_str());
}

void test_METHOD_serializeRemotes_WITH_two_remotes_SHOULD_return_string(void)
{
  Remote remoteA = { 1, 0, "foo" };
  Remote remoteB = { 42, 42, "bar" };

  Remote remotes[2] = { remoteA, remoteB };

  String serialized = serializerTest.serializeRemotes(remotes, 2);
  String expected = "[{\"id\":1,\"rolling_code\":0,\"name\":\"foo\"},{\"id\":42,\"rolling_code\":"
                    "42,\"name\":\"bar\"}]";

  TEST_ASSERT_EQUAL_STRING(expected.c_str(), serialized.c_str());
}

void test_METHOD_serializeRemotes_WITH_one_remote_SHOULD_return_string(void)
{
  Remote remoteA = { 1, 0, "foo" };

  Remote remotes[] = { remoteA };

  String serialized = serializerTest.serializeRemotes(remotes, 1);
  String expected = "[{\"id\":1,\"rolling_code\":0,\"name\":\"foo\"}]";

  TEST_ASSERT_EQUAL_STRING(expected.c_str(), serialized.c_str());
}

void test_METHOD_serializeNetworkConfig_WITH_config_SHOULD_return_string(void)
{
  NetworkConfiguration config = { "foo", "bar" };

  String serialized = serializerTest.serializeNetworkConfig(config);
  String expected = "{\"ssid\":\"foo\",\"password\":\"bar\"}";

  TEST_ASSERT_EQUAL_STRING(expected.c_str(), serialized.c_str());
}

void test_METHOD_serializeSystemInfos_WITH_info_SHOULD_return_string(void)
{
  SystemInfos infos = { "1.0.0" };

  String serialized = serializerTest.serializeSystemInfos(infos);
  String expected = "{\"version\":\"1.0.0\"}";

  TEST_ASSERT_EQUAL_STRING(expected.c_str(), serialized.c_str());
}

void test_METHOD_serializeSystemInfos_WITH_info_extended_SHOULD_return_string(void)
{
  SystemInfosExtended infos = { "1.0.0", "FF:FF:FF:FF:FF:FF", "255.255.255.255" };

  String serialized = serializerTest.serializeSystemInfos(infos);
  String expected = "{\"version\":\"1.0.0\",\"mac\":\"FF:FF:FF:FF:FF:FF\",\"ip\":\"255.255.255.255\"}";

  TEST_ASSERT_EQUAL_STRING(expected.c_str(), serialized.c_str());
}

void test_METHOD_serializeNetworks_WITH_one_network_SHOULD_return_string(void)
{
  Network network = { "foo", -85 };

  Network networks[] = { network };

  String serialized = serializerTest.serializeNetworks(networks, 1);
  String expected = "[{\"ssid\":\"foo\",\"rssi\":-85}]";

  TEST_ASSERT_EQUAL_STRING(expected.c_str(), serialized.c_str());
}

void test_METHOD_serializeNetworks_WITH_two_networks_SHOULD_return_string(void)
{
  Network networkA = { "foo", -85 };
  Network networkB = { "baz", -60 };

  Network networks[] = { networkA, networkB };

  String serialized = serializerTest.serializeNetworks(networks, 2);
  String expected = "[{\"ssid\":\"foo\",\"rssi\":-85},{\"ssid\":\"baz\",\"rssi\":-60}]";

  TEST_ASSERT_EQUAL_STRING(expected.c_str(), serialized.c_str());
}

void test_METHOD_serializeMQTTConfig_WITH_config_SHOULD_return_string(void)
{
  MQTTConfiguration config = { true, "foo.bar", 1883, "foo", "bar" };

  String serialized = serializerTest.serializeMQTTConfig(config);
  String expected = "{\"enabled\":true,\"broker\":\"foo.bar\",\"port\":1883,\"username\":\"foo\","
                    "\"password\":\"bar\"}";

  TEST_ASSERT_EQUAL_STRING(expected.c_str(), serialized.c_str());
}