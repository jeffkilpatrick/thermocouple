//
//  PhidgetsSensor.cpp
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 12/1/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#include "PhidgetsSensor.h"

#include "phidget21.h"

class TemperaturePhidget::Impl {
public:
    Impl(int serial);
    ~Impl();

    int GetInputs() const;
    void* GetHandle() const;
private:
    CPhidgetTemperatureSensorHandle m_handle;
    int m_inputs;
};

TemperaturePhidget::Impl::Impl(int serial)
    : m_handle(nullptr)
    , m_inputs(-1)
{
    CPhidgetTemperatureSensor_create(&m_handle);

    if (CPhidget_open(reinterpret_cast<CPhidgetHandle>(m_handle), serial)) {
        return; // TODO-jrk: report this
    }

    if (CPhidget_waitForAttachment(reinterpret_cast<CPhidgetHandle>(m_handle), 1000)) {
        return; // TODO-jrk: report this
    }

    CPhidgetTemperatureSensor_getTemperatureInputCount(m_handle, &m_inputs);
}

TemperaturePhidget::Impl::~Impl()
{
    CPhidget_close(reinterpret_cast<CPhidgetHandle>(m_handle));
    CPhidget_delete(reinterpret_cast<CPhidgetHandle>(m_handle));
}

int
TemperaturePhidget::Impl::GetInputs() const
{
    return m_inputs;
}

void*
TemperaturePhidget::Impl::GetHandle() const
{
    return m_handle;
}

//
//
//

TemperaturePhidget::TemperaturePhidget(int serial)
    : m_impl(new Impl(serial))
{ }

TemperaturePhidget::~TemperaturePhidget()
{}

int
TemperaturePhidget::GetInputs() const
{
    return m_impl->GetInputs();
}

void*
TemperaturePhidget::GetHandle() const
{
    return m_impl->GetHandle();
}

//
//
//

PhidgetsSensor::PhidgetsSensor(
    std::shared_ptr<TemperaturePhidget> phidget,
    const SensorId& sensorId,
    std::chrono::milliseconds interval)

    : PollingSensor(sensorId, interval)
    , m_phidget(phidget)
{ }

PhidgetsProbeSensor::PhidgetsProbeSensor(
    std::shared_ptr<TemperaturePhidget> phidget,
    const SensorId& sensorId,
    int input,
    std::chrono::milliseconds interval)

    : PhidgetsSensor(phidget, sensorId, interval)
    , m_input(input)
{
    Start();
}

PhidgetsProbeSensor::~PhidgetsProbeSensor()
{
    Stop();
}

bool
PhidgetsProbeSensor::Poll(float& value) const
{
    auto handle = reinterpret_cast<CPhidgetTemperatureSensorHandle>(m_phidget->GetHandle());
    double temperature = 0.0;
    if (CPhidgetTemperatureSensor_getTemperature(handle, m_input, &temperature)) {
        return false;
    }

    value = temperature;
    return true;
}

PhidgetsAmbientSensor::PhidgetsAmbientSensor(
    std::shared_ptr<TemperaturePhidget> phidget,
    const SensorId& sensorId,
    std::chrono::milliseconds interval)

    : PhidgetsSensor(phidget, sensorId, interval)
{
    Start();
}

PhidgetsAmbientSensor::~PhidgetsAmbientSensor()
{
    Stop();
}

bool
PhidgetsAmbientSensor::Poll(float& value) const
{
    auto handle = reinterpret_cast<CPhidgetTemperatureSensorHandle>(m_phidget->GetHandle());
    double ambient = 0.0;
    if (CPhidgetTemperatureSensor_getAmbientTemperature(handle, &ambient)) {
        return false;
    }

    value = ambient;
    return true;
}

