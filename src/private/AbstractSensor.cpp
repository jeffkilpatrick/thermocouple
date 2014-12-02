//
//  AbstractSensor.cpp
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/29/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#include "AbstractSensor.h"

AbstractSensor::AbstractSensor(const ISensor::SensorId& sensorId)
    : m_identifier(sensorId)
{ }

const ISensor::SensorId&
AbstractSensor::Identifier() const
{
    return m_identifier;
}

