# C++ Elevator Simulator

## Authors
Joseph Marques
Varrahan Uthayan
Sam Michael
Dania Mahmoud
Hamnah Qureshi

## What is this project about:
This project implements an elevator system in C++. It has three subsystems, floor, elevator, and scheduler, which work together to simulate an elevator. 
The elevator system will handle requests and manage the movement between floors based on the request.
The Scheduler will be used to coordinate the movement of cars such that each car carries roughly the same number of passengers as all of the others, 
and so that the waiting time for passengers at floors is minimized.

## Files:
ElevatorSubsystem.cpp: Code for the elevator subsystem logic
ElevatorSubsystem.h: Header file for elevator subsystem class
Event.h: Header file for events for the system
Floor.cpp: Code for floor subsystem logic
Floor.h: Header file for floor class
Main.cpp: Main code for the system
Scheduler.cpp: Code for the elevator scheduler logic
Scheduler.h: Header file for the scheduler class
ElevatorEnums.h: Enums for states
Datagram.h: Class for DatagramSocket, DatagramPacket, and InetAddress
ElevatorInfo.h Class for elevatorInfo that holds real time information about the elevator
schedulerApp: The scheduler app (executable)
SchedulerStateDiagram.png: The state diagram for the scheduler
ElevatorStateDiagram.png: The state diagram for the elevator
SequenceDiagram: The sequence diagram for the elevator system
ClassDiagram: The class diagram for the elevator system
FloorTest.cpp: Test code for floor
SchedulerTest.cpp: Test code for scheduler
ElevatorSubsystemTest.cpp: Test code for elevator system
Test1.txt, Test2.txt, Test3.txt: Test data

## Set up instructions:
1. Launch an editor with C++ installed in your Linux environment (Visual Studios WSL was used)
2. Open the terminal in your Linux environment
3. Ensure all all the test<x>.txt files, main.cpp files are in the same directory
3. In your terminal, cd into the directory the aforementioned files
4. In your terminal, run the following command: apt install g++ if not already installed
5. In your terminal, to compile and run the program, enter the following command:
g++ -o schedulerApp Main.cpp Scheduler.cpp ElevatorSubsystem.cpp Floor.cpp 
./schedulerApp [input.txt file]

To run unit test for example ElevatorTest:
g++ -o elevatorTest tests/FloorElevatorTest.cpp Scheduler.cpp ElevatorSubsystem.cpp Floor.cpp
./elevatorTest

## Must Haves:
C++ complier