//
//  PhidgetsSensor.h
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/30/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#ifndef Thermocouple_PhidgetsSensor_h
#define Thermocouple_PhidgetsSensor_h

#include "PollingSensor.h"

class PhidgetException : public std::runtime_error {
public:
    PhidgetException(int errorCode);
};

class PhidgetOpener {
public:
    virtual void OpenPhidget(void* handle, int serial) const = 0;
};

class TemperaturePhidget {
public:
    TemperaturePhidget(
        const PhidgetOpener& opener,
        int serial);

    ~TemperaturePhidget() noexcept;

    int GetInputs() const;
    void* GetHandle() const;

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;

public:
    TemperaturePhidget(const TemperaturePhidget&) = delete;
    TemperaturePhidget& operator=(const TemperaturePhidget&) = delete;
};

class PhidgetsSensor : public PollingSensor {
public:
    PhidgetsSensor(
        std::shared_ptr<TemperaturePhidget> phidget,
        const SensorId& sensorId,
        std::chrono::milliseconds interval = DefaultInterval);

protected:
    std::shared_ptr<TemperaturePhidget> m_phidget;
};

class PhidgetsProbeSensor : public PhidgetsSensor {
public:
    PhidgetsProbeSensor(
        std::shared_ptr<TemperaturePhidget> phidget,
        const SensorId& sensorId,
        int input,
        std::chrono::milliseconds interval = DefaultInterval);
    ~PhidgetsProbeSensor() noexcept;

    bool Poll(float& value) const override;

private:
    const int m_input;
};

class PhidgetsAmbientSensor : public PhidgetsSensor {
public:
    PhidgetsAmbientSensor(
        std::shared_ptr<TemperaturePhidget> phidget,
        const SensorId& sensorId,
        std::chrono::milliseconds interval = DefaultInterval);
    ~PhidgetsAmbientSensor() noexcept;

    bool Poll(float& value) const override;
};
#endif
