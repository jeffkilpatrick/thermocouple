//
//  main.cpp
//  PhidgetsMonitor
//
//  Created by Jeff Kilpatrick on 11/30/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#include "IListener.h"
#include "PhidgetsManager.h"
#include "SensorBroker.h"
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

using std::cout;
using std::endl;
using std::flush;

static std::mutex s_printingMutex;

class PrintingListener final : public IListener {
public:
    PrintingListener(const ISensor::SensorId& sensorId)
        : m_sensorId(sensorId)
    { }

    void Notify(float celsius) override {
        std::lock_guard<std::mutex> lock(s_printingMutex);
        cout << m_sensorId << ": " << ((9.0/5.0) * celsius) + 32.0 << endl;
    };
private:
    const ISensor::SensorId m_sensorId;

public:
    PrintingListener() = delete;
    PrintingListener(const PrintingListener&) = delete;
    PrintingListener& operator=(const PrintingListener&) = delete;
};

int main(int argc, const char* argv[])
{
    std::shared_ptr<RemotePhidgetsManager> remotePhidgets;

    if (argc > 1) {
        remotePhidgets = RemotePhidgetsManager::OpenAddress(argv[1]);
    }
    else {
        LocalPhidgetsManager::Instance();
    }

    auto& broker = SensorBroker::Instance();

    const std::chrono::seconds interval(1);
    std::vector<std::shared_ptr<IListener>> listeners;

    cout << "Waiting for sensors..." << flush;
    auto sensors = broker.AvailableSensors();
    while (sensors.empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        cout << '.' << flush;
        sensors = broker.AvailableSensors();
    }
    cout << endl;

    cout << "Sensors:\n";
    for (const auto& sid : sensors) {
        cout << "\t" << sid << endl;

        auto listener = std::make_shared<PrintingListener>(sid);
        broker.NotifyInterval(listener, sid, interval);
        listeners.push_back(listener);
    }

    getchar();

    return 0;
}
