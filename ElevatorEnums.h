#ifndef ELEVATOR_ENUMS_H
#define ELEVATOR_ENUMS_H

/**
 * Enumerations used throughout the elevator system
 */

// Scheduler states
enum schedulerState {
    SCHEDULER_IDLE, 
    SCHEDULER_THINKING, 
    SCHEDULER_ALLOCATE_ELEVATOR
};

// Elevator movement states 
enum elevatorState {
    ELEVATOR_REST, 
    ELEVATOR_MOVING_UP, 
    ELEVATOR_MOVING_DOWN, 
    ELEVATOR_DOOR_OPEN, 
    ELEVATOR_DOOR_CLOSE
};

// Direction enumeration 
enum Direction {
    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_IDLE
};

#endif // ELEVATOR_ENUMS_H