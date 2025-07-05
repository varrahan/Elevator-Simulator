#ifndef FLOOR_H
#define FLOOR_H

#include <atomic>
#include <string>
#include <thread>
#include <fstream>
#include <iostream>
#include "Scheduler.h"

class Floor {
private:
    Scheduler& scheduler;
    std::string inputFileName;
    std::atomic<bool> done{false};
    std::thread resThread;
    std::atomic<int> totalEvents{0};    // Total events received
    std::atomic<int> completedEvents{0}; // Processed events count
    DatagramSocket sendSchedulerSocket;
    DatagramSocket receiveSchedulerSocket;

public:
    /**
     * Constructor for the Floor class.
     * 
     * @param s The scheduler instance.
     * @param fileName The input file containing event data.
     */
    Floor(Scheduler& s, const std::string& fileName);

    /**
     * Handle the responses from the scheduler and print them to the console.
     */
    void handleResponses();
    
    /**
     * Destructor for the Floor class, ensures the response thread is joined.
     */
    ~Floor();

    /**
     * Main function that reads events from the input file and sends th
    */
   void run();

    int getCompletedEvents() { return completedEvents.load(); }

    int getTotalEvents() { return totalEvents.load(); }
};
#endif