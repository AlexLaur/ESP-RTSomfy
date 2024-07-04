/**
 * @file RTSTransmitter.h
 * @author Laurette Alexandre
 * @brief Header for RTS Protocol.
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

#include <Arduino.h>
#include <transmitterAbs.h>

class RTSTransmitter : public TransmitterAbstract
{
  public:
  void init();
  bool sendUpCmd(const unsigned long remoteId, const unsigned int rollingCode);
  bool sendStopCmd(const unsigned long remoteId, const unsigned int rollingCode);
  bool sendDownCmd(const unsigned long remoteId, const unsigned int rollingCode);
  bool sendProgCmd(const unsigned long remoteId, const unsigned int rollingCode);

  // Only to allow tests on buildFrame method.
  byte* getBytesFrame();
  size_t getBytesFrameSize();

  private:
  byte m_frame[7];

  void buildFrame(const unsigned long remoteId, const unsigned int rollingCode, const byte action);
  void sendCommand(); // Will call sendCommand(sync)
  void sendCommand(const byte sync);
  void debugBuildedFrame(const int base);
};