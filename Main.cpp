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
#include "Floor.h"
#include "ElevatorSubsystem.h"

// Default number of elevators if not specified
#define DEFAULT_NUM_ELEVATORS 4

// Base port for elevator subsystems
#define ELEVATOR_PORT_BASE 9000

int main(int argc, char* argv[]) {
    // Get filename and number of elevators
    std::string filename = argv[1];
    int numElevators = (argc > 2) ? std::stoi(argv[2]) : DEFAULT_NUM_ELEVATORS;
    
    std::cout << "Starting elevator system with " << numElevators << " elevators" << std::endl;

    // Create scheduler with specified number of elevators
    Scheduler scheduler(numElevators);

    // Create floor instance
    Floor floor(scheduler, filename);

    // Use a vector to store all the elevatorSubsystems, each with its own port
    std::vector<std::unique_ptr<ElevatorSubsystem>> elevatorSubsystems;
    std::vector<std::thread> elevatorSubsystemThreads;
    
    for (int i = 0; i < numElevators; i++) {
        int port = ELEVATOR_PORT_BASE + i;
        elevatorSubsystems.push_back(std::make_unique<ElevatorSubsystem>(scheduler, i, port));
        elevatorSubsystemThreads.push_back(std::thread(&ElevatorSubsystem::run, elevatorSubsystems[i].get()));
    }

    // Create scheduler and floor threads
    std::thread schedulerThread(&Scheduler::run, &scheduler);
    std::thread floorThread(&Floor::run, &floor);

    // Wait for floor thread to complete
    floorThread.join();

    // Wait for scheduler thread to complete
    schedulerThread.join();
    
    // Wait for all elevator subsystem threads to complete
    for (auto& thread : elevatorSubsystemThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    return 0;
}