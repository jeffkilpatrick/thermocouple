//
//  SensorBroker.cpp
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/22/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#include "SensorBroker.h"

#include <atomic>
#include <unordered_map>

#pragma mark SensorBroker::Impl Decl

struct SensorBroker::Impl {
    ISensor::SubscriptionId AddNotification(
        const ISensor::SensorId& sensorId,
        std::shared_ptr<Subscription> sub);

    std::unordered_map<ISensor::SensorId, std::shared_ptr<ISensor>> m_sensors;
    std::unordered_map<ISensor::SubscriptionId, std::pair<ISensor::SensorId, std::shared_ptr<Subscription>>> m_subscriptions;
    std::atomic<ISensor::SubscriptionId> m_lastSubscriptionId;
};

ISensor::SubscriptionId
SensorBroker::Impl::AddNotification(
    const ISensor::SensorId& sensorId,
    std::shared_ptr<Subscription> sub)
{
    auto subId = ++m_lastSubscriptionId;

    // Record the subscription, even if we don't have a sensor yet.
    m_subscriptions[subId] = std::make_pair(sensorId, sub);

    // If we do have a sensor, start up notifications.
    auto sensor = m_sensors.find(sensorId);
    if (sensor != m_sensors.end()) {
        sensor->second->AddNotification(subId, sub);
    }

    return subId;
}

//
//
//

namespace {
    std::unique_ptr<SensorBroker> s_instance;
}

/*static*/ SensorBroker&
SensorBroker::Instance()
{
    if (!s_instance) {
        s_instance.reset(new SensorBroker()); // TODO-jrk
    }

    return *s_instance;
}

/*static*/ SensorBroker*
SensorBroker::GetInstance()
{
    return s_instance.get();
}

SensorBroker::SensorBroker()
    : m_impl(new Impl())
{ }

void
SensorBroker::Reset()
{
    std::unique_ptr<Impl> newImpl(new Impl);
    m_impl.swap(newImpl);
}

std::set<ISensor::SensorId>
SensorBroker::AvailableSensors() const
{
    std::set<ISensor::SensorId> sensors;

    for (const auto& s : m_impl->m_sensors) {
        if (s.second && s.second->GetStatus() != Subscription::Status::NO_SENSOR) {
            sensors.insert(s.first);
        }
    }

    return std::move(sensors);
}

ISensor::SubscriptionId
SensorBroker::NotifyInterval(
    std::weak_ptr<IListener> listener,
    const ISensor::SensorId& sensorId,
    std::chrono::milliseconds interval)
{
    auto sub = std::make_shared<IntervalSubscription>(listener, interval);
    return m_impl->AddNotification(sensorId, sub);
}

ISensor::SubscriptionId
SensorBroker::NotifyOnChange(
    std::weak_ptr<IListener> listener,
    const ISensor::SensorId& sensorId,
    float delta)
{
    auto sub = std::make_shared<OnChangeSubscription>(listener, delta);
    return m_impl->AddNotification(sensorId, sub);
}

bool
SensorBroker::Unsubscribe(ISensor::SubscriptionId subId)
{
    auto sub = m_impl->m_subscriptions.find(subId);
    if (sub == m_impl->m_subscriptions.end()) {
        return false;
    }

    auto sensor = m_impl->m_sensors.find(sub->second.first);
    if (sensor != m_impl->m_sensors.end()) {
        sensor->second->Unsubscribe(subId);
    }

    m_impl->m_subscriptions.erase(sub);

    return true;
}

bool
SensorBroker::Pause(ISensor::SubscriptionId subId)
{
    auto subEntry = m_impl->m_subscriptions.find(subId);
    if (subEntry == m_impl->m_subscriptions.end()) {
        return false;
    }

    auto sub = subEntry->second.second;

    if (sub->GetStatus() == Subscription::Status::ACTIVE) {
        sub->SetStatus(Subscription::Status::PAUSED);
        return true;
    }

    return false;
}

bool
SensorBroker::Unpause(ISensor::SubscriptionId subId)
{
    auto subEntry = m_impl->m_subscriptions.find(subId);
    if (subEntry == m_impl->m_subscriptions.end()) {
        return false;
    }

    auto sub = subEntry->second.second;

    if (sub->GetStatus() == Subscription::Status::PAUSED) {
        sub->SetStatus(Subscription::Status::ACTIVE);
        return true;
    }

    return false;
}

Subscription::Status
SensorBroker::GetStatus(ISensor::SubscriptionId subId) const
{
    auto sub = m_impl->m_subscriptions.find(subId);
    if (sub == m_impl->m_subscriptions.end()) {
        return Subscription::Status::UNKNOWN_SUBSCRIPTION;
    }

    auto sensor = m_impl->m_sensors.find(sub->second.first);
    if (sensor == m_impl->m_sensors.end()) {
        return Subscription::Status::NO_SENSOR;
    }

    return sensor->second->GetStatus(subId);
}

bool
SensorBroker::Register(std::shared_ptr<ISensor> sensor)
{
    if (sensor) {
        auto ident = sensor->Identifier();
        if (!m_impl->m_sensors.insert(std::make_pair(ident, sensor)).second) {
            return false;
        }

        for (const auto& sub : m_impl->m_subscriptions) {
            if (sub.second.first == ident) {
                sub.second.second->SetStatus(Subscription::Status::ACTIVE);
                sensor->AddNotification(sub.first, sub.second.second);
            }
        }

        return true;
    }

    return false;
}

bool
SensorBroker::Unregister(const ISensor::SensorId& sensorId)
{
    return m_impl->m_sensors.erase(sensorId) > 0;
}

