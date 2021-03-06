//
//  AbstractSensor.h
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/29/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#pragma once

#include "ISensor.h"
#include <mutex>
#include <unordered_map>

class AbstractSensor : public ISensor {

protected:
    explicit AbstractSensor(SensorId sensorId);

public:
    const SensorId& Identifier() const override;

    void AddNotification(
        SubscriptionId subId,
        std::shared_ptr<Subscription> subscription) override;

    void Pause(SubscriptionId subId) override;
    void Unpause(SubscriptionId subId) override;
    void Unsubscribe(SubscriptionId subId) override;

    Subscription::Status GetStatus() const override;
    Subscription::Status GetStatus(SubscriptionId subId) const override;

protected:
	using SystemTime = std::chrono::time_point<std::chrono::system_clock>;
	virtual void Notify(
		float value,
		const SystemTime& time);

private:
    using RecursiveMutex = std::recursive_mutex;
    using ScopedRecursiveLock = std::lock_guard<RecursiveMutex>;

    const SensorId m_identifier;
	std::unordered_map<SubscriptionId, std::shared_ptr<Subscription>> m_subscriptions;
	mutable RecursiveMutex m_subscriptionsMutex;
};
