//
//  AbstractSensor.cpp
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/29/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#include "AbstractSensor.h"

AbstractSensor::AbstractSensor(ISensor::SensorId sensorId)
    : m_identifier(std::move(sensorId))
{ }

const ISensor::SensorId&
AbstractSensor::Identifier() const
{
    return m_identifier;
}

void
AbstractSensor::AddNotification(
    SubscriptionId subId,
    std::shared_ptr<Subscription> subscription)
{
    ScopedRecursiveLock lock(m_subscriptionsMutex);
    Unsubscribe(subId);
    m_subscriptions.emplace(subId, subscription);
}

void
AbstractSensor::Pause(SubscriptionId subId)
{
    ScopedRecursiveLock lock(m_subscriptionsMutex);
    auto sub = m_subscriptions.find(subId);
    if (sub != m_subscriptions.end()) {
        sub->second->SetStatus(Subscription::Status::PAUSED);
    }
}

void
AbstractSensor::Unpause(SubscriptionId subId)
{
    ScopedRecursiveLock lock(m_subscriptionsMutex);
    auto sub = m_subscriptions.find(subId);
    if (sub != m_subscriptions.end()) {
        sub->second->SetStatus(Subscription::Status::ACTIVE);
    }
}

void
AbstractSensor::Unsubscribe(SubscriptionId subId)
{
    ScopedRecursiveLock lock(m_subscriptionsMutex);
    m_subscriptions.erase(subId);
}

Subscription::Status
AbstractSensor::GetStatus() const
{
    ScopedRecursiveLock lock(m_subscriptionsMutex);
    if (m_subscriptions.empty()) {
        return Subscription::Status::UNKNOWN_SUBSCRIPTION;
    }

    for (const auto& sub : m_subscriptions) {
        if (sub.second->GetStatus() == Subscription::Status::ACTIVE) {
            return Subscription::Status::ACTIVE;
        }
    }

    // All must be paused
    return Subscription::Status::PAUSED;
}

Subscription::Status
AbstractSensor::GetStatus(SubscriptionId subId) const
{
    ScopedRecursiveLock lock(m_subscriptionsMutex);
    auto sub = m_subscriptions.find(subId);
    if (sub == m_subscriptions.end()) {
        return Subscription::Status::UNKNOWN_SUBSCRIPTION;
    }

    return sub->second->GetStatus();
}

void
AbstractSensor::Notify(
	float value,
	const SystemTime& time)
{
    ScopedRecursiveLock lock(m_subscriptionsMutex);
    for (auto& idAndSub : m_subscriptions) {
        auto& sub = idAndSub.second;

        // Disable subscriptions that go nowhere
        if (sub->GetListener().expired()) {
            sub->SetStatus(Subscription::Status::UNKNOWN_SUBSCRIPTION);
            continue;
        }

        // Skip non-active subscriptions
        if (sub->GetStatus() != Subscription::Status::ACTIVE) {
            continue;
        }

        // Notify!
        if (sub->ShouldNotify(value, time)) {
            sub->Notify(value, time);
        }
    }
}
