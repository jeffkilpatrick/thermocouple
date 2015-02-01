//
//  SensorBroker.h
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/22/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#ifndef Thermocouple_SensorBroker_h
#define Thermocouple_SensorBroker_h

#include "IListener.h"
#include "ISensor.h"

#include <set>

#pragma GCC visibility push(default)

class SensorBroker {
public:
    // Get the SensorBroker instance, initializing if necessary.
    static SensorBroker& Instance();

    // Get the SensorBroker instance, but do not initialize.
    static SensorBroker* GetInstance();

    // Get the set of currently available sensors.
    std::set<ISensor::SensorId> AvailableSensors() const;

    // Notify the specified listener of the temperature at the given interval.
    ISensor::SubscriptionId NotifyInterval(
        std::weak_ptr<IListener> listener,
        const ISensor::SensorId& sensor,
        std::chrono::milliseconds interval);

    // Notify the specified listener when the temperature changes by at least delta.
    ISensor::SubscriptionId NotifyOnChange(
        std::weak_ptr<IListener> listener,
        const ISensor::SensorId& sensor,
        float delta);

    // Purge the speicifed subscription.
    // @return true if the given subscription is valid.
    bool Unsubscribe(ISensor::SubscriptionId subscription);

    // Pause the speicifed subscription.
    // @return true if the given subscription is valid.
    bool Pause(ISensor::SubscriptionId subscription);

    // Unpause the speicifed subscription.
    // @return true if the given subscription is valid.
    bool Unpause(ISensor::SubscriptionId subscription);

    // Get the status of the given subscription.
    Subscription::Status GetStatus(ISensor::SubscriptionId subscription) const;

    // Register a sensor.
    // @return true if the given sensor's ID is unique and registration succeeds.
    bool Register(std::shared_ptr<ISensor> sensor);

    // Unregister a sensor
    // @return true if the given ID is valid and deregistration succeeds.
    bool Unregister(const ISensor::SensorId& sensorId);

    // Reset the broker, forgetting all sensors and subscriptions.
    void Reset();
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;

    SensorBroker();

public:
    SensorBroker(const SensorBroker&) = delete;
    SensorBroker& operator=(const SensorBroker&) = delete;
};

#pragma GCC visibility pop

#endif
