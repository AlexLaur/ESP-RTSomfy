/**
 * @file RTSTransmitter.cpp
 * @author Laurette Alexandre
 * @brief Implementation for RTS Protocol.
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

#include <RTSTransmitter.h>

#define PORT_TX D1

#define SYMBOL 640
#define BYTE_ACTION_UP 0x2
#define BYTE_ACTION_STOP 0x1
#define BYTE_ACTION_DOWN 0x4
#define BYTE_ACTION_PROG 0x8

#define SIG_HIGH GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, 1 << PORT_TX)
#define SIG_LOW GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, 1 << PORT_TX)

void RTSTransmitter::init()
{
  pinMode(PORT_TX, OUTPUT);
  SIG_LOW;
  digitalWrite(PORT_TX, LOW);
}

bool RTSTransmitter::sendUpCmd(const unsigned long remoteId, const unsigned int rollingCode)
{
  this->buildFrame(remoteId, rollingCode, BYTE_ACTION_UP);
  this->sendCommand();
  return true;
}

bool RTSTransmitter::sendStopCmd(const unsigned long remoteId, const unsigned int rollingCode)
{
  this->buildFrame(remoteId, rollingCode, BYTE_ACTION_STOP);
  this->sendCommand();
  return true;
}

bool RTSTransmitter::sendDownCmd(const unsigned long remoteId, const unsigned int rollingCode)
{
  this->buildFrame(remoteId, rollingCode, BYTE_ACTION_DOWN);
  this->sendCommand();
  return true;
}

bool RTSTransmitter::sendProgCmd(const unsigned long remoteId, const unsigned int rollingCode)
{
  this->buildFrame(remoteId, rollingCode, BYTE_ACTION_PROG);
  this->sendCommand();
  return true;
}

byte* RTSTransmitter::getBytesFrame(){
  return this->m_frame;
}

size_t RTSTransmitter::getBytesFrameSize(){
  return sizeof(this->m_frame) / sizeof(byte);
}

// PRIVATE
void RTSTransmitter::buildFrame(const unsigned long remoteId, const unsigned int rollingCode, const byte action)
{
  this->m_frame[0] = 0xA7;
  this->m_frame[1] = action << 4;
  this->m_frame[2] = rollingCode >> 8;
  this->m_frame[3] = rollingCode;
  this->m_frame[4] = remoteId >> 16;
  this->m_frame[5] = remoteId >> 8;
  this->m_frame[6] = remoteId;

  byte checksum = 0;
  for (byte i = 0; i < 7; i++)
  {
    checksum = checksum ^ this->m_frame[i] ^ (this->m_frame[i] >> 4);
  }
  checksum &= 0b1111;

  this->m_frame[1] |= checksum;

  for (byte i = 1; i < 7; i++)
  {
    this->m_frame[i] ^= this->m_frame[i - 1];
  }

  // For debug
  this->debugBuildedFrame(HEX);
  this->debugBuildedFrame(BIN);
  LOG_DEBUG("Frame builded.");
};

void RTSTransmitter::sendCommand()
{
  this->sendCommand(2);
  for (int i = 0; i < 2; i++)
  {
    this->sendCommand(7);
  }
}

void RTSTransmitter::sendCommand(const byte sync)
{
  if (sync == 2)
  {
    // Only with the first frame.
    // Wake-up pulse & Silence
    SIG_HIGH;
    delayMicroseconds(9415);
    SIG_LOW;
    delayMicroseconds(89565);
  }

  // Hardware sync: two sync for the first frame, seven for the following
  // ones.
  for (int i = 0; i < sync; i++)
  {
    SIG_HIGH;
    delayMicroseconds(4 * SYMBOL);
    SIG_LOW;
    delayMicroseconds(4 * SYMBOL);
  }

  // Software sync
  SIG_HIGH;
  delayMicroseconds(4550);
  SIG_LOW;
  delayMicroseconds(SYMBOL);

  // Data: bits are sent one by one, starting with the MSB.
  for (byte i = 0; i < 56; i++)
  {
    if (((this->m_frame[i / 8] >> (7 - (i % 8))) & 1) == 1)
    {
      SIG_LOW;
      delayMicroseconds(SYMBOL);
      SIG_HIGH;
      delayMicroseconds(SYMBOL);
    }
    else
    {
      SIG_HIGH;
      delayMicroseconds(SYMBOL);
      SIG_LOW;
      delayMicroseconds(SYMBOL);
    }
  }

  SIG_LOW;
  delayMicroseconds(30415); // Inter-frame silence
};

void RTSTransmitter::debugBuildedFrame(const int base)
{
  String debugFrame;
  for (int i = 0; i < 7; i++)
  {
    debugFrame += String(this->m_frame[i], base);
    debugFrame += " ";
  }
  debugFrame.toUpperCase();
  LOG_DEBUG("Frame:", debugFrame);
}