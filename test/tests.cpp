// Copyright 2024 Kachalov Mikhail

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <thread> // NOLINT [build/c++11]
#include <chrono> // NOLINT [build/c++11]
#include "TimedDoor.h"

class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class MockDoor : public Door {
 public:
    MOCK_METHOD(void, lock, (), (override));
    MOCK_METHOD(void, unlock, (), (override));
    MOCK_METHOD(bool, isDoorOpened, (), (override));
    MOCK_METHOD(void, throwState, (), ());
};

class TimedDoorTest : public testing::Test {
 protected:
    TimedDoor* timedDoor;
    Timer* timer;

    void SetUp() override {
        timedDoor = new TimedDoor(2);
        timer = new Timer();
    }

    void TearDown() override {
        delete timedDoor;
        delete timer;
    }
};

TEST_F(TimedDoorTest, UnlockingDoorKeepsItOpenAfterTimeout) {
    timedDoor->unlock();
    std::this_thread::sleep_for(std::chrono::seconds(
        timedDoor->getTimeOut() + 5));
    EXPECT_TRUE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, LockingDoorClosesItAfterTimeout) {
    timedDoor->lock();
    std::this_thread::sleep_for(std::chrono::seconds(
        timedDoor->getTimeOut() + 5));
    EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, LockingAfterUnlockingClosesTheDoor) {
    timedDoor->unlock();
    std::this_thread::sleep_for(std::chrono::seconds(
        timedDoor->getTimeOut() - 1));
    timedDoor->lock();
    std::this_thread::sleep_for(std::chrono::seconds(
        timedDoor->getTimeOut() + 1));
    EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, MultipleUnlocksKeepsDoorOpenAfterTimeout) {
    timedDoor->unlock();
    std::this_thread::sleep_for(std::chrono::seconds(
        timedDoor->getTimeOut() + 3));
    timedDoor->unlock();
    std::this_thread::sleep_for(std::chrono::seconds(
        timedDoor->getTimeOut() + 3));
    timedDoor->unlock();
    std::this_thread::sleep_for(std::chrono::seconds(
        timedDoor->getTimeOut() + 3));
    EXPECT_TRUE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorOpensWhenUnlocked) {
    timedDoor->unlock();
    EXPECT_TRUE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, DoorClosesWhenLocked) {
    timedDoor->lock();
    EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, MultipleUnlocksKeepsDoorOpen) {
    timedDoor->unlock();
    timedDoor->unlock();
    EXPECT_TRUE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, MultipleLocksKeepsDoorClosed) {
    timedDoor->unlock();
    timedDoor->lock();
    timedDoor->lock();
    EXPECT_FALSE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, OpenAndCloseDoorFunctionality) {
    timedDoor->unlock();
    timedDoor->lock();
    EXPECT_FALSE(timedDoor->isDoorOpened());
    timedDoor->unlock();
    EXPECT_TRUE(timedDoor->isDoorOpened());
}

TEST_F(TimedDoorTest, CorrectTimeoutValueReturned) {
    int expectedTimeout = 2;
    EXPECT_EQ(timedDoor->getTimeOut(), expectedTimeout);
}
