//
//  PhidgetsSensor.h
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/30/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#pragma once

#include "SensorPoller.h"

class PhidgetException : public std::runtime_error {
public:
    explicit PhidgetException(int errorCode);
};

//
//
//

class PhidgetOpener {
public:
    virtual void OpenPhidget(void* handle, int serial) const = 0;
};

//
//
//

class TemperaturePhidget {
public:
    constexpr static int TIMEOUT_MS = 5000;

    ~TemperaturePhidget();
    TemperaturePhidget(const TemperaturePhidget&) = delete;
    TemperaturePhidget(TemperaturePhidget&&) = delete;
    TemperaturePhidget& operator=(const TemperaturePhidget&) = delete;
    TemperaturePhidget& operator=(TemperaturePhidget&&) = delete;

    TemperaturePhidget(
        const PhidgetOpener& opener,
        int serial);

    [[nodiscard]] int GetInputs() const;
    [[nodiscard]] void* GetHandle() const;

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

//
//
//

class PhidgetsSensor
    : public IPollableSensor
    , public AbstractSensor
{
public:
    ~PhidgetsSensor();
    PhidgetsSensor(const PhidgetsSensor&) = delete;
    PhidgetsSensor(PhidgetsSensor&&) = delete;
    PhidgetsSensor& operator=(const PhidgetsSensor&) = delete;
    PhidgetsSensor& operator=(PhidgetsSensor&&) = delete;

protected:
    PhidgetsSensor(
        std::shared_ptr<TemperaturePhidget> phidget,
        SensorId sensorId);

    [[nodiscard]]
    TemperaturePhidget& GetPhidget();

private:
    std::shared_ptr<TemperaturePhidget> m_phidget;
};

//
//
//

class PhidgetsProbeSensor final : public PhidgetsSensor
{
public:
    PhidgetsProbeSensor(
        std::shared_ptr<TemperaturePhidget> phidget,
        SensorId sensorId,
        int input);

    void PollAndNotify() override;

private:
    const int m_input;
};

//
//
//

class PhidgetsAmbientSensor final : public PhidgetsSensor {
public:
    PhidgetsAmbientSensor(
        std::shared_ptr<TemperaturePhidget> phidget,
        SensorId sensorId);

    void PollAndNotify() override;
};

