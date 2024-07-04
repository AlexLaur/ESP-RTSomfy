/**
 * @file config.h
 * @author Laurette Alexandre
 * @brief Configuration for SOMFY Controller.
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

const char APP_NAME[] = "ESP-RTSomfy";
const char FIRMWARE_VERSION[] = "2.1.0";

const char AP_SSID[] = "ESP-RTSomfy Fallback Hotspot";
const char AP_PASSWORD[] = "5cKErSRCyQzy";

const unsigned short SERVER_PORT = 80;

const unsigned short MAX_NETWORK_SCAN = 15;

// Only 16 chars for the name.
// Warning: Increase with value will take more space in the database.
// If some remotes exists. These will be erase.
const unsigned short MAX_REMOTE_NAME_LENGTH = 17;  // 16 chars + 1 (\0)
const unsigned short MAX_REMOTES = 16;
const unsigned long REMOTE_BASE_ADDRESS = 0x100000;

const unsigned short DEFAULT_MQTT_PORT = 1883;