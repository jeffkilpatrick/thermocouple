//
//  IListener.h
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/22/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#pragma once

#pragma GCC visibility push(default)

class IListener {
public:
    virtual void Notify(float celsius) = 0;

    IListener() = default;
    IListener(const IListener&) = delete;
    IListener& operator=(const IListener&) = delete;
};

#pragma GCC visibility pop
