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

#include "Phidget21/phidget21.h"

#include <atomic>
#include <mutex>
#include <queue>
#include <sstream>
#include <thread>

class PhidgetsManager::Impl {
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

    mutable std::mutex m_registrationMutex;
    std::condition_variable m_registrationCv;
};

PhidgetsManager::Impl::Impl()
    : m_handle(nullptr)
    , m_registrationThread([this] () { RegistrationLoop(); })
{ }

PhidgetsManager::Impl::~Impl()
{
    Close();
    m_exitRegistration = true;
    m_registrationThread.join();
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
    auto phidget = std::make_shared<TemperaturePhidget>(serial);
    m_phidgets[serial] = phidget;
    auto ambient = std::make_shared<PhidgetsAmbientSensor>(phidget, AmbientSensorId(serial));
    SensorBroker::Instance().Register(ambient);

    for (int i = 0; i < phidget->GetInputs(); ++i) {
        auto probe = std::make_shared<PhidgetsProbeSensor>(phidget, ProbeSensorId(serial, i), i);
        SensorBroker::Instance().Register(probe);
    }
}

void
PhidgetsManager::Impl::UnregisterSensor(int serial)
{
    auto phidget = m_phidgets[serial];
    m_phidgets.erase(serial);
    SensorBroker::Instance().Unregister(AmbientSensorId(serial));
    for (int i = 0; i < phidget->GetInputs(); ++i) {
        SensorBroker::Instance().Unregister(ProbeSensorId(serial, i));
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
    LocalPhidgetsManager* s_phidgetsManager = nullptr;
}

/*static*/ LocalPhidgetsManager&
LocalPhidgetsManager::Instance()
{
    if (!s_phidgetsManager) { // TODO-jrk: InitOnce()
        s_phidgetsManager = new LocalPhidgetsManager();
    }

    return *s_phidgetsManager;
}

//
//
//

class LocalPhidgetsManager::Impl : public PhidgetsManager::Impl {
public:
    Impl();
protected:
    void DoOpen() override;
};

LocalPhidgetsManager::Impl::Impl()
{
    Open();
}

void
LocalPhidgetsManager::Impl::DoOpen()
{
    CPhidgetManager_open(m_handle);
}

LocalPhidgetsManager::LocalPhidgetsManager()
    : m_impl(new Impl())
{ }

