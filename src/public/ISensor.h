//
//  ISensor.h
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/22/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#pragma once

#include "Subscription.h"

#include <chrono>
#include <cstdint>
#include <string>

#pragma GCC visibility push(default)

class IListener;

class ISensor {
public:
    using SensorId = std::string;
    using SubscriptionId = uint32_t;

    ~ISensor() = default;
    ISensor(const ISensor&) = delete;
    ISensor(ISensor&&) = delete;
    ISensor& operator=(const ISensor&) = delete;
    ISensor& operator=(ISensor&&) = delete;

    // Get this sensor's unique identifier.
    [[nodiscard]]
    virtual const SensorId& Identifier() const = 0;

    // Register to be notified.
    //
    // @param subId
    //      The identifier to associate with this subscription. Any existing
    //      subscription with this identifier will be replaced.
    // @param subscription
    //      Notification policy.
    virtual void AddNotification(
        SubscriptionId subId,
        std::shared_ptr<Subscription> subscription) = 0;

    // Temporarily stop receiving notifications.
    virtual void Pause(SubscriptionId) = 0;

    // Resume receiving notifications.
    virtual void Unpause(SubscriptionId) = 0;

    // Permantently stop receiving notifications.
    virtual void Unsubscribe(SubscriptionId) = 0;

    // Get the status of this sensor.
    // @return If the sensor has no subscriptions, UNKNOWN_SUBSCRIPTION
    //         is returned. If all subscriptions are paused, the result
    //         is PAUSED. Otherwise, ACTIVE is returned.
    [[nodiscard]]
    virtual Subscription::Status GetStatus() const = 0;

    // Get the status of a specific sensor.
    [[nodiscard]]
    virtual Subscription::Status GetStatus(SubscriptionId) const = 0;

protected:
    ISensor() = default;
};

#pragma GCC visibility pop
