// Copyright 2024 Kachalov Mikhail

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include <thread> // NOLINT [build/c++11]
#include <chrono> // NOLINT [build/c++11]
#include "TimedDoor.h"

class MockDoor : public Door {
 public:
  MOCK_METHOD(void, lock, (), (override));
  MOCK_METHOD(void, unlock, (), (override));
  MOCK_METHOD(bool, isDoorOpened, (), (override));
};

class MockTimerClient : public TimerClient {
 public:
  MOCK_METHOD(void, Timeout, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
  MockDoor* mock_door;
  MockTimerClient* mock_timer_client;
  TimedDoor* timed_door;

  void SetUp() override {
    mock_door = new MockDoor();
    mock_timer_client = new MockTimerClient();
    timed_door = new TimedDoor(5);
  }

  void TearDown() override {
    delete mock_door;
    delete mock_timer_client;
    delete timed_door;
  }
};

TEST_F(TimedDoorTest, DoorLockTest) {
  EXPECT_CALL(*mock_door, lock()).Times(1);
  timed_door->lock();
}

TEST_F(TimedDoorTest, DoorUnlockTest) {
  EXPECT_CALL(*mock_door, unlock()).Times(1);
  timed_door->unlock();
}

TEST_F(TimedDoorTest, DoorOpenedTest) {
  EXPECT_CALL(*mock_door, isDoorOpened())
      .Times(1)
      .WillOnce(::testing::Return(true));

  bool opened = timed_door->isDoorOpened();
  EXPECT_TRUE(opened);
}

TEST_F(TimedDoorTest, TimerTimeoutTest) {
  EXPECT_CALL(*mock_timer_client, Timeout()).Times(1);
  mock_timer_client->Timeout();
}

TEST_F(TimedDoorTest, OpenedDoorThrowsExceptionAfterTimeout) {
  EXPECT_CALL(*mock_door, isDoorOpened())
      .Times(1)
      .WillOnce(::testing::Return(true));

  EXPECT_THROW(timed_door->throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, ClosedDoorNoExceptionAfterTimeout) {
  EXPECT_CALL(*mock_door, isDoorOpened())
      .Times(1)
      .WillOnce(::testing::Return(false));

  EXPECT_NO_THROW(timed_door->throwState());
}

TEST_F(TimedDoorTest, TimerRegistersClient) {
  Timer timer;
  EXPECT_NO_THROW(timer.tregister(5, mock_timer_client));
}

TEST_F(TimedDoorTest, TimerTriggersTimeout) {
  Timer timer;
  EXPECT_CALL(*mock_timer_client, Timeout()).Times(1);
  timer.tregister(5, mock_timer_client);
}

TEST_F(TimedDoorTest, TimedDoorGetTimeout) {
  EXPECT_EQ(timed_door->getTimeOut(), 5);
}

TEST_F(TimedDoorTest, DoorAndTimerInteractionTest) {
  Timer timer;
  DoorTimerAdapter adapter(*timed_door);

  EXPECT_CALL(*mock_door, unlock()).Times(1);
  EXPECT_CALL(*mock_timer_client, Timeout()).Times(1);

  timed_door->unlock();
  timer.tregister(5, &adapter);
  adapter.Timeout();
}
