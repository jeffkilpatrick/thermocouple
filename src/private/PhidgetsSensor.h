//
//  PhidgetsSensor.h
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/30/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#ifndef Thermocouple_PhidgetsSensor_h
#define Thermocouple_PhidgetsSensor_h

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
    class Impl;

public:
    TemperaturePhidget(
        const PhidgetOpener& opener,
        int serial);

    ~TemperaturePhidget();

    int GetInputs() const;
    void* GetHandle() const;

private:
    std::unique_ptr<Impl> m_impl;

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
        const SensorId& sensorId,
        std::shared_ptr<SensorPoller> poller);

    std::shared_ptr<TemperaturePhidget> m_phidget;
    std::shared_ptr<SensorPoller>       m_poller;
};

//
//
//

class PhidgetsProbeSensor : public PhidgetsSensor
{
public:
    PhidgetsProbeSensor(
        std::shared_ptr<TemperaturePhidget> phidget,
        const SensorId& sensorId,
        int input,
        std::shared_ptr<SensorPoller> poller);

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
        const SensorId& sensorId,
        std::shared_ptr<SensorPoller> poller);

    void PollAndNotify() override;
};

#endif
