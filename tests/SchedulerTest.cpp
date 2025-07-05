#include <iostream>
#include <thread>
#include <chrono>
#include <cassert>
#include <algorithm>
#include "../Scheduler.h"
#include "../ElevatorSubsystem.h"

#define NUM_ELEVATORS 4

// Test event creation function
Event createTestEvent(bool isFromFloor, const std::string& source, 
                      const std::string& floorBtn, int elevatorBtn, int fault) {
    Event event;
    event.isFromFloor = isFromFloor;
    event.source = source;
    event.floorButton = floorBtn;
    event.elevatorButton = elevatorBtn;
    event.currentFloor = std::stoi(source);
    event.fault = fault;
    return event;
}

// Run testing program
int main() {
    // Create Scheduler with 3 elevators
    Scheduler scheduler(NUM_ELEVATORS);
    std::cout << "Scheduler created with 3 elevators" << std::endl;
    
    // Confirm the number of elevators
    assert(scheduler.getNumElevators() == NUM_ELEVATORS && "Three elevators created");    
    
    // Start scheduler thread
    std::thread schedulerThread([&scheduler]() {
        scheduler.run();
    });

    // Use a vector to store all the elevatorSubsystems, each with its own port
    std::vector<std::unique_ptr<ElevatorSubsystem>> elevatorSubsystems;
    std::vector<std::thread> elevatorSubsystemThreads;
    
    for (int i = 0; i < NUM_ELEVATORS; i++) {
        int port = ELEVATOR_PORT_BASE + i;
        elevatorSubsystems.push_back(std::make_unique<ElevatorSubsystem>(scheduler, i, port));
        elevatorSubsystemThreads.push_back(std::thread(&ElevatorSubsystem::run, elevatorSubsystems[i].get()));
    }
    
    // Test the scheduler with different scenarios to check elevator assignment
    std::cout << "\nTesting elevator assignment:" << std::endl;
    
    // Test scenario 1 - no fault
    Event noFaultRequest = createTestEvent(true, "1", "UP", 3, 0);
    int elevator1 = scheduler.assignOptimalElevator(noFaultRequest);
    elevatorSubsystems[elevator1]->getElevator()->setEvent(noFaultRequest);
    std::cout << "Floor 3 with button UP request assigned to elevator " << elevator1 << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2)); 
    // Test if most optimal elevator is assigned
    assert(scheduler.getInfoMap()[elevator1].isBusy && "Optimal elevator is assigned");
    assert((scheduler.getInfoMap()[elevator1].id == 0) || (scheduler.getInfoMap()[elevator1].id == 1)  || (scheduler.getInfoMap()[elevator1].id == 2) && "All choices are optimal");
    std::cout << "Test passed: Elevator no fault did not terminate the elevator"<< std::endl;


    // Test scenario 2 - doors open stuck fault
    Event doorsOpenRequest = createTestEvent(true, "4", "DOWN", 2, DOOR_OPEN_STUCK);
    int elevator2 = scheduler.assignOptimalElevator(doorsOpenRequest);
    elevatorSubsystems[elevator2]->getElevator()->setEvent(doorsOpenRequest);
    std::cout << "Floor 7 with button DOWN request assigned to elevator " << elevator2 << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2)); 
    // Test if most optimal elevator is assigned
    assert(scheduler.getInfoMap()[elevator2].isBusy && "Optimal elevator is assigned");
    assert((scheduler.getInfoMap()[elevator2].id == 1)  || (scheduler.getInfoMap()[elevator2].id == 2) && "Elevator 1 and 2 are optimal choices");
    // Test to make sure elevator is not terminate with door open stuck fault
    assert(std::find(scheduler.getRemovedElevators().begin(), scheduler.getRemovedElevators().end(), elevator2) == scheduler.getRemovedElevators().end());
    std::cout << "Test passed: Elevator door open stuck fault did not terminate the elevator"<< std::endl;
    

    // Test scenario 3 - door close stuck fault
    Event doorsCloseRequest = createTestEvent(true, "5", "DOWN", 1, DOOR_CLOSE_STUCK);
    int elevator3 = scheduler.assignOptimalElevator(doorsCloseRequest);
    elevatorSubsystems[elevator3]->getElevator()->setEvent(doorsCloseRequest);
    std::cout << "Floor 5 button DOWN request assigned to elevator: " << elevator3 << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2)); 
    // Test if most optimal elevator is assigned
    assert(scheduler.getInfoMap()[elevator3].isBusy && "Optimal elevator is assigned");
    assert((scheduler.getInfoMap()[elevator3].id == 2)  && "Elevator 2 is the optimal choices");
    // Test to make sure elevator is not terminate with door close stuck fault
    assert(std::find(scheduler.getRemovedElevators().begin(), scheduler.getRemovedElevators().end(), elevator3) == scheduler.getRemovedElevators().end());
    std::cout << "Test passed: Elevator door close stuck fault did not terminate the elevator"<< std::endl;


    // Test scenario 4 - arrival sensor fault
    Event arrivalSensorRequest = createTestEvent(true, "3", "UP", 6, ARRIVAL_SENSOR_ISSUE);
    int elevator4 = scheduler.assignOptimalElevator(arrivalSensorRequest);
    elevatorSubsystems[elevator4]->getElevator()->setEvent(arrivalSensorRequest);
    elevatorSubsystems[elevator4]->removeElevator();
    std::cout << "Floor 3 button UP request assigned to elevator: " << elevator4 << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5)); 
    // Test to make sure elevator is terminated because arrival sensor fault was set to true 
    assert(std::find(scheduler.getRemovedElevators().begin(), scheduler.getRemovedElevators().end(), elevator4) != scheduler.getRemovedElevators().end());
    std::cout << "Test passed: Elevator arrival sensor did terminate the elevator"<< std::endl;



     // Test scenario 1 - elevator stuck fault
     Event elevatorStuckRequest = createTestEvent(true, "1", "UP", 3, 1);
     int elevator5 = scheduler.assignOptimalElevator(elevatorStuckRequest);
     elevatorSubsystems[elevator1]->getElevator()->setEvent(elevatorStuckRequest);
     std::cout << "Floor 3 with button UP request assigned to elevator " << elevator1 << std::endl;
     std::this_thread::sleep_for(std::chrono::seconds(2)); 
    // Test to make sure elevator is not terminated with elevator stuck fault
    assert(std::find(scheduler.getRemovedElevators().begin(), scheduler.getRemovedElevators().end(), elevator5) != scheduler.getRemovedElevators().end());
    std::cout << "Test passed: Elevator stuck fault did not terminate the elevator"<< std::endl;


    // Complete test and finish scheduler thread
    std::cout << "\nTest complete" << std::endl;
    scheduler.finish();
    if (schedulerThread.joinable()) {
        schedulerThread.join();
    }

    for (auto& thread : elevatorSubsystemThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    return 0;
}