//
//  AbstractSensor.h
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/29/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#ifndef Thermocouple_AbstractSensor_h
#define Thermocouple_AbstractSensor_h

#include "ISensor.h"

class AbstractSensor : public ISensor {

protected:
    AbstractSensor(const SensorId& sensorId);

public:
    const SensorId& Identifier() const override;

private:
    const SensorId m_identifier;
};

#endif
