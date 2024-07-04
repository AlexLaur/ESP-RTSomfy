/**
 * @file observer.cpp
 * @author Laurette Alexandre
 * @brief Implementation for Observer pattern.
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

#include <observer.h>

void Subject::attach(Observer* observer)
{
  short freeIndex = -1;
  for (unsigned short i = 0; i < sizeof(this->m_observers) / sizeof(this->m_observers[0]); ++i)
  {
    if (this->m_observers[i] == nullptr)
    {
      freeIndex = i;
      break;
    }
  }

  if (freeIndex == -1)
  {
    LOG_ERROR("Cannot add a new observer. No space left to store another observer.");
    return;
  }
  this->m_observers[freeIndex] = observer;
}

void Subject::deattach(Observer* observer)
{
  for (unsigned short i = 0; i < sizeof(this->m_observers) / sizeof(this->m_observers[0]); ++i)
  {
    if (observer == this->m_observers[i])
    {
      this->m_observers[i] = nullptr;
    }
  }
}

void Subject::notify(const char* action, const Remote& remote)
{
  LOG_DEBUG("A message will be delivered to all observers.");
  for (unsigned short i = 0; i < sizeof(this->m_observers) / sizeof(this->m_observers[0]); ++i)
  {
    if (this->m_observers[i] == nullptr)
    {
      continue;
    }
    this->m_observers[i]->notified(action, remote);
  }
}
