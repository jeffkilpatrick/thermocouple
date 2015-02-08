//
//  PhidgetsManager.cpp
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/30/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#include "PhidgetsManager.h"

#include "PhidgetsSensor.h"
#include "SensorBroker.h"

#ifdef PHIGET21_FRAMEWORK
#include "Phidget21/phidget21.h"
#else
#include "phidget21.h"
#endif

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <sstream>
#include <thread>

class PhidgetsManager::Impl : public PhidgetOpener {
public:
    Impl();
    ~Impl();

    void Open();
    void Close();

    int OnAttach(CPhidgetHandle h);
    int OnDetach(CPhidgetHandle h);

protected:
    virtual void DoOpen() = 0;
    CPhidgetManagerHandle m_handle;

private:
    static bool IsTempSensor(CPhidgetHandle h);
    void RegisterSensor(int serial);
    void UnregisterSensor(int serial);
    void RegistrationLoop();

    std::thread m_registrationThread;
    std::queue<int> m_registerQueue;
    std::queue<int> m_unregisterQueue;
    std::atomic<bool> m_exitRegistration;
    std::unordered_map<int, std::shared_ptr<TemperaturePhidget>> m_phidgets;
    std::unique_ptr<SensorPoller> m_poller;

    mutable std::mutex m_registrationMutex;
    std::condition_variable m_registrationCv;
};

PhidgetsManager::Impl::Impl()
    : m_handle(nullptr)
    , m_registrationThread([this] () { RegistrationLoop(); })
    , m_poller(std::make_unique<SensorPoller>())
{ }

PhidgetsManager::Impl::~Impl()
{
    try {
        m_exitRegistration = true;
        m_registrationCv.notify_one();
        m_registrationThread.join();
        Close();
    }
    catch (...) { }
}

namespace {

int
AttachHandler(CPhidgetHandle h, void* anImpl)
{
    auto impl = reinterpret_cast<PhidgetsManager::Impl*>(anImpl);
    return impl->OnAttach(h);
}

int
DetachHandler(CPhidgetHandle h, void* anImpl)
{
    PhidgetsManager::Impl* impl = reinterpret_cast<PhidgetsManager::Impl*>(anImpl);
    return impl->OnDetach(h);
}

}

void
PhidgetsManager::Impl::Open()
{
    CPhidgetManager_create(&m_handle);
    CPhidgetManager_set_OnAttach_Handler(m_handle, AttachHandler, this);
    CPhidgetManager_set_OnDetach_Handler(m_handle, DetachHandler, this);
    DoOpen();
}

void
PhidgetsManager::Impl::Close()
{
    if (!m_handle) {
        return;
    }

    while (!m_phidgets.empty()) {
        UnregisterSensor(m_phidgets.begin()->first);
    }

    CPhidgetManager_close(m_handle);
    CPhidgetManager_delete(m_handle);
    m_handle = nullptr;
}

int
PhidgetsManager::Impl::OnAttach(CPhidgetHandle phid)
{
    if (!IsTempSensor(phid)) {
        return 0;
    }

	int serialNo;
	CPhidget_getSerialNumber(phid, &serialNo);

    {
        std::lock_guard<std::mutex> lock(m_registrationMutex);
        m_registerQueue.push(serialNo);
    }
    m_registrationCv.notify_one();

    return 0;
}

int
PhidgetsManager::Impl::OnDetach(CPhidgetHandle phid)
{
    if (!IsTempSensor(phid)) {
        return 0;
    }

	int serialNo;
	CPhidget_getSerialNumber(phid, &serialNo);

    {
        std::lock_guard<std::mutex> lock(m_registrationMutex);
        m_unregisterQueue.push(serialNo);
    }
    m_registrationCv.notify_one();

    return 0;
}

/*static*/ bool
PhidgetsManager::Impl::IsTempSensor(CPhidgetHandle h)
{
	CPhidget_DeviceClass cls;
	CPhidget_getDeviceClass(h, &cls);
    return cls == PHIDCLASS_TEMPERATURESENSOR;
}

namespace {

ISensor::SensorId
AmbientSensorId(int serial)
{
    std::ostringstream str;
    str << serial << ":amb";
    return str.str();
}

ISensor::SensorId
ProbeSensorId(int serial, int input)
{
    std::ostringstream str;
    str << serial << ":" << input;
    return str.str();
}

}

void
PhidgetsManager::Impl::RegisterSensor(int serial)
{
    try {
        auto phidget = std::make_shared<TemperaturePhidget>(*this, serial);
        m_phidgets[serial] = phidget;
        auto ambient = m_poller->CreateSensor<PhidgetsAmbientSensor>(phidget, AmbientSensorId(serial));
        SensorBroker::Instance().Register(ambient);

        for (int i = 0; i < phidget->GetInputs(); ++i) {
            auto probe = m_poller->CreateSensor<PhidgetsProbeSensor>(phidget, ProbeSensorId(serial, i), i);
            SensorBroker::Instance().Register(probe);
        }
    }
    catch ( PhidgetException& e ) {
        // TODO-jrk: we need a logger
        std::cerr << "Failed to register sensor " << serial << ": " << e.what() << std::endl;
    }
}

void
PhidgetsManager::Impl::UnregisterSensor(int serial)
{
    auto phidget = m_phidgets[serial];
    m_phidgets.erase(serial);

    auto broker = SensorBroker::GetInstance();
    if (!broker) {
        return;
    }

    broker->Unregister(AmbientSensorId(serial));
    for (int i = 0; i < phidget->GetInputs(); ++i) {
        broker->Unregister(ProbeSensorId(serial, i));
    }
}

void
PhidgetsManager::Impl::RegistrationLoop()
{
    while (!m_exitRegistration) {
        std::unique_lock<std::mutex> lock(m_registrationMutex);
        m_registrationCv.wait(lock);

        while (!m_registerQueue.empty()) {
            auto serial = m_registerQueue.front();
            m_registerQueue.pop();
            RegisterSensor(serial);
        }

        while (!m_unregisterQueue.empty()) {
            auto serial = m_unregisterQueue.front();
            m_unregisterQueue.pop();
            UnregisterSensor(serial);
        }

        lock.unlock();
    }
}

//
//
//

namespace {
    std::unique_ptr<LocalPhidgetsManager> s_phidgetsManager;
}

/*static*/ LocalPhidgetsManager&
LocalPhidgetsManager::Instance()
{
    if (!s_phidgetsManager) { // TODO-jrk: InitOnce()
        s_phidgetsManager.reset(new LocalPhidgetsManager());
    }

    return *s_phidgetsManager;
}

//
//
//

class LocalPhidgetsManager::Impl : public PhidgetsManager::Impl {
public:
    Impl();
    void OpenPhidget(void* handle, int serial) const override;

protected:
    void DoOpen() override;
};

LocalPhidgetsManager::Impl::Impl()
{
    Open();
}

void
LocalPhidgetsManager::Impl::OpenPhidget(void* handle, int serial) const
{
    int result;

    if ((result = CPhidget_open(reinterpret_cast<CPhidgetHandle>(handle), serial))) {
        throw PhidgetException(result);
    }
}

void
LocalPhidgetsManager::Impl::DoOpen()
{
    CPhidgetManager_open(m_handle);
}

LocalPhidgetsManager::LocalPhidgetsManager()
    : m_impl(new Impl())
{ }

//
//
//

class RemotePhidgetsManager::Impl : public PhidgetsManager::Impl {
public:
    Impl(const char *mdnsService, const char* password);
    Impl(const char *address, int port, const char* password);
    void OpenPhidget(void* handle, int serial) const override;

protected:
    void DoOpen() override;
private:
    const char* m_mdnsService;
    const char* m_address;
    const char* m_password;
    const int m_port;
};

RemotePhidgetsManager::Impl::Impl(const char *mdnsService, const char* password)
    : m_mdnsService(mdnsService)
    , m_address(nullptr)
    , m_password(password)
    , m_port(0)
{
    Open();
}

RemotePhidgetsManager::Impl::Impl(const char *address, int port, const char* password)
    : m_mdnsService(nullptr)
    , m_address(address)
    , m_password(password)
    , m_port(port)
{
    Open();
}

void
RemotePhidgetsManager::Impl::OpenPhidget(void* handle, int serial) const
{
    int result;

    if (m_mdnsService) {
        result = CPhidget_openRemote(reinterpret_cast<CPhidgetHandle>(handle), serial, m_mdnsService, m_password);
    }
    else {
        result = CPhidget_openRemoteIP(reinterpret_cast<CPhidgetHandle>(handle), serial, m_address, m_port, m_password);
    }

    if (result) {
        throw PhidgetException(result);
    }
}

void
RemotePhidgetsManager::Impl::DoOpen()
{
    int result;
    if (m_mdnsService) {
        result = CPhidgetManager_openRemote(m_handle, m_mdnsService, m_password);
    }
    else {
        result = CPhidgetManager_openRemoteIP(m_handle, m_address, m_port, m_password);
    }

    if (result) {
        const char* errorDesc;
        CPhidget_getErrorDescription(result, &errorDesc);
        throw std::runtime_error(errorDesc);
    }
}

RemotePhidgetsManager::RemotePhidgetsManager(const char *mdnsService, const char* password)
    : m_impl(new Impl(mdnsService, password))
{ }

RemotePhidgetsManager::RemotePhidgetsManager(const char *address, int port, const char* password)
    : m_impl(new Impl(address, port, password))
{ }

RemotePhidgetsManager::~RemotePhidgetsManager()
{ }

/*static*/ std::unique_ptr<RemotePhidgetsManager>
RemotePhidgetsManager::OpenMdns(const char *mdnsName, const char* password)
{
    try {
        return std::unique_ptr<RemotePhidgetsManager>(new RemotePhidgetsManager(mdnsName, password));
    }
    catch (const std::runtime_error&) {
        return nullptr;
    }
}

/*static*/ std::unique_ptr<RemotePhidgetsManager>
RemotePhidgetsManager::OpenAddress(const char *address, int port, const char* password)
{
    try {
        return std::unique_ptr<RemotePhidgetsManager>(new RemotePhidgetsManager(address, port, password));
    }
    catch (const std::runtime_error&) {
        return nullptr;
    }
}
