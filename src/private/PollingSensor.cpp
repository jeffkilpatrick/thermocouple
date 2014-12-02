//
//  PollingSensor.cpp
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/30/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#include "PollingSensor.h"

#include "IListener.h"

/*static*/ const std::chrono::milliseconds PollingSensor::DefaultInterval = std::chrono::milliseconds(1000);

PollingSensor::PollingSensor(
    const SensorId& sensorId,
    std::chrono::milliseconds interval)

    : AbstractSensor(sensorId)
    , m_interval(interval)
{ }

PollingSensor::~PollingSensor()
{
    // Last ditch effort. If this does anything, we're going to
    // crash with some regularity. Our child should have
    // called Stop() in their destructor.
    Stop();
}

void
PollingSensor::AddNotification(
    SubscriptionId subId,
    std::shared_ptr<Subscription> subscription)
{
    ScopedRecursiveLock lock(m_subscriptionsMutex);

    Unsubscribe(subId);
    m_subscriptions.emplace(subId, subscription);
}

void
PollingSensor::Pause(SubscriptionId subId)
{
    ScopedRecursiveLock lock(m_subscriptionsMutex);

    auto sub = m_subscriptions.find(subId);
    if (sub != m_subscriptions.end()) {
        sub->second->SetStatus(Subscription::Status::PAUSED);
    }
}

void
PollingSensor::Unpause(SubscriptionId subId)
{
    ScopedRecursiveLock lock(m_subscriptionsMutex);

    auto sub = m_subscriptions.find(subId);
    if (sub != m_subscriptions.end()) {
        sub->second->SetStatus(Subscription::Status::ACTIVE);
    }
}

void
PollingSensor::Unsubscribe(SubscriptionId subId)
{
    ScopedRecursiveLock lock(m_subscriptionsMutex);

    m_subscriptions.erase(subId);
}

Subscription::Status
PollingSensor::GetStatus() const
{
    ScopedRecursiveLock lock(m_subscriptionsMutex);

    if (m_subscriptions.empty()) {
        return Subscription::Status::UNKNOWN_SUBSCRIPTION;
    }

    for (auto sub = m_subscriptions.begin(); sub != m_subscriptions.end(); ++sub) {
        if (sub->second->GetStatus() == Subscription::Status::ACTIVE) {
            return Subscription::Status::ACTIVE;
        }
    }

    // All must be paused
    return Subscription::Status::PAUSED;
}

Subscription::Status
PollingSensor::GetStatus(SubscriptionId subId) const
{
    ScopedRecursiveLock lock(m_subscriptionsMutex);

    auto sub = m_subscriptions.find(subId);
    if (sub == m_subscriptions.end()) {
        return Subscription::Status::UNKNOWN_SUBSCRIPTION;
    }
    
    return sub->second->GetStatus();
}

void
PollingSensor::Start()
{
    if (!m_thread.joinable()) {
        m_threadExit = false;
        m_thread = std::thread([this] () { RunLoop(); });
    }
}

void
PollingSensor::Stop()
{
    if (m_thread.joinable()) {
        m_threadExit = true;
        m_thread.join();
    }
}

void
PollingSensor::RunLoop()
{
    while (!m_threadExit) {
        float value;
        bool valid = Poll(value);

        if (valid) {
            auto now = std::chrono::system_clock::now();
            ScopedRecursiveLock lock(m_subscriptionsMutex);
            for (auto subEntry = m_subscriptions.begin(); subEntry != m_subscriptions.end(); ++subEntry) {
                auto& sub = subEntry->second;

                // Erase subscriptions that go nowhere
                if (sub->GetListener().expired()) {
                    m_subscriptions.erase(subEntry);
                    continue;
                }

                // Skip non-active subscriptions
                if (sub->GetStatus() != Subscription::Status::ACTIVE) {
                    continue;
                }

                // Notify!
                if (sub->ShouldNotify(value, now)) {
                    sub->Notify(value, now);
                }
            }
        } // valid

        std::this_thread::sleep_for(m_interval);
    }
}

