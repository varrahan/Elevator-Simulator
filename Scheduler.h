#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <map>
#include <vector>
#include "Event.h"
#include "ElevatorInfo.h"

#define SCHEDULER_PORT 8000
#define FLOOR_PORT 8001  
#define ELEVATOR_PORT 8002  
#define ELEVATOR_PORT_BASE 9000  // Base port for elevator subsystems
#define ELEVATOR_CAPACITY 10

#include "ElevatorEnums.h"

class Scheduler {
private:
    DatagramSocket receiveSocket; // for receiving from either the floor or elevatorsubsystem
    DatagramSocket floorSendSocket;  // for sending responses back to the floor
    DatagramSocket elevatorSendSocket; // for sending events to the elevator

    std::mutex floorMtx, elevatorMtx, stateMtx, elevatorInfoMtx;
    std::condition_variable floorCV, elevatorCV;
    std::atomic<bool> done{false};
    schedulerState state = schedulerState::SCHEDULER_IDLE;

    std::vector<int> removedElevators;
    
    // Track elevator information
    struct ElevatorInfo {
        int id;
        int currentFloor;
        elevatorState state;
        int passengers;
        int totalPassengers;
        bool isBusy;
    };
    
    std::map<int, ElevatorInfo> elevatorInfoMap;
    int numElevators;
public:
    /**
     * Constructor for the Scheduler class
     * @param elevatorCount The number of elevators in the system
     */
    Scheduler(int elevatorCount = 4);
    
    ~Scheduler() = default;

    void removeElevator(int elevatorId);

    /**
     * Mark the scheduler as finished and notify all threads
     */
    void finish();

    /**
     * Main function that continuously processes events from the floor and sends them to the elevator
     */
    void run();

    /**
     * Checks if the scheduler is finished or not 
     * @return True if scheduler is finished, false otherwise
     */
    bool isFinish();
    
    /**
     * Get the current state of the scheduler
     * @return The current scheduler state
     */
    schedulerState getState() const;
    
    /**
     * Get the number of elevators in the system
     * @return The number of elevators
     */
    int getNumElevators() const { return numElevators; }
    void updateState(schedulerState newState);
    void sendToFloor(const Event& event);
    void sendToElevator(const Event& event);
    bool receiveEvent(Event& event);
    
    // Method to assign the optimal elevator based on various factors
    int assignOptimalElevator(const Event& event);
    
    // Updates internal elevator information based on received updates
    void updateElevatorInfo(const Event& event);
    
    /**
     * Get the information map of the elevators
     * @return  The infomap of the elevator
     */
    std::map<int, ElevatorInfo> getInfoMap() { 
        return elevatorInfoMap;
     } 

    /**
     * Get the list of removed elevators
     * @return a list of removed elevators
     */
     std::vector<int> getRemovedElevators(){
        return removedElevators;
     }
};

#endif