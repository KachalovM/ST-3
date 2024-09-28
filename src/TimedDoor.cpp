// Copyright 2024 Kachalov Mikhail

#include <stdexcept>
#include <chrono> // NOLINT [build/c++11]
#include <thread> // NOLINT [build/c++11]

#include "TimedDoor.h"

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& door) : door(door) {}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        throw std::runtime_error("Door is not opened after timeout");
    }
}

TimedDoor::TimedDoor(int timeout)
    : iTimeout(timeout), isOpened(false),
      adapter(new DoorTimerAdapter(*this)) {}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

void TimedDoor::unlock() {
    isOpened = true;
}

void TimedDoor::lock() {
    isOpened = false;
}

int TimedDoor::getTimeOut() const {
    return iTimeout;
}

void TimedDoor::throwState() {
    throw std::runtime_error("Door left open for too long!");
}

void Timer::sleep(int time) {
    std::this_thread::sleep_for(std::chrono::seconds(time));
}

void Timer::tregister(int time, TimerClient* client) {
    sleep(time);
    client->Timeout();
}
