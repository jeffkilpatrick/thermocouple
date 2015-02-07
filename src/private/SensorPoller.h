//
//  SensorPoller.h
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/30/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#ifndef Thermocouple_SensorPoller_h
#define Thermocouple_SensorPoller_h

#include "AbstractSensor.h"

#include <atomic>
#include <thread>
#include <vector>

class IPollableSensor {
public:
    virtual void PollAndNotify() = 0;
};

//
// A multi-sensor polling loop.
//
class SensorPoller {

public:
    static const std::chrono::milliseconds DefaultInterval;

    SensorPoller(std::chrono::milliseconds pollInterval = DefaultInterval);
    ~SensorPoller();

    template<class T, class... Ts>
    std::shared_ptr<T> CreateSensor(Ts&&... params)
    {
        auto sensor = std::make_shared<T>(std::forward<Ts>(params)...);
        m_sensors.push_back(sensor);
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
    std::vector<std::weak_ptr<IPollableSensor>> m_sensors;
    std::timed_mutex m_pollSleepMutex;
    std::atomic<bool> m_threadExit;

public:
    SensorPoller(const SensorPoller&) = delete;
    SensorPoller& operator=(const SensorPoller&) = delete;
};

#endif
