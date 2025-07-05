#ifndef ELEVATOR_INFO_H
#define ELEVATOR_INFO_H

#include <vector>
#include <cstring>
#include <string>
#include <cstdint>
#include "ElevatorEnums.h"

/**
 * Maintains real-time information about individual elevators for coordination by the central dispatcher.
 */
class ElevatorInfo {
private:
    int currentFloor;         // Present vertical position of the elevator cabin
    Direction travelDirection; // Ongoing vertical trajectory of the elevator
    int passengers;            // Quantity of people currently inside the cabin
    int elevatorID;    // Unique numeric designation for this particular elevator
    int doorState;             // Entry portal condition (0=sealed, 1=accessible)
    int targetFloor;           // Intended stopping point for ongoing journey
    int startingFloor;         // Departure point for current transport operation
    bool taskFinished;         // Whether the active transport assignment is concluded

public:
    /**
     * Initializes an elevator tracking entity with baseline configuration values
     */
    ElevatorInfo() 
        : currentFloor(1), travelDirection(Direction::DIRECTION_UP), 
          passengers(0), elevatorID(0), doorState(0), 
          targetFloor(0), startingFloor(0), taskFinished(false) {}
    
    /**
     * Reconstructs elevator state information from binary network data
     */
    ElevatorInfo(const std::vector<uint8_t>& dataBytes) {
        // Transform network byte sequence into structured elevator data
        if (dataBytes.size() >= 32) { // Reduced size - 4 bytes less for outOfService
            int position = 0;
            
            std::memcpy(&currentFloor, dataBytes.data() + position, sizeof(int));
            position += sizeof(int);
            
            int directionValue;
            std::memcpy(&directionValue, dataBytes.data() + position, sizeof(int));
            travelDirection = static_cast<Direction>(directionValue);
            position += sizeof(int);
            
            // Skip outOfService field which was removed
            
            std::memcpy(&passengers, dataBytes.data() + position, sizeof(int));
            position += sizeof(int);
            
            std::memcpy(&elevatorID, dataBytes.data() + position, sizeof(int));
            position += sizeof(int);
            
            std::memcpy(&doorState, dataBytes.data() + position, sizeof(int));
            position += sizeof(int);
            
            std::memcpy(&targetFloor, dataBytes.data() + position, sizeof(int));
            position += sizeof(int);
            
            std::memcpy(&startingFloor, dataBytes.data() + position, sizeof(int));
            position += sizeof(int);
            
            int completionStatus;
            std::memcpy(&completionStatus, dataBytes.data() + position, sizeof(int));
            taskFinished = (completionStatus == 1);
        }
    }

    // Access methods for retrieving elevator properties
    int getElevatorId() const { return elevatorID; }
    int getOccupantCount() const { return passengers; }
    int getCurrentPosition() const { return currentFloor; }
    Direction getMovementDirection() const { return travelDirection; }
    int getDoorPosition() const { return doorState; }
    int getFinalDestination() const { return targetFloor; }
    int getInitialPosition() const { return startingFloor; }
    bool isTaskComplete() const { return taskFinished; }

    // Modifier methods for updating elevator attributes
    void assignElevatorId(int id) { elevatorID = id; }
    void updateOccupantCount(int count) { passengers = count; }
    void updatePosition(int floor) { currentFloor = floor; }
    void changeDirection(Direction newDirection) { travelDirection = newDirection; }
    void setDoorPosition(int state) { doorState = state; }
    void setTargetFloor(int destination) { targetFloor = destination; }
    void setStartingFloor(int origin) { startingFloor = origin; }
    void markTaskComplete(bool status) { taskFinished = status; }

};

#endif // ELEVATOR_INFO_H