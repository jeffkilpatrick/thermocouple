
#ifndef PHIDGETS_HOST
#define PHIDGETS_HOST "localhost"
#endif

#include <PhidgetsManager.h>
#include <SensorBroker.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <iostream>
#include <map>
#include <cmath>
#include <ncurses.h>
#include <thread>
#include <vector>

struct SensorInfo {
    std::string Name;
    int Row;
    std::atomic<float> Value;

    SensorInfo(std::string name, int row)
        : Name(name)
        , Row(row)
    {
        Value = std::nanf("");
    }

    SensorInfo(const SensorInfo& other)
        : Name(other.Name)
        , Row(other.Row)
    {
        Value = (float)other.Value;
    }
};

std::map<ISensor::SensorId, SensorInfo> Sensors = {
    { "167056:0",   SensorInfo("BT",  0) },
    { "167056:1",   SensorInfo("ET",  1) },
    { "167056:2",   SensorInfo("MET", 2) },
    { "167056:amb", SensorInfo("AMB", 3) }
};

class UpdateListener final : public IListener {
public:
    UpdateListener( SensorInfo& info ) : m_info(info) { }
    void Notify(float celsius) override { m_info.Value = ((9.0/5.0) * celsius) + 32.0; }

private:
    UpdateListener(const UpdateListener&) = delete;
    UpdateListener& operator=(const UpdateListener&) = delete;
    SensorInfo& m_info;
};

void AttachRunLoop()
{
    auto& broker = SensorBroker::Instance();
    std::vector<std::shared_ptr<IListener>> listeners;

    std::set<ISensor::SensorId> knownSensors;
    while (true) {
        auto currentSensors = broker.AvailableSensors();
        decltype(currentSensors) newSensors;

        std::set_difference(
            currentSensors.begin(), currentSensors.end(),
            knownSensors.begin(), knownSensors.end(),
            std::inserter(newSensors, newSensors.begin()) );

        for ( const auto& n : newSensors ) {
            knownSensors.insert(n);

            auto s = Sensors.find(n);
            if (s != Sensors.end()) {
                auto listener = std::make_shared<UpdateListener>( s->second );
                broker.NotifyInterval( listener, n, std::chrono::seconds(1) );
                listeners.push_back( listener );
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

class Outputter {
public:
    virtual void Flush() const { };
    virtual void OutputHeader() const { };
    virtual void OutputReading( const SensorInfo& s) const { };
};

class StdoutOutputter : public Outputter{
public:
    void Flush() const override {
        printf("\n");
        fflush(stdout);
    }
    void OutputReading( const SensorInfo& s ) const override {
        float value = s.Value;
        if ( !std::isnan( value ) ) {
            printf("%-10s%g\n", s.Name.substr(0, 9).c_str(), value );
        }
        else {
            printf("%-10sNAN\n", s.Name.substr(0, 9).c_str() );
        }
    }
};

class CursesOutputter : public Outputter {
public:
    CursesOutputter() {
        initscr();
    }
    ~CursesOutputter() {
        endwin();
    }

    void Flush() const override {
        refresh();
    }

    void OutputHeader() const override {
        for (const auto& s : Sensors ) {
            mvprintw( s.second.Row,  0, "%s", s.second.Name.substr(0, 9).c_str() );
        }
    }

    void OutputReading( const SensorInfo& s ) const override {
        float value = s.Value;
        if ( !std::isnan(value) ) {
            mvprintw( s.Row, 10, "%g", value );
        }
    }
};

int main(int argc, char* argv[])
{
    const char* host = argc > 1 ? argv[1] : PHIDGETS_HOST;
    std::shared_ptr<RemotePhidgetsManager> phidgetsManager = RemotePhidgetsManager::OpenAddress(host);

    auto attachThread = std::thread(AttachRunLoop);
    //StdoutOutputter outputter;
    CursesOutputter outputter;

    outputter.OutputHeader();
    while (true) {
    for (const auto& s : Sensors ) {
        outputter.OutputReading( s.second );
    }
    outputter.Flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    attachThread.join();

    return 0;
}
