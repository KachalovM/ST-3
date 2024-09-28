// Copyright 2024 Kachalov Mikhail

#include <chrono>
#include <stdexcept>
#include <thread>
#include <iostream>

#include "TimedDoor.h"

DoorTimerAdapter::DoorTimerAdapter(TimedDoor& door) : door(door) {}

void DoorTimerAdapter::Timeout() {
    if (door.isDoorOpened()) {
        door.throwState();
    }
}

TimedDoor::TimedDoor(int timeout) : iTimeout(timeout), isOpened(false), adapter(new DoorTimerAdapter(*this)) {}

bool TimedDoor::isDoorOpened() {
    return isOpened;
}

void TimedDoor::unlock() {
    isOpened = true;
    Timer timer;
    timer.tregister(iTimeout, adapter);
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
