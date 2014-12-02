//
//  RecordingListener.cpp
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/30/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#include "RecordingListener.h"

void
RecordingListener::Notify(float celsius)
{
    m_values.emplace_back(std::chrono::system_clock::now(), celsius);
}

const std::vector<RecordingListener::Value>&
RecordingListener::GetValues() const
{
    return m_values;
}