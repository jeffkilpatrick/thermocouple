//
//  PollingSensor.h
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/30/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#pragma once

#include "AbstractSensor.h"

#include <atomic>
#include <mutex>
#include <thread>
#include <unordered_map>

//
// A polling sensor.
//
class PollingSensor : public AbstractSensor {

public:
    virtual ~PollingSensor() noexcept;

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

    using RecursiveMutex = std::recursive_mutex;
    using ScopedRecursiveLock = std::lock_guard<RecursiveMutex>;

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
    std::timed_mutex m_pollSleepMutex;
    std::atomic<bool> m_threadExit;

    //
    // Deleted defaults
    //
    PollingSensor(const PollingSensor&) = delete;
    PollingSensor& operator=(const PollingSensor&) = delete;
};
