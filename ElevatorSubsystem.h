#ifndef ELEVATORSUBSYSTEM_H
#define ELEVATORSUBSYSTEM_H

#include <condition_variable>
#include <mutex>
#include <thread>
#include <memory>
#include "Scheduler.h"
#include "ElevatorEnums.h"

#define TIME_BTWN_1_FLOOR 9
#define TIME_BTWN_2_FLOORS 11
#define TIME_BTWN_3_FLOORS 13
#define TIME_BTWN_X_FLOORS_PER_FLOOR 4 // This is when floors is more than 3
#define TIME_TO_LOAD_UNLOAD_1_PASSENGER 4 // A passenger loading/unloading
#define TIME_TO_OPEN_CLOSE_DOOR 2 // Time to open and close door

// Faults for system
#define ELEVATOR_STUCK 1
#define DOOR_OPEN_STUCK 2
#define DOOR_CLOSE_STUCK 3
#define ARRIVAL_SENSOR_ISSUE 4

// Recovery time for stuck faults
#define RECOVERY_TIME 5

class Elevator;

/**
 * Elevator subsystem class represents a single elevator subsystem
 * It receives events from the scheduler and manages a single elevator
 */
class ElevatorSubsystem {
private:
    Scheduler& scheduler;
    std::condition_variable cv;         // Condition variable for elevator
    std::mutex mtx;                     // Mutex for elevator operations
    std::unique_ptr<Elevator> elevator; // The elevator managed by this subsystem
    std::thread elevatorThread;         // Thread to run the elevator
    int elevatorId;                     // ID of the elevator

    DatagramSocket receiveSocket;       // Socket to receive events from scheduler
    DatagramSocket sendSocket;          // Socket to send responses to scheduler

    bool receiveEvent(Event& event);
    void sendResponse(const Event& response);

public:
    /**
     * Constructor for the ElevatorSubsystem class
     * 
     * @param s The scheduler instance
     * @param id The ID for this elevator
     * @param port The UDP port to listen on
     */
    ElevatorSubsystem(Scheduler& s, int id, int port);

    /**
     * Adds the elevator response event to the scheduler
     * 
     * @param response The elevator response event
     */
    void addElevatorResponse(Event response);

    /**
     * Checks if the scheduler is finished or not 
     * @return True if scheduler is finished, false otherwise
     */    
    bool isFinish();

    /**
     * Main function that processes events from the scheduler
     */
    void run();

    /**
     * Gets the elevator ID
     * @return The elevator ID
     */
    int getElevatorId() const { return elevatorId; }

    /**
     * Returns the elevator pointer 
     * @return The elevator pointer
     */
    Elevator* getElevator() { return elevator.get(); }

    /**
     * Destructor for elevator subsystem
     */
    ~ElevatorSubsystem();

    /**
     * Removes elevator
     */
    void removeElevator();

    friend class Elevator;
};

/**
 * Elevator class represents a single elevator car
 * It handles the actual elevator operations like moving, opening doors, etc.
 */
class Elevator {
private:
    ElevatorSubsystem& elevatorSubsystem;
    int elevatorId;
    Event event;
    elevatorState state;
    int curr_floor;
    int passengers;         // Current number of passengers
    int totalPassengers;    // Total passengers served
    std::mutex mtx;
    std::condition_variable cv;

public:
    /**
     * Constructor for the Elevator class
     * 
     * @param elevatorSubsystem The elevator subsystem instance
     * @param id The unique ID of the elevator
     */
    Elevator(ElevatorSubsystem& elevatorSubsystem_a, int id);

    /**
     * Sets the event for the elevator
     * 
     * @param event The event to set
     */
    void setEvent(Event event);

    /**
     * Gets the current floor of the elevator
     * 
     * @return The current floor
     */
    int getCurrentFloor() const { return curr_floor; }

    /**
     * Gets the current state of the elevator
     * 
     * @return The current state
     */
    elevatorState getState() const { return state; }

    /**
     * Gets the current number of passengers
     * 
     * @return The number of passengers
     */
    int getPassengers() const { return passengers; }

    /**
     * Gets the total number of passengers served
     * 
     * @return The total number of passengers served
     */
    int getTotalPassengers() const { return totalPassengers; }

        /**
     * Moves to the destination floor level
     * @param dstn The destination floor level
     * @return If move to was successfull
     */
    bool moveTo(int dstn);

    /**
     * Calculate move time between floors
     * @param dstn The destination floor level
     * @return The time to move between floors
     */
    int moveBetweenFloorsTime(int dstn);

    /**
     * Opens the elevator door
     */
    void openDoors();

    /**
     * Closes the elevator door
     */
    void closeDoors();

    /**
     * Load passengers
     */
    void load();

    /**
     * Unload passengers
     */
    void unload();

    /**
     * Main function that processes events from the scheduler and sends responses back
     */
    void run();

    friend class ElevatorSubsystem;
};

#endif // ELEVATORSUBSYSTEM_H