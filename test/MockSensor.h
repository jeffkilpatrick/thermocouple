//
//  MockSensor.h
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/29/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#ifndef Thermocouple_MockSensor_h
#define Thermocouple_MockSensor_h

#include "SensorPoller.h"

class MockSensor
    : public IPollableSensor
    , public AbstractSensor
{
public:
    MockSensor(
        const SensorId& sensorId,
        float value = 35.0);

    void SetValue(float value);
protected:
    void PollAndNotify() override;

    std::atomic<float> m_value;

public:
    MockSensor(const MockSensor&) = delete;
    MockSensor& operator=(const MockSensor&) = delete;
};

#endif
