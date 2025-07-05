# C++ Elevator Simulator

## Authors
- Joseph Marques
- Varrahan Uthayan
- Sam Michael
- Dania Mahmoud
- Hamnah Qureshi

## Licensing

Copyright © 2025 Carleton University. All rights reserved.

This work and all associated materials are the intellectual property of Carleton University and are protected under Canadian copyright law.

Permission is hereby granted, free of charge, to any person obtaining a copy of this work and associated documentation files (the “Materials”), to use, reproduce, and distribute the Materials for **non-commercial educational and research purposes only**, subject to the following conditions:

- The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Materials.
- The name of Carleton University and its logos shall not be used in advertising or publicity pertaining to the distribution of the Materials without prior written permission.
- Users must ensure that proper attribution is given and that the Materials are not presented as their own original work. Any unauthorized use that constitutes plagiarism or academic misconduct is strictly prohibited.

THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL CARLETON UNIVERSITY BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.

## What is this project about:
This project implements an elevator system in C++. It has three subsystems, floor, elevator, and scheduler, which work together to simulate an elevator. 
The elevator system will handle requests and manage the movement between floors based on the request.
The Scheduler will be used to coordinate the movement of cars such that each car carries roughly the same number of passengers as all of the others, 
and so that the waiting time for passengers at floors is minimized.

## Files:
- ElevatorSubsystem.cpp: Code for the elevator subsystem logic
- ElevatorSubsystem.h: Header file for elevator subsystem class
- Event.h: Header file for events for the system
- Floor.cpp: Code for floor subsystem logic
- Floor.h: Header file for floor class
- Main.cpp: Main code for the system
- Scheduler.cpp: Code for the elevator scheduler logic
- Scheduler.h: Header file for the scheduler class
- ElevatorEnums.h: Enums for states
- Datagram.h: Class for DatagramSocket, DatagramPacket, and InetAddress
- ElevatorInfo.h Class for elevatorInfo that holds real time information about the elevator

- tests/FloorTest.cpp: Test code for floor
- tests/SchedulerTest.cpp: Test code for scheduler
- tests/ElevatorSubsystemTest.cpp: Test code for elevator system

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
