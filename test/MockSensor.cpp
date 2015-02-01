//
//  MockSensor.cpp
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/29/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#include "MockSensor.h"

#include "IListener.h"

MockSensor::MockSensor(
    const SensorId& sensorId,
    std::chrono::milliseconds interval,
    float value)

    : PollingSensor(sensorId, interval)
    , m_value(value)
{ }

MockSensor::~MockSensor() noexcept
{
	try {
	    Stop();
	}
	catch (...) { } // not worth crashing
}

void
MockSensor::SetValue(float value) noexcept
{
    m_value = value;
}

bool
MockSensor::Poll(float& value) const noexcept
{
    value = m_value;
    return true;
}
