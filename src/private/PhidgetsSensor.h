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
    PhidgetException(int errorCode);
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
    TemperaturePhidget(
        const PhidgetOpener& opener,
        int serial);

    ~TemperaturePhidget();

    int GetInputs() const;
    void* GetHandle() const;

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;

public:
    TemperaturePhidget(const TemperaturePhidget&) = delete;
    TemperaturePhidget& operator=(const TemperaturePhidget&) = delete;
};

//
//
//

class PhidgetsSensor
    : public IPollableSensor
    , public AbstractSensor
{
protected:
    PhidgetsSensor(
        std::shared_ptr<TemperaturePhidget> phidget,
        SensorId sensorId);

    ~PhidgetsSensor();

    std::shared_ptr<TemperaturePhidget> m_phidget;
};

//
//
//

class PhidgetsProbeSensor : public PhidgetsSensor
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

class PhidgetsAmbientSensor : public PhidgetsSensor {
public:
    PhidgetsAmbientSensor(
        std::shared_ptr<TemperaturePhidget> phidget,
        SensorId sensorId);

    void PollAndNotify() override;
};

