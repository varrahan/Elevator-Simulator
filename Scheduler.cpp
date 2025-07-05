#include "Scheduler.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <algorithm>
#include <limits>

/**
 * Constructor for the Scheduler class
 * @param elevatorCount The number of elevators in the system
 */
Scheduler::Scheduler(int elevatorCount) 
    : floorMtx(), elevatorMtx(), stateMtx(), elevatorInfoMtx(),
      floorCV(), elevatorCV(), 
      receiveSocket(SCHEDULER_PORT), floorSendSocket(), elevatorSendSocket(),
      numElevators(elevatorCount) {
    
    // Initialize elevator info map
    // Using 0-based indexing to be consistent with the ElevatorSubsystem
    for (int i = 0; i < numElevators; ++i) {
        elevatorInfoMap[i] = {i, 1, elevatorState::ELEVATOR_REST, 0, 0, false}; // Start at floor 1
    }
}

void Scheduler::updateState(schedulerState newState) {
    std::unique_lock<std::mutex> lock(stateMtx);
    state = newState;
}

void Scheduler::removeElevator(int elevatorId) {
    std::unique_lock<std::mutex> lock(stateMtx);
    elevatorInfoMap.erase(elevatorId);
    removedElevators.push_back(elevatorId);
}

/**
 * Get the current state of the scheduler
 * @return The current scheduler state
 */
schedulerState Scheduler::getState() const {
    return state;
}

void Scheduler::sendToFloor(const Event& event) {
    try {
        std::vector<uint8_t> data = event.event_to_bytes();
        DatagramPacket packet(data, data.size(), InetAddress::getLocalHost(), FLOOR_PORT);
        floorSendSocket.send(packet);
        std::cout << "Sent message to floor" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error sending to floor: " << e.what() << std::endl;
    }
}

void Scheduler::sendToElevator(const Event& event) {
    try {
        // Calculate the correct port for the assigned elevator
        int elevatorPort = ELEVATOR_PORT_BASE + event.assignedElevator; // This works because the ports have a "Base + offset which is the id"
        
        std::vector<uint8_t> data = event.event_to_bytes();
        DatagramPacket packet(data, data.size(), InetAddress::getLocalHost(), elevatorPort);
        elevatorSendSocket.send(packet);
        std::cout << "Sent message to elevator " << event.assignedElevator 
                  << " on port " << elevatorPort << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error sending to elevator: " << e.what() << std::endl;
    }
}

bool Scheduler::receiveEvent(Event& event) {
    try {
        std::vector<uint8_t> data(100);
        DatagramPacket packet(data, data.size());
        
        //because there are no more events left to receive
        if (done) return false;
        
        // Receive the packet
        receiveSocket.receive(packet);
        
        // Deserialize the event
        event = Event::bytes_to_event(data);

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error receiving event: " << e.what() << std::endl;
        return false;
    }
}

void Scheduler::updateElevatorInfo(const Event& event) {
    std::lock_guard<std::mutex> lock(elevatorInfoMtx);
    
    int elevatorId = event.assignedElevator; //Get associated ID
 
    // Update the elevator's current floor
    elevatorInfoMap[elevatorId].currentFloor = event.currentFloor;
    
    // Update passengers 
    if (event.riders >= 0) {
        elevatorInfoMap[elevatorId].passengers = event.riders;
    }
    
    // Determine elevator direction based on the floorButton field
    if (event.floorButton == "UP") {
        elevatorInfoMap[elevatorId].state = elevatorState::ELEVATOR_MOVING_UP;
    } else if (event.floorButton == "DOWN") {
        elevatorInfoMap[elevatorId].state = elevatorState::ELEVATOR_MOVING_DOWN;
    } else if (event.floorButton.empty() && event.source.find("Elevator:") != std::string::npos) {
        // when no direction given, assume it's at REST
        elevatorInfoMap[elevatorId].state = elevatorState::ELEVATOR_REST;
    }
    
    // MARK ELEVATOR AS NOT BUSY IF THE REQUEST IS COMPLETE
    if (event.isComplete) { elevatorInfoMap[elevatorId].isBusy = false;}
    
}

int Scheduler::assignOptimalElevator(const Event& event) {
    std::lock_guard<std::mutex> lock(elevatorInfoMtx);
    
    // Parse request details
    int originFloor = std::stoi(event.source);
    bool isGoingUp = (event.floorButton == "UP");
    
    // Find the best elevator for this request
    int bestElevator = -1;
    int bestScore = 1000000000; // A very big number
    
    for (int i = 0; i < numElevators; i++) {
        if (std::find(removedElevators.begin(), removedElevators.end(), i) != removedElevators.end()) {
            continue;
        }
        auto& info = elevatorInfoMap[i];
        
        // Calculate a score for this elevator (lower is better)
        int score = 1000;  // Base score
        
        // Add a lot of points if the elevator is in use, makes it unlikely to be chosen to service a request
        if (info.isBusy) {
            score += 5000; 
        }
      
        // Check position relative to request
        int distance = abs(info.currentFloor - originFloor);
        score += distance * 10;  // weighted calculation for the score, the further it is the worse
        
        bool elevatorGoingUp = (info.state == elevatorState::ELEVATOR_MOVING_UP);
        bool elevatorGoingDown = (info.state == elevatorState::ELEVATOR_MOVING_DOWN);
        bool elevatorAtRest = (info.state == elevatorState::ELEVATOR_REST || info.state == elevatorState::ELEVATOR_DOOR_OPEN || info.state == elevatorState::ELEVATOR_DOOR_CLOSE);
        
        // change the score based on the elevator's direction
        if (isGoingUp) {
            if (elevatorGoingUp && info.currentFloor <= originFloor) { // if passenger going up and elevator is below him and going up 
                score -= 500;
            }
            else if (elevatorAtRest) {// Elevator at rest can be used
                score -= 300;
            }
        } 
        else {
            // Passenger going DOWN - elevators above are good
            if (elevatorGoingDown && info.currentFloor >= originFloor) { // if passenger is going down and there's an elevator above him and going down
                score -= 500;
            }
            else if (info.currentFloor > originFloor) {// ANY elevator above the passenger going down gets priority
                score -= 400;
            }
            else if (elevatorAtRest) {
                // Elevator at rest can be used
                score -= 300;
            }
        }
        
      
        std::cout << "  Elevator " << i << " score: " << score << std::endl;
        
        // Update best elevator if this one has a better score
        if (score < bestScore) {
            bestScore = score;
            bestElevator = i;
        }
    }
    
    // If we couldn't find a suitable elevator use the next one
    if (bestElevator == -1) {
        static int lastAssigned = -1;
        bestElevator = (lastAssigned + 1) % numElevators;
        lastAssigned = bestElevator;
    }
    
    // Mark the chosen elevators as busy
    elevatorInfoMap[bestElevator].isBusy = true;
    
    return bestElevator;
}

/**
 * Mark the scheduler as finished and notify all threads
 */
void Scheduler::finish() {
    std::unique_lock<std::mutex> lock(floorMtx);
    done.store(true);
    floorCV.notify_all();
    elevatorCV.notify_all();
    exit(1);
}

/**
 * Checks if the scheduler is finished or not 
 * @return True if scheduler is finished, false otherwise
 */
bool Scheduler::isFinish() {
    return done;
}

/**
 * Main function that continuously processes events from the floor and sends them to the elevator
 */
void Scheduler::run() {
    while (!done) {
        updateState(schedulerState::SCHEDULER_IDLE);

        Event event;
        if (receiveEvent(event)) {
            if (event.isFromFloor) {
                // Process floor request
                updateState(schedulerState::SCHEDULER_ALLOCATE_ELEVATOR);

                // Select the optimal elevator based on our algorithm
                int chosenElevator = assignOptimalElevator(event);
                
                // Modify the event to include the assigned elevator
                event.assignedElevator = chosenElevator;
                
                std::cout << "Scheduler processing event: Time=" << event.time 
                          << ", Source=" << event.source 
                          << ", Floor Button=" << event.floorButton 
                          << ", Elevator Button=" << event.elevatorButton 
                          << ", Assigned to Elevator=" << chosenElevator
                          << ", Fault=" << event.fault << std::endl;

                // Send the event to the elevator subsystem
                sendToElevator(event);
            } else {
                    // This is a response from an elevator
                    
                    // Update our internal record of elevator positions and states
                    updateElevatorInfo(event);
                    
                    // Forward to the floor subsystem, especially completion messages
                    if (event.isComplete) {
                        std::cout << "Scheduler forwarding completion notification to floor" << std::endl;
                    }
                    sendToFloor(event);
                }
            }
        }
        
        // Small delay to prevent busy waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
