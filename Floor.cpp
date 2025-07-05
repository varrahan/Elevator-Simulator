#include "Floor.h"
#include <sstream>
#include <iostream>
#include <chrono>
#include <thread>

/**
 * Prints out the responses from the scheduler of floor events
 *
 */
void Floor::handleResponses() {
    while (!done) {
        try {
            //create a datagram packet to receive 
            std::vector<uint8_t> data(100);
            DatagramPacket receivePacket(data, data.size());
            // Block until a datagram is received via receiveSchedulerSocket. 
            receiveSchedulerSocket.receive(receivePacket);

            Event response = Event::bytes_to_event(data);
            std::cout << "Floor received response: Time=" << response.time 
                      << ", Source=" << response.source 
                      << ", Floor Button=" << response.floorButton 
                      << ", Elevator Button=" << response.elevatorButton 
                      << ", Complete=" << (response.isComplete ? "true" : "false") 
                      << ", Fault=" << response.fault << std::endl;
            
            // Only count completions, not intermediate updates
            if (response.isComplete) {
                completedEvents++;
                std::cout << "Event completed! Completed " << completedEvents << " of " << totalEvents << " events" << std::endl;
            }

            if (totalEvents == completedEvents) { break;}

        } catch(const std::runtime_error& e) {
            if (!done){
                std::cerr << "Error in receiving response from the scheduler" << e.what() << std::endl;
                exit(1);
            }
        }       
    }

    std::cout << "Finishing up ..." << std::endl;
    scheduler.finish(); 
}

/**
 * Floor Constructor
 *
 * @param s Reference to the Scheduler object
 * @param fileName Name of the input file containing floor events
 */
Floor::Floor(Scheduler& s, const std::string& fileName) : scheduler(s), inputFileName(fileName), receiveSchedulerSocket(FLOOR_PORT) {
    resThread = std::thread(&Floor::handleResponses, this);
}

/**
 * Destructor for the Floor class
 * 
 */
Floor::~Floor() {
    if (resThread.joinable()) {
        resThread.join();
    }
}

/**
 * Reads floor event data from a file and sends events to the scheduler
 * 
 */
void Floor::run() {

    std::ifstream inFile(inputFileName);
    std::string line;
    int lineNumber = 0;

    while (std::getline(inFile, line)) {
        // Skip first two lines
        lineNumber += 1;
        if (lineNumber <= 2) {
            continue;
        }

        Event event;
        std::stringstream extract(line);
        // Extract values into the event struct
        if (!(extract >> event.time >> event.source >> event.floorButton >> event.elevatorButton >> event.fault)) {
            std::cerr << "Error with line: " << line << std::endl;
        }
        // Mark the event as originating from a floor
        event.isFromFloor = true;
        std::cout << "Floor created event: Time=" << event.time << ", Source=" << event.source << ", Floor Button=" << event.floorButton << ", Elevator Button=" << event.elevatorButton << ", Fault=" << event.fault << std::endl;
        totalEvents++; 
        
        //create a datagram packet with the event information
        std::vector<uint8_t> event_data = event.event_to_bytes();
        DatagramPacket sendPacket(event_data, event_data.size(), InetAddress::getLocalHost(), SCHEDULER_PORT);

        //send it on the sendSchedulerSocket
        try {
            sendSchedulerSocket.send(sendPacket);
        } catch (const std::runtime_error& e) {
            std::cerr << e.what();
            exit(1);
        }
        
        // Add some delay
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}