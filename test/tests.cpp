// Copyright 2024 Kachalov Mikhail

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <thread> // NOLINT [build/c++11]
#include <chrono> // NOLINT [build/c++11]
#include "TimedDoor.h"

class TimedDoorTest : public ::testing::Test {
protected:
    MockTimedDoor* timedDoor;

    void SetUp() override {
        timedDoor = new MockTimedDoor(2);
    }

    void TearDown() override {
        delete timedDoor;
    }
};

TEST_F(TimedDoorTest, DoorTimeoutAfterUnlock) {
    timedDoor->unlock();
    std::this_thread::sleep_for(std::chrono::seconds(timedDoor->getTimeOut() + 1));
    EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorTimeoutAfterLock) {
    timedDoor->lock();
    std::this_thread::sleep_for(std::chrono::seconds(timedDoor->getTimeOut() + 1));
    EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorTimeoutAfterUnlockAndLock) {
    timedDoor->unlock();
    std::this_thread::sleep_for(std::chrono::seconds(timedDoor->getTimeOut() - 1));
    timedDoor->lock();
    std::this_thread::sleep_for(std::chrono::seconds(timedDoor->getTimeOut() + 1));
    EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorTimeoutAfterMultipleUnlocks) {
    for (int i = 0; i < 3; ++i) {
        timedDoor->unlock();
        std::this_thread::sleep_for(std::chrono::seconds(timedDoor->getTimeOut() + 1));
    }
    EXPECT_TRUE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorCanBeOpenedAfterUnlock) {
    timedDoor->unlock();
    EXPECT_TRUE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorCanBeClosedAfterLock) {
    timedDoor->lock();
    EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorCanBeOpenedAfterMultipleUnlocks) {
    for (int i = 0; i < 2; ++i) {
        timedDoor->unlock();
    }
    EXPECT_TRUE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorCanBeClosedAfterMultipleLocks) {
    timedDoor->unlock();
    for (int i = 0; i < 2; ++i) {
        timedDoor->lock();
    }
    EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorCanBeOpenedAndClosed) {
    timedDoor->unlock();
    EXPECT_TRUE(timedDoor->isDoorOpened());
    timedDoor->lock();
    EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, GetTimeoutReturnsCorrectValue) {
    int expectedTimeout = 2;
    EXPECT_EQ(timedDoor->getTimeOut(), expectedTimeout);
}
