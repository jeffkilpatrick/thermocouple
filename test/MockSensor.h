//
//  MockSensor.h
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/29/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#ifndef Thermocouple_MockSensor_h
#define Thermocouple_MockSensor_h

#include "PollingSensor.h"

class MockSensor : public PollingSensor {

public:
    MockSensor(
        const SensorId& sensorId,
        std::chrono::milliseconds interval = std::chrono::seconds(1),
        float value = 35.0);

    ~MockSensor() noexcept;

    void SetValue(float value) noexcept;
protected:
    bool Poll(float& value) const noexcept override;

    std::atomic<float> m_value;

public:
    MockSensor(const MockSensor&) = delete;
    MockSensor& operator=(const MockSensor&) = delete;
};

#endif
