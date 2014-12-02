//
//  PollingSensor.h
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/30/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#ifndef Thermocouple_PollingSensor_h
#define Thermocouple_PollingSensor_h

#include "AbstractSensor.h"

#include <mutex>
#include <thread>
#include <unordered_map>

//
// A polling sensor.
//
class PollingSensor : public AbstractSensor {

public:
    virtual ~PollingSensor();

    void AddNotification(
        SubscriptionId subId,
        std::shared_ptr<Subscription>) override;

    void Pause(SubscriptionId subId) override;
    void Unpause(SubscriptionId subId) override;
    void Unsubscribe(SubscriptionId subId) override;

    Subscription::Status GetStatus() const override;
    Subscription::Status GetStatus(SubscriptionId subId) const override;

    void Start();
    void Stop();

    static const std::chrono::milliseconds DefaultInterval;

protected:

    PollingSensor(
        const SensorId& sensorId,
        std::chrono::milliseconds pollInterval = DefaultInterval);

    virtual bool Poll(float& value) const = 0;

private:

    //
    // Types
    //
    
    typedef std::recursive_mutex RecursiveMutex;
    typedef std::lock_guard<RecursiveMutex> ScopedRecursiveLock;

    //
    // The thread's main()
    //
    void RunLoop();

    //
    // Private data
    //
    std::unordered_map<SubscriptionId, std::shared_ptr<Subscription>> m_subscriptions;
    const std::chrono::milliseconds m_interval;
    mutable RecursiveMutex m_subscriptionsMutex;

    std::thread m_thread;
    std::atomic<bool> m_threadExit;

    //
    // Deleted defaults
    //
    PollingSensor(const PollingSensor&) = delete;
    PollingSensor& operator=(const PollingSensor&) = delete;
};

#endif
