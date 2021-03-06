//
//  SensorPoller.h
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/30/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#pragma once

#include "AbstractSensor.h"

#include <atomic>
#include <shared_mutex>
#include <thread>
#include <vector>

class IPollableSensor {
public:

    //
    // Poll the sensor and notify any watchers.
    //
    virtual void PollAndNotify() = 0;
};

//
// A multi-sensor polling loop.
//
class SensorPoller {

public:
    static const std::chrono::milliseconds DefaultInterval;

    ~SensorPoller();

    SensorPoller(const SensorPoller&) = delete;
    SensorPoller(SensorPoller&&) = delete;
    SensorPoller& operator=(const SensorPoller&) = delete;
    SensorPoller& operator=(SensorPoller&&) = delete;

    explicit SensorPoller(std::chrono::milliseconds pollInterval = DefaultInterval);

    template<class T, class... Ts>
    std::shared_ptr<T> CreateSensor(Ts&&... params)
    {
        auto sensor = std::make_shared<T>(std::forward<Ts>(params)...);

        {
            std::unique_lock lock(m_sensorMutex);
            m_sensors.push_back(sensor);
        }

        return sensor;
    }

    void Start();
    void Stop();

private:

    //
    // The thread's main()
    //
    void RunLoop();

    //
    // Private data
    //
    const std::chrono::milliseconds m_interval;

    std::thread m_thread;
    std::vector<std::shared_ptr<IPollableSensor>> m_sensors;
    std::shared_mutex m_sensorMutex;
    std::timed_mutex m_pollSleepMutex;
    std::atomic<bool> m_threadExit;
};
