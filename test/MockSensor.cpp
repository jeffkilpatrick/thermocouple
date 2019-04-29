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
    SensorId sensorId,
    ValuesVec&& values)

    : AbstractSensor(std::move(sensorId))
    , m_values(std::move(values))
{ }

void
MockSensor::SetValues(ValuesVec&& values)
{
    std::unique_lock lock(m_valuesMutex);
    m_values = std::move(values);
    m_valueIndex = 0;
}

void
MockSensor::PollAndNotify()
{
    std::shared_lock lock(m_valuesMutex);
    Notify(m_values[m_valueIndex]);
    m_valueIndex = (m_valueIndex + 1) % m_values.size();
}
