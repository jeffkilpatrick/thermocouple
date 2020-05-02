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

class SensorBroker::Impl {
public:
    using Sensors = std::unordered_map<ISensor::SensorId, std::shared_ptr<ISensor>>;
    using Subscriptions = std::unordered_map<ISensor::SubscriptionId, std::pair<ISensor::SensorId, std::shared_ptr<Subscription>>>;
    ISensor::SubscriptionId AddNotification(
        const ISensor::SensorId& sensorId,
        std::shared_ptr<Subscription> sub);

    bool AddSensor(const ISensor::SensorId& sensorId, std::shared_ptr<ISensor> sensor);
    const Sensors& GetSensors() const { return m_sensors; }
    ISensor* GetSensor(const ISensor::SensorId& sensorId);
    bool RemoveSensor(const ISensor::SensorId& sensorId);

    const Subscriptions& GetSubscriptions() const { return m_subscriptions; }
    Subscription* GetSubscription(ISensor::SubscriptionId subId);
    const ISensor::SensorId* GetSubscriptionSensor(ISensor::SubscriptionId subId) const;
    void RemoveSubscription(ISensor::SubscriptionId subId);

private:
    Sensors m_sensors;
    Subscriptions m_subscriptions;
    std::atomic<ISensor::SubscriptionId> m_lastSubscriptionId{};
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
        sensor->second->AddNotification(subId, std::move(sub));
    }

    return subId;
}

bool
SensorBroker::Impl::AddSensor(const ISensor::SensorId& sensorId, std::shared_ptr<ISensor> sensor)
{
    auto [_, inserted] = m_sensors.insert(std::make_pair(sensorId, std::move(sensor)));
    return inserted;
}

ISensor*
SensorBroker::Impl::GetSensor(const ISensor::SensorId& sensorId)
{
    auto s = m_sensors.find(sensorId);
    return s == m_sensors.cend() ? nullptr : s->second.get();
}

bool
SensorBroker::Impl::RemoveSensor(const ISensor::SensorId& sensorId)
{
    return m_sensors.erase(sensorId) > 0;
}

Subscription*
SensorBroker::Impl::GetSubscription(ISensor::SubscriptionId subId)
{
    auto s = m_subscriptions.find(subId);
    return s == m_subscriptions.cend() ? nullptr : s->second.second.get();
}


const ISensor::SensorId*
SensorBroker::Impl::GetSubscriptionSensor(ISensor::SubscriptionId subId) const
{
    auto s = m_subscriptions.find(subId);
    return s == m_subscriptions.cend() ? nullptr : &s->second.first;
}

void
SensorBroker::Impl::RemoveSubscription(ISensor::SubscriptionId subId)
{
    m_subscriptions.erase(subId);
}

//
//
//

namespace {
    std::unique_ptr<SensorBroker> s_instance;
} // unnamed namespace

/*static*/ SensorBroker&
SensorBroker::Instance()
{
    if (!s_instance) {
        s_instance.reset(new SensorBroker()); // NOLINT(cppcoreguidelines-owning-memory)
    }

    return *s_instance;
}

/*static*/ SensorBroker*
SensorBroker::GetInstance()
{
    return s_instance.get();
}

SensorBroker::~SensorBroker() = default;

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

    for (const auto& s : m_impl->GetSensors()) {
        if (s.second && s.second->GetStatus() != Subscription::Status::NO_SENSOR) {
            sensors.insert(s.first);
        }
    }

    return sensors;
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
SensorBroker::Unsubscribe(ISensor::SubscriptionId subscription)
{
    const auto* sensorId = m_impl->GetSubscriptionSensor(subscription);
    if (sensorId == nullptr) {
        return false;
    }

    auto* sensor = m_impl->GetSensor(*sensorId);
    if (sensor != nullptr) {
        sensor->Unsubscribe(subscription);
    }

    m_impl->RemoveSubscription(subscription);

    return true;
}

bool
SensorBroker::Pause(ISensor::SubscriptionId subscription)
{
    auto* sub = m_impl->GetSubscription(subscription);
    if (sub == nullptr) {
        return false;
    }

    if (sub->GetStatus() == Subscription::Status::ACTIVE) {
        sub->SetStatus(Subscription::Status::PAUSED);
        return true;
    }

    return false;
}

bool
SensorBroker::Unpause(ISensor::SubscriptionId subscription)
{
    auto* sub = m_impl->GetSubscription(subscription);
    if (sub == nullptr) {
        return false;
    }

    if (sub->GetStatus() == Subscription::Status::PAUSED) {
        sub->SetStatus(Subscription::Status::ACTIVE);
        return true;
    }

    return false;
}

Subscription::Status
SensorBroker::GetStatus(ISensor::SubscriptionId subscription) const
{
    const auto* sensorId = m_impl->GetSubscriptionSensor(subscription);
    if (sensorId == nullptr) {
        return Subscription::Status::UNKNOWN_SUBSCRIPTION;
    }

    const auto* sensor = m_impl->GetSensor(*sensorId);
    if (sensor == nullptr) {
        return Subscription::Status::NO_SENSOR;
    }

    return sensor->GetStatus(subscription);
}

bool
SensorBroker::Register(const std::shared_ptr<ISensor>& sensor)
{
    if (sensor) {
        auto ident = sensor->Identifier();
        auto inserted = m_impl->AddSensor(ident, sensor);
        if (!inserted) {
            return false;
        }

        for (const auto& sub : m_impl->GetSubscriptions()) {
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
    return m_impl->RemoveSensor(sensorId);
}

