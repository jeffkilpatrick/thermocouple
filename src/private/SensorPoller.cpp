//
//  SensorPoller.cpp
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/30/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#include "SensorPoller.h"

#include "IListener.h"

/*static*/ const std::chrono::milliseconds SensorPoller::DefaultInterval = std::chrono::milliseconds(1000);

SensorPoller::SensorPoller(std::chrono::milliseconds interval)
    : m_interval(interval)
{ }

SensorPoller::~SensorPoller()
{
    try {
        Stop();
    }
    catch (...) { } // nothing worth crashing over here
}

void
SensorPoller::Start()
{
    if (!m_thread.joinable()) {
        m_threadExit = false;
        m_pollSleepMutex.lock();
        m_thread = std::thread([this] () { RunLoop(); });
    }
}

void
SensorPoller::Stop()
{
    if (m_thread.joinable()) {
        m_threadExit = true;
        m_pollSleepMutex.unlock();
        m_thread.join();
    }
}

void
SensorPoller::RunLoop()
{
    while (!m_threadExit) {
        auto now = std::chrono::system_clock::now();
        for (const auto& sensorPtr : m_sensors ) {
            auto sensor = sensorPtr.lock();
            if (!sensor) {
                // TODO-jrk: remove sensor
                continue;
            }

            sensor->PollAndNotify();
        }

        auto wake = now + m_interval;
        m_pollSleepMutex.try_lock_until(wake);
    }
}

