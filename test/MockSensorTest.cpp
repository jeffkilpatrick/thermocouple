//
//  MockSensorTest.cpp
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/30/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#include "MockSensor.h"
#include "RecordingListener.h"

#include "gtest/gtest.h"

TEST(MockSensorTest, Interval) {
    const auto pollInterval = std::chrono::milliseconds(10);
    const auto sleepTime = pollInterval * 5.9;
    const ISensor::SubscriptionId subId = 1;

    const auto startTime = std::chrono::system_clock::now();
    auto listener = std::make_shared<RecordingListener>();
    {
        auto poller = std::make_unique<SensorPoller>(pollInterval);
        auto s = poller->CreateSensor<MockSensor>("mock", MockSensor::ValuesVec{ 35.0 });
        s->AddNotification(subId, std::make_shared<IntervalSubscription>(listener, pollInterval));
        poller->Start();
        std::this_thread::sleep_for(sleepTime);
    }

    auto& values = listener->GetValues();

    EXPECT_LE(5, values.size());
    auto lastTime = startTime;
    for (auto value : values) {
        EXPECT_FLOAT_EQ(35.0, value.second);
        EXPECT_LT(lastTime, value.first);
        lastTime = value.first;
    }
}

TEST(MockSensorTest, MultiValue) {
    const auto pollInterval = std::chrono::milliseconds(10);
    const auto sleepTime = pollInterval * 5.9;
    const ISensor::SubscriptionId subId = 1;

    const auto startTime = std::chrono::system_clock::now();
    auto listener = std::make_shared<RecordingListener>();
    {
        auto poller = std::make_unique<SensorPoller>(pollInterval);
        auto s = poller->CreateSensor<MockSensor>("mock", MockSensor::ValuesVec{ 1, 2, 3 });
        s->AddNotification(subId, std::make_shared<IntervalSubscription>(listener, pollInterval));
        poller->Start();
        std::this_thread::sleep_for(sleepTime);
    }


    MockSensor::ValuesVec expectedValues{ 1, 2, 3, 1, 2 };
    auto& actualValues = listener->GetValues();

    EXPECT_LE(5, actualValues.size());
    auto lastTime = startTime;
    for (size_t idx = 0; idx < std::min(size_t{5}, actualValues.size()); ++idx) {
        EXPECT_FLOAT_EQ(expectedValues[idx], actualValues[idx].second);
        EXPECT_LT(lastTime, actualValues[idx].first);
        lastTime = actualValues[idx].first;
    }

}

TEST(MockSensorTest, Delta) {
    const auto pollInterval = std::chrono::milliseconds(5);
    const ISensor::SubscriptionId subId = 1;

    auto listener = std::make_shared<RecordingListener>();
    {
        auto poller = std::make_unique<SensorPoller>(pollInterval);
        auto s = poller->CreateSensor<MockSensor>("mock", MockSensor::ValuesVec{ 35.0 });
        s->AddNotification(subId, std::make_shared<OnChangeSubscription>(listener, 0.5));
        poller->Start();

        std::this_thread::sleep_for(pollInterval * 2);
        s->SetValues({ 36.0 });
        std::this_thread::sleep_for(pollInterval * 2);
    }

    auto& values = listener->GetValues();
    EXPECT_EQ(2, values.size());
}

TEST(MockSensorTest, Pause) {
    const auto pollInterval = std::chrono::milliseconds(5);
    const ISensor::SubscriptionId subId = 1;

    auto listener = std::make_shared<RecordingListener>();
    auto& values = listener->GetValues();
    {
        auto poller = std::make_unique<SensorPoller>(pollInterval);
        auto s = poller->CreateSensor<MockSensor>("mock", MockSensor::ValuesVec{ 35.0 });
        s->AddNotification(subId, std::make_shared<OnChangeSubscription>(listener, 0.5));
        poller->Start();

        std::this_thread::sleep_for(pollInterval * 2);
        EXPECT_EQ(1, values.size());

        s->SetValues({ 36.0 });
        std::this_thread::sleep_for(pollInterval * 2);
        EXPECT_EQ(2, values.size());

        s->Pause(1);
        EXPECT_EQ(Subscription::Status::PAUSED, s->GetStatus(1));

        s->SetValues({ 37.0 });
        std::this_thread::sleep_for(pollInterval * 2);
        EXPECT_EQ(2, values.size());

        s->Unpause(1);
        EXPECT_EQ(Subscription::Status::ACTIVE, s->GetStatus(1));

        std::this_thread::sleep_for(pollInterval * 2);
        EXPECT_EQ(3, values.size());
    }
}

TEST(MockSensorTest, DeletedListener) {
    const auto pollInterval = std::chrono::milliseconds(5);
    auto listener1 = std::make_shared<RecordingListener>();
    auto listener2 = std::make_shared<RecordingListener>();

    auto poller = std::make_unique<SensorPoller>(pollInterval);
    auto s = poller->CreateSensor<MockSensor>("mock", MockSensor::ValuesVec{ 35.0 });
    s->AddNotification(1, std::make_shared<IntervalSubscription>(listener1, pollInterval));
    s->AddNotification(2, std::make_shared<IntervalSubscription>(listener2, pollInterval));
    poller->Start();

    EXPECT_EQ(Subscription::Status::ACTIVE, s->GetStatus(1));
    EXPECT_EQ(Subscription::Status::ACTIVE, s->GetStatus(2));

    std::this_thread::sleep_for(pollInterval * 2);
    listener1 = nullptr;
    std::this_thread::sleep_for(pollInterval * 2);

    EXPECT_EQ(Subscription::Status::UNKNOWN_SUBSCRIPTION, s->GetStatus(1));
    EXPECT_EQ(Subscription::Status::ACTIVE, s->GetStatus(2));
}
