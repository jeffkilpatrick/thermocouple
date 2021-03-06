//
//  SensorBrokerTest.cpp
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/30/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#include "MockSensor.h"
#include "RecordingListener.h"
#include "SensorBroker.h"

#include "gtest/gtest.h"

class SensorBrokerTest : public testing::Test {
public:
    virtual void TearDown() {
        SensorBroker::Instance().Reset();
    }
};

TEST_F(SensorBrokerTest, Register) {
    auto& broker = SensorBroker::Instance();
    auto poller = std::make_unique<SensorPoller>();

    auto sensor1 = poller->CreateSensor<MockSensor>("mock1", MockSensor::ValuesVec{ 35.0 });
    auto sensor2 = poller->CreateSensor<MockSensor>("mock2", MockSensor::ValuesVec{ 35.0 });

    EXPECT_TRUE(broker.Register(sensor1));
    auto sensors = broker.AvailableSensors();
    EXPECT_EQ(1, sensors.size());

    EXPECT_TRUE(broker.Register(sensor2));
    sensors = broker.AvailableSensors();
    EXPECT_EQ(2, sensors.size());

    EXPECT_FALSE(broker.Register(sensor2));
    sensors = broker.AvailableSensors();
    EXPECT_EQ(2, sensors.size());
}

TEST_F(SensorBrokerTest, NotifyAvailableSensor) {
    const std::chrono::milliseconds interval(5);
    auto& broker = SensorBroker::Instance();
    auto poller = std::make_unique<SensorPoller>(interval);

    auto sensor = poller->CreateSensor<MockSensor>("mock", MockSensor::ValuesVec{ 35.0 });
    poller->Start();

    EXPECT_TRUE(broker.Register(sensor));

    auto listener = std::make_shared<RecordingListener>();
    broker.NotifyInterval(listener, "mock", interval);
    std::this_thread::sleep_for(interval * 2);
    EXPECT_LE(1, listener->GetValues().size());
}

TEST_F(SensorBrokerTest, NotifyUnavailableSensor) {
    const std::chrono::milliseconds interval(5);
    auto& broker = SensorBroker::Instance();
    auto poller = std::make_unique<SensorPoller>(interval);

    auto sensor = poller->CreateSensor<MockSensor>("mock", MockSensor::ValuesVec{ 35.0 });
    poller->Start();

    auto listener = std::make_shared<RecordingListener>();
    broker.NotifyOnChange(listener, "mock", 0.5);

    // No sensor --> no data
    std::this_thread::sleep_for(interval * 2);
    EXPECT_EQ(0, listener->GetValues().size());

    // Hook up the sensor
    EXPECT_TRUE(broker.Register(sensor));

    // Yay, we have data.
    std::this_thread::sleep_for(interval * 2);
    EXPECT_EQ(1, listener->GetValues().size());
}

TEST_F(SensorBrokerTest, Unsubscribe) {
    const std::chrono::milliseconds interval(5);
    auto& broker = SensorBroker::Instance();
    auto poller = std::make_unique<SensorPoller>(interval);

    auto sensor = poller->CreateSensor<MockSensor>("mock", MockSensor::ValuesVec{ 35.0 });
    broker.Register(sensor);
    poller->Start();

    auto listener = std::make_shared<RecordingListener>();
    auto subId = broker.NotifyOnChange(listener, "mock", 0.5);

    std::this_thread::sleep_for(interval * 2);
    EXPECT_EQ(1, listener->GetValues().size());

    EXPECT_TRUE(broker.Unsubscribe(subId));
    sensor->SetValues({ 36.0 });
    std::this_thread::sleep_for(interval * 2);

    EXPECT_EQ(1, listener->GetValues().size());
    EXPECT_FALSE(broker.Unsubscribe(subId));
}

TEST_F(SensorBrokerTest, Pause) {
    const std::chrono::milliseconds interval(5);
    auto& broker = SensorBroker::Instance();
    auto poller = std::make_unique<SensorPoller>(interval);

    auto sensor = poller->CreateSensor<MockSensor>("mock", MockSensor::ValuesVec{ 35.0 });
    broker.Register(sensor);
    poller->Start();

    auto listener = std::make_shared<RecordingListener>();
    auto subId = broker.NotifyOnChange(listener, "mock", 0.5);

    // Initial reading
    std::this_thread::sleep_for(interval * 2);
    EXPECT_EQ(1, listener->GetValues().size());
    EXPECT_EQ(Subscription::Status::ACTIVE, broker.GetStatus(subId));

    // Changed value
    EXPECT_TRUE(broker.Pause(subId));
    EXPECT_EQ(Subscription::Status::PAUSED, broker.GetStatus(subId));
    sensor->SetValues({ 36.0 });
    std::this_thread::sleep_for(interval * 2);
    EXPECT_EQ(1, listener->GetValues().size());
    EXPECT_FALSE(broker.Pause(subId)); // NOP == false

    // No longer paused
    EXPECT_TRUE(broker.Unpause(subId));
    EXPECT_EQ(Subscription::Status::ACTIVE, broker.GetStatus(subId));
    std::this_thread::sleep_for(interval * 2);
    EXPECT_EQ(2, listener->GetValues().size());
    EXPECT_FALSE(broker.Unpause(subId)); // NOP == false
}
