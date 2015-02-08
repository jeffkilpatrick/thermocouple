//
//  PhidgetsSensor.cpp
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 12/1/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#include "PhidgetsSensor.h"

#ifdef PHIGET21_FRAMEWORK
#include "Phidget21/phidget21.h"
#else
#include "phidget21.h"
#endif

namespace {
const char* GetPhidgetErrorString(int errorCode)
{
    const char* errorString;
    CPhidget_getErrorDescription(errorCode, &errorString);

    return errorString;
}
}

PhidgetException::PhidgetException(int errorCode)
    : std::runtime_error( GetPhidgetErrorString(errorCode) )
{}

class TemperaturePhidget::Impl {
public:
    Impl(
        const PhidgetOpener& opener,
         int serial);

    ~Impl();

    int GetInputs() const;
    void* GetHandle() const;
private:
    CPhidgetTemperatureSensorHandle m_handle;
    int m_inputs;
};

TemperaturePhidget::Impl::Impl(
    const PhidgetOpener& opener,
    int serial)

    : m_handle(nullptr)
    , m_inputs(-1)
{
    CPhidgetTemperatureSensor_create(&m_handle);

    opener.OpenPhidget(m_handle, serial);

    int result;

    if ((result = CPhidget_waitForAttachment(reinterpret_cast<CPhidgetHandle>(m_handle), 5000))) {
        throw PhidgetException(result);
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

TemperaturePhidget::TemperaturePhidget(
    const PhidgetOpener& opener,
    int serial)

    : m_impl(new Impl(opener, serial))
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
    SensorId sensorId)

    : AbstractSensor(std::move(sensorId))
    , m_phidget(phidget)
{ }

PhidgetsSensor::~PhidgetsSensor()
{ }

//
//
//

PhidgetsProbeSensor::PhidgetsProbeSensor(
    std::shared_ptr<TemperaturePhidget> phidget,
    SensorId sensorId,
    int input)

    : PhidgetsSensor(phidget, std::move(sensorId))
    , m_input(input)
{ }

void
PhidgetsProbeSensor::PollAndNotify()
{
    auto handle = reinterpret_cast<CPhidgetTemperatureSensorHandle>(m_phidget->GetHandle());
    double temperature = 0.0;
    if (CPhidgetTemperatureSensor_getTemperature(handle, m_input, &temperature)) {
        return;
    }

    Notify(temperature);
}

//
//
//

PhidgetsAmbientSensor::PhidgetsAmbientSensor(
    std::shared_ptr<TemperaturePhidget> phidget,
    SensorId sensorId)

    : PhidgetsSensor(phidget, std::move(sensorId))
{ }

void
PhidgetsAmbientSensor::PollAndNotify()
{
    auto handle = reinterpret_cast<CPhidgetTemperatureSensorHandle>(m_phidget->GetHandle());
    double ambient = 0.0;
    if (CPhidgetTemperatureSensor_getAmbientTemperature(handle, &ambient)) {
        return;
    }

    Notify(ambient);
}

