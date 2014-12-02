//
//  PhidgetsManager.h
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/30/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#ifndef Thermocouple_PhidgetsManager_h
#define Thermocouple_PhidgetsManager_h

#pragma GCC visibility push(default)

#include <memory>

class PhidgetsManager {
public:
    class Impl;
};

class LocalPhidgetsManager : public PhidgetsManager {
    class Impl;
public:
    static LocalPhidgetsManager& Instance();

private:
    LocalPhidgetsManager();
    std::unique_ptr<Impl> m_impl;
};

#pragma GCC visibility pop

#endif
