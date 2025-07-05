
#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <sstream>
#include <thread>
#include <vector>
#include <memory>
#include <cassert>
#include "../Floor.h"
#include "../ElevatorSubsystem.h"

// Default number of elevators if not specified
#define DEFAULT_NUM_ELEVATORS 4

// Base port for elevator subsystems
#define ELEVATOR_PORT_BASE 9000

// Test event creation function
Event createTestEvent(bool isFromFloor, const std::string& source, 
    const std::string& floorBtn, int elevatorBtn, int faultType) {
    Event event;
    event.isFromFloor = isFromFloor;
    event.source = source;
    event.floorButton = floorBtn;
    event.elevatorButton = elevatorBtn;
    event.currentFloor = std::stoi(source);
    event.fault = faultType;
    return event;
}

int main() {

    Scheduler scheduler;
    
    // Use a vector to store all the elevatorSubsystems, each with its own port
    std::vector<std::unique_ptr<ElevatorSubsystem>> elevatorSubsystems;
    std::vector<std::thread> elevatorSubsystemThreads;
    
    for (int i = 0; i < DEFAULT_NUM_ELEVATORS; i++) {
        int port = ELEVATOR_PORT_BASE + i;
        // Assert that the port is valid (within a reasonable range, for example)
        assert(port >= ELEVATOR_PORT_BASE && port < ELEVATOR_PORT_BASE + 100 && "Port number out of range.");

        elevatorSubsystems.push_back(std::make_unique<ElevatorSubsystem>(scheduler, i, port));
        assert(elevatorSubsystems[i] != nullptr && "Failed to create elevator subsystem.");

        // Assert that the elevator ID matches the expected value
        assert(elevatorSubsystems[i]->getElevatorId() == i && "Elevator ID mismatch.");
        elevatorSubsystemThreads.push_back(std::thread(&ElevatorSubsystem::run, elevatorSubsystems[i].get()));
    }
    std::cout << "Test Passed: All four elevators successfully initialized." << std::endl;


    // Fault Testing with events

    // Stuck elevator fault
    Event elevator_stuck = createTestEvent(true, "2", "UP", 5, ELEVATOR_STUCK);
    int elevatorId = scheduler.assignOptimalElevator(elevator_stuck);
    elevatorSubsystems[elevatorId]->getElevator()->setEvent(elevator_stuck);
    std::this_thread::sleep_for(std::chrono::seconds(2)); 
    assert(elevatorSubsystems[elevatorId]->getElevator()->moveTo(2) == false && "Elevator should fail due to being stuck.");
    std::cout << "Test Passed: Elevator failed because of being stuck." << std::endl;

    // Arrival sensor fault
    Event arrival_sensor = createTestEvent(true, "3", "DOWN", 1, ARRIVAL_SENSOR_ISSUE);
    elevatorId = scheduler.assignOptimalElevator(arrival_sensor);
    elevatorSubsystems[elevatorId]->getElevator()->setEvent(arrival_sensor);
    std::this_thread::sleep_for(std::chrono::seconds(2)); 
    assert(elevatorSubsystems[elevatorId]->getElevator()->moveTo(3) == false && "Elevator was supposed to fail because the arrival sensor had a fault.");
    std::cout << "Test Passed: Elevator failed because of arrival sensor fault." << std::endl;

    // Doors open stuck fault
    Event door_open_stuck = createTestEvent(true, "4", "UP", 6, DOOR_OPEN_STUCK);
    elevatorId = scheduler.assignOptimalElevator(door_open_stuck);
    elevatorSubsystems[elevatorId]->getElevator()->openDoors();
    std::this_thread::sleep_for(std::chrono::seconds(2)); 
    assert(elevatorSubsystems[elevatorId]->getElevator()->getState() == ELEVATOR_DOOR_OPEN && "Elevator doors were supposed to be stuck open.");
    std::cout << "Test Passed: Elevator failed because of elevator doors were stuck open." << std::endl;

    // Door close stuck fault
    Event door_close_stuck = createTestEvent(true, "5", "DOWN", 2, DOOR_CLOSE_STUCK);
    elevatorId = scheduler.assignOptimalElevator(door_close_stuck);
    elevatorSubsystems[elevatorId]->getElevator()->closeDoors();
    std::this_thread::sleep_for(std::chrono::seconds(2)); 
    assert(elevatorSubsystems[elevatorId]->getElevator()->getState() == ELEVATOR_DOOR_CLOSE && "Elevator doors were supposed to be stuck closed.");
    std::cout << "Test Passed: Elevator failed because of elevator doors were stuck closed." << std::endl;

    std::cout << "All fault tests passed successfully." << std::endl;
    
    scheduler.finish();  // finish the scheduler
    assert(scheduler.isFinish() == true);  // Ensure the scheduler is finished (won't actually run since the scheduler exits)
    
    // Wait for all elevator subsystem threads to complete
    for (auto& thread : elevatorSubsystemThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    return 0;
}
