// Copyright 2024 Kachalov Mikhail

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <thread> // NOLINT [build/c++11]
#include <chrono> // NOLINT [build/c++11]
#include "TimedDoor.h"

class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, ());
};

class MockTimedDoor : public TimedDoor {
 public:
    explicit MockTimedDoor(int timeout) : TimedDoor(timeout) {}
};

class TaskDoorTime : public ::testing::Test {
 protected:
    TimedDoor* door;

    void SetUp() override {
        door = new TimedDoor(100);
    }

    void TearDown() override {
        delete door;
    }
};


TEST_F(TaskDoorTime, DoorLockTest) {
    EXPECT_CALL(*door, lock()).Times(1);
    door->lock();
}

TEST_F(TaskDoorTime, DoorUnlockTest) {
    EXPECT_CALL(*door, unlock()).Times(1);
    door->unlock();
}

TEST_F(TaskDoorTime, DoorOpenedTest) {
    EXPECT_CALL(*door, isDoorOpened())
        .Times(1)
        .WillOnce(::testing::Return(true));

    bool opened = door->isDoorOpened();
    EXPECT_TRUE(opened);
}

TEST_F(TaskDoorTime, OpenedDoorThrowsExceptionAfterTimeout) {
    door->unlock();
    EXPECT_CALL(*door, isDoorOpened())
        .Times(1)
        .WillOnce(::testing::Return(true));

    EXPECT_THROW(door->throwState(), std::runtime_error); 
}

TEST_F(TaskDoorTime, ClosedDoorNoExceptionAfterTimeout) {
    door->lock();
    EXPECT_CALL(*door, isDoorOpened())
        .Times(1)
        .WillOnce(::testing::Return(false));

    EXPECT_NO_THROW(door->throwState());
}

TEST_F(TaskDoorTime, TimerRegistersClient) {
    Timer timer;
    EXPECT_NO_THROW(timer.tregister(5, mock_timer_client));
}

TEST_F(TaskDoorTime, TimerTriggersTimeout) {
    Timer timer;
    EXPECT_CALL(*mock_timer_client, Timeout()).Times(1);
    timer.tregister(5, mock_timer_client);
}

TEST_F(TaskDoorTime, TimedDoorGetTimeout) {
    EXPECT_EQ(door->getTimeOut(), 100);
}

TEST_F(TaskDoorTime, DoorAndTimerInteractionTest) {
    EXPECT_CALL(*door, unlock()).Times(1);
    EXPECT_CALL(*mock_timer_client, Timeout()).Times(1);

    door->unlock();
    Timer timer;
    timer.tregister(5, &adapter);
    adapter.Timeout();
}
