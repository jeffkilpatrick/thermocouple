//
//  MockSensor.h
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/29/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#pragma once

#include "SensorPoller.h"

#include <shared_mutex>
#include <vector>

class MockSensor
    : public IPollableSensor
    , public AbstractSensor
{
public:
    using ValuesVec = std::vector<float>;

    MockSensor(
        SensorId sensorId,
        ValuesVec&& values);

    void SetValues(ValuesVec&& values);

protected:
    void PollAndNotify() override;

    ValuesVec m_values;
    mutable std::shared_mutex m_valuesMutex;

    ValuesVec::size_type m_valueIndex{0};

public:
    MockSensor(const MockSensor&) = delete;
    MockSensor& operator=(const MockSensor&) = delete;
};
