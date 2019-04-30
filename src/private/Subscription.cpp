//
//  Subscription.cpp
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/30/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#include "Subscription.h"

#include "IListener.h"
#include <cmath>

Subscription::Subscription(std::weak_ptr<IListener> listener)
    : m_listener(std::move(listener))
    , m_status(Subscription::Status::ACTIVE)
{ }

Subscription::Status
Subscription::GetStatus() const
{
    return m_status;
}

std::weak_ptr<IListener>
Subscription::GetListener() const
{
    return m_listener;
}

void
Subscription::SetStatus(Subscription::Status s)
{
    m_status = s;
}

void
Subscription::Notify(
    float currentValue,
    Clock::time_point /*currentTime*/)
{
    auto listener = m_listener.lock();
    if (listener) {
        listener->Notify(currentValue);
    }
}

//
#pragma mark IntervalSubscription
//

IntervalSubscription::IntervalSubscription(
    std::weak_ptr<IListener> listener,
    Interval interval)

    : Subscription(std::move(listener))
    , m_interval(interval)
{ }

bool
IntervalSubscription::ShouldNotify(
    float /*currentValue*/,
    Clock::time_point currentTime) const
{
    auto interval = currentTime - m_lastNotification;
    return interval >= m_interval;
}

void
IntervalSubscription::Notify(
    float currentValue,
    Clock::time_point currentTime)
{
    m_lastNotification = currentTime;
    Subscription::Notify(currentValue, currentTime);
}

//
#pragma mark OnChangeSubscription
//

OnChangeSubscription::OnChangeSubscription(
    std::weak_ptr<IListener> listener,
    float delta)

    : Subscription(std::move(listener))
    , m_delta(delta)
    , m_lastValue(std::numeric_limits<float>::min())
{ }

bool
OnChangeSubscription::ShouldNotify(
    float currentValue,
    Clock::time_point /*currentTime*/) const
{
    auto delta = fabsf(currentValue - m_lastValue);
    return delta >= m_delta;
}

void
OnChangeSubscription::Notify(
    float currentValue,
    Clock::time_point currentTime)
{
    m_lastValue = currentValue;
    Subscription::Notify(currentValue, currentTime);
}
