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
    float value)

    : AbstractSensor(sensorId)
    , m_value(value)
{ }

void
MockSensor::SetValue(float value)
{
    m_value = value;
}

void
MockSensor::PollAndNotify()
{
    Notify(m_value);
}
