#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <sstream>
#include <iostream>
#include "Datagram.h"
#include "ElevatorEnums.h"

/**
 * Structure to represent elevator events.
 * This includes the time of the event, the source (floor or elevator), 
 * the button pressed, and whether the event is from the floor or the elevator.
 */
struct Event {
    std::string time;       // time of the event
    std::string source;     // floor or elevator number
    std::string floorButton; // command type (UP or DOWN)
    int elevatorButton;     // destination floor button pressed in elevator
    bool isFromFloor;       // true if event is from floor, false if from elevator
    int assignedElevator;   // assigned elevator ID
    int currentFloor;       // current floor of the elevator
    int riders;             // number of riders in the elevator
    bool isComplete;        // indicates if the request has been completed
    int fault;              // fault in system

    // Default constructor
    Event() : time(""), source(""), floorButton(""), elevatorButton(0), 
              isFromFloor(false), assignedElevator(0), currentFloor(0),
              riders(0), isComplete(false), fault(0) {}
    
    // Constructor with parameters
    Event(const std::string& t, const std::string& src, const std::string& fb, 
          int eb, bool isFF, int ae = 0, int cf = 0, int r = 0, bool ic = false, int f = 0) 
        : time(t), source(src), floorButton(fb), elevatorButton(eb),
          isFromFloor(isFF), assignedElevator(ae), currentFloor(cf),
          riders(r), isComplete(ic), fault(f) {}

    //Turns an event into a vector of bytes so that it can be sent via UDP to other processes
    std::vector<uint8_t> event_to_bytes() const {
        // Create the data string including isComplete
        std::string data = time + "," + source + "," + floorButton + "," 
                         + std::to_string(elevatorButton) + ","
                         + (isFromFloor ? "1" : "0") + ","
                         + std::to_string(assignedElevator) + ","
                         + std::to_string(currentFloor) + ","
                         + std::to_string(riders) + ","
                         + (isComplete ? "1" : "0") + ","
                         + std::to_string(fault);
                         
        std::vector<uint8_t> result(data.begin(), data.end());
        return result;
    }
    
    static Event bytes_to_event(const std::vector<uint8_t>& data) {
        Event event; // event to be returned
        
        // Convert the data to a string, trimming any null bytes or extra whitespace
        std::string str;
        for (size_t i = 0; i < data.size(); i++) {
            if (data[i] == 0) break; // Stop at null terminator
            str.push_back(static_cast<char>(data[i]));
        }
        
        // Trim trailing whitespace
        while (!str.empty() && (str.back() == ' ' || str.back() == '\0')) {
            str.pop_back();
        }
        
        std::stringstream stream(str); // make a string stream for parsing
    
        // Parse the time, source and floorButton
        std::getline(stream, event.time, ',');
        std::getline(stream, event.source, ',');
        std::getline(stream, event.floorButton, ',');
        
        // Parse the elevatorBtn and change it into an int
        std::string elevatorBtn;
        std::getline(stream, elevatorBtn, ',');
        event.elevatorButton = std::stoi(elevatorBtn);
        
        // Parse the isFromFloor field
        std::string isFromFloorStr;
        std::getline(stream, isFromFloorStr, ',');
        
        // Trim any spaces
        while (!isFromFloorStr.empty() && isFromFloorStr[0] == ' ')
            isFromFloorStr.erase(0, 1);
        while (!isFromFloorStr.empty() && isFromFloorStr[isFromFloorStr.length()-1] == ' ')
            isFromFloorStr.erase(isFromFloorStr.length()-1, 1);
        
        // Determine if event is from floor or elevator
        if (event.source.find("Elevator") == std::string::npos) {
            event.isFromFloor = true;
        } else {
            event.isFromFloor = false;
        }
        
        // Parse assigned elevator ID
        std::string assignedElevatorStr;
        if (std::getline(stream, assignedElevatorStr, ',')) {
            event.assignedElevator = std::stoi(assignedElevatorStr);
        }
        
        // Parse current floor
        std::string currentFloorStr;
        if (std::getline(stream, currentFloorStr, ',')) {
            event.currentFloor = std::stoi(currentFloorStr);
        }
        
        // Parse riders count
        std::string ridersStr;
        if (std::getline(stream, ridersStr, ',')) {
            event.riders = std::stoi(ridersStr);
        }
        
        // Parse isComplete flag
        std::string isCompleteStr;
        if (std::getline(stream, isCompleteStr, ',')) {
            event.isComplete = (isCompleteStr == "1");
        }

        // Parse fault
        std::string faultStr;
        if (std::getline(stream, faultStr)) {
            event.fault = std::stoi(faultStr);
        }
        
        return event;
    }
};

#endif