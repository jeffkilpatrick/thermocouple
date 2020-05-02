//
//  Subscription.h
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/30/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#pragma once

#include <chrono>
#include <memory>

class IListener;
class Subscription {
public:
    enum class Status {
        UNKNOWN_SUBSCRIPTION,
        NO_SENSOR,
        ACTIVE,
        PAUSED
    };

    using Clock = std::chrono::system_clock;

    virtual ~Subscription() = default;
    Subscription(const Subscription&) = delete;
    Subscription(Subscription&&) = delete;
    Subscription& operator=(const Subscription&) = delete;
    Subscription& operator=(Subscription&&) = delete;

    [[nodiscard]] std::weak_ptr<IListener> GetListener() const;

    [[nodiscard]] Status GetStatus() const;
    void SetStatus(Status s);

    [[nodiscard]]
    virtual bool ShouldNotify(
        float currentValue,
        Clock::time_point currentTime) const = 0;

    virtual void Notify(
        float currentValue,
        Clock::time_point currentTime);

protected:
    explicit Subscription(std::weak_ptr<IListener> listener);

private:
    std::weak_ptr<IListener> m_listener;
    Status m_status;
};

//
//
//
class IntervalSubscription : public Subscription {
public:
    using Interval = std::chrono::milliseconds;

    IntervalSubscription(
        std::weak_ptr<IListener> listener,
        Interval interval);

    [[nodiscard]]
    bool ShouldNotify(
        float currentValue,
        Clock::time_point currentTime) const override;

    void Notify(
        float currentValue,
        Clock::time_point currentTime) override;

private:
    const Interval m_interval;
    Clock::time_point m_lastNotification;
};

//
//
//
class OnChangeSubscription : public Subscription {
public:
    OnChangeSubscription(
        std::weak_ptr<IListener> listener,
        float delta);

    [[nodiscard]]
    bool ShouldNotify(
        float currentValue,
        Clock::time_point currentTime) const override;

    void Notify(
        float currentValue,
        Clock::time_point currentTime) override;

private:
    const float m_delta;
    float m_lastValue;
};
