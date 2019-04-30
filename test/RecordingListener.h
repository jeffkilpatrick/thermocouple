//
//  RecordingListener.h
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/30/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#ifndef Thermocouple_RecordingListener_h
#define Thermocouple_RecordingListener_h

#include "IListener.h"

#include <chrono>
#include <memory>
#include <vector>

class RecordingListener final : public IListener {
public:
    using Value = std::pair<std::chrono::system_clock::time_point, float>;

    RecordingListener() = default;
    void Notify(float celsius) override;

    const std::vector<Value>& GetValues() const;

private:
    std::vector<Value> m_values;

public:
    RecordingListener(const RecordingListener&) = delete;
    RecordingListener& operator=(const RecordingListener&) = delete;
};

#endif
