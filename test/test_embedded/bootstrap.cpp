#include <Arduino.h>
#include <unity.h>

#include "./test_jsonSerializer.h"
#include "./test_eepromDatabase.h"
#include "./test_RTSTransmitter.h"
#include "./test_controller.h"

void setUp(void)
{
  // set stuff up here
}

void tearDown(void)
{
  // clean stuff up here
  FakeSystemManager::requestRestartCalled = false;

  FakeDatabase::shouldFailDeleteRemote = false;
  FakeDatabase::shouldFailUpdateRemote = false;
  FakeDatabase::shouldReturnEmptyRemote = false;
  FakeDatabase::shouldFailCreateRemote = false;
  FakeDatabase::shouldFailUpdateNetworkConfiguration = false;
  FakeDatabase::shouldFailUpdateMQTTConfiguration = false;

  FakeTransmitter::sendUPCommandCalled = false;
  FakeTransmitter::sendSTOPCommandCalled = false;
  FakeTransmitter::sendDOWNCommandCalled = false;
  FakeTransmitter::sendPROGCommandCalled = false;
}

void RUN_UNITY_TESTS()
{
  UNITY_BEGIN();
  // JSONSerializer tests
  RUN_JSONSERIALIZER_TESTS();
  // EEPROMDatabase tests
  RUN_EEPROMDATABASE_TESTS();
  // Controller tests
  RUN_CONTROLLER_TESTS();
  // RTS Transmitter tests
  RUN_RTSTRANSMITTER_TESTS();
  UNITY_END();
}

#ifdef ARDUINO

#include <Arduino.h>

void setup()
{
  Serial.begin(115200);
  delay(1000);

  RUN_UNITY_TESTS();
}

void loop() { }

#else

int main(int argc, char** argv)
{
  RUN_UNITY_TESTS();
  return 0;
}

#endif
