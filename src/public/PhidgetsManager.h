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
public:
    static LocalPhidgetsManager& Instance();

private:
    LocalPhidgetsManager();

    class Impl;
    std::unique_ptr<Impl> m_impl;
};

class RemotePhidgetsManager : public PhidgetsManager {
public:

    // Open a Phidgets web service via mDNS.
    // @param mdnsName The service name; may be NULL to open any.
    // @param password The service password; may be NULL if server is running unsecured.
    // @returns nullptr on error.
    static std::shared_ptr<RemotePhidgetsManager> OpenMdns(
        const char* mdnsName,
        const char* password = nullptr);

    // Open a Phidgets web service via hostname or IP.
    // @param mdnsName The host; may not be NULL;
    // @param int The service port; default is 5001.
    // @param password The service password; may be NULL if server is running unsecured.
    // @returns nullptr on error.
    static std::shared_ptr<RemotePhidgetsManager> OpenAddress(
        const char* address,
        int port = 5001,
        const char* password = nullptr);

private:
    RemotePhidgetsManager(const char* mdnsName, const char* password);
    RemotePhidgetsManager(const char* address, int port, const char* password);

    class Impl;
    std::unique_ptr<Impl> m_impl;
};

#pragma GCC visibility pop

#endif
