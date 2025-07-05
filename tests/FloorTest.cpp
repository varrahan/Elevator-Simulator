#include <iostream>
#include <thread>
#include <cassert>
#include "../Floor.h"
#include "../Scheduler.h"
#include "../ElevatorSubsystem.h"

// Test event creation function
Event createTestEvent(const std::string& source, const std::string& floorBtn, int elevatorBtn, int faultType) {
    Event event;
    event.isFromFloor = true;
    event.source = source;
    event.floorButton = floorBtn;
    event.elevatorButton = elevatorBtn;
    event.currentFloor = std::stoi(source);
    event.fault = faultType;
    return event;
}

int main() {
    // simulated input file
    const char* tempFileName = "temp_floor_test.txt";
    std::ofstream outFile(tempFileName);
    assert(outFile.is_open());
    //simulated headers (will be skipped)
    outFile << "Header line 1\n";
    outFile << "Header line 2\n";
    // Write sample event data
    outFile << "14:05:15.0 2 Up 4 0 \n"; 
    outFile << "14:07:15.0 5 Up 7 1 \n"; 
    outFile << "14:09:15.0 5 Up 7 2 \n"; 
    outFile << "14:11:13.0 2 Down 4 3 \n"; 
    outFile << "14:13:12.0 3 Down 1 4 \n"; 

    outFile.close();

    int NUM_ELEVATORS = 4;
    Scheduler scheduler(NUM_ELEVATORS);
    std::cout << "Scheduler created with 2 elevators" << std::endl;

    Floor floor(scheduler, tempFileName);
    std::cout << "Floor instance created" << std::endl;
    assert(floor.getCompletedEvents() == 0 && "Floor should have 0 completed events initially");
    
    // Use a vector to store all the elevatorSubsystems, each with its own port
    std::vector<std::unique_ptr<ElevatorSubsystem>> elevatorSubsystems;
    std::vector<std::thread> elevatorSubsystemThreads;

    // Create elevator subsytem threads
    for (int i = 0; i < NUM_ELEVATORS; i++) {
        int port = ELEVATOR_PORT_BASE + i;
        elevatorSubsystems.push_back(std::make_unique<ElevatorSubsystem>(scheduler, i, port));
        elevatorSubsystemThreads.push_back(std::thread(&ElevatorSubsystem::run, elevatorSubsystems[i].get()));
    }

    // Create scheduler and floor threads
    std::thread schedulerThread(&Scheduler::run, &scheduler);
    std::thread floorThread(&Floor::run, &floor);

    // Wait for floor thread to complete
    floorThread.join();
    assert(floor.getTotalEvents() > 0 && "Floor thread did not complete successfully.");

    // Wait for scheduler thread to complete
    schedulerThread.join();
    scheduler.finish();

    // Wait for all elevator subsystem threads to complete
    for (auto& thread : elevatorSubsystemThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    std::cout << "Floor test completed successfully." << std::endl;
    return 0;
}