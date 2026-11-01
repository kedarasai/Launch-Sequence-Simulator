Launch Sequencer Simulator

A command-line program written in C that simulates a rocket launch sequence using a state machine. It runs the pre-launch checks, countdown, ignition, liftoff, stage separation and orbit insertion, and it can also simulate a failure at different points.

This is a simplified learning project. The numbers come from a basic model and are not real flight data.

Features
Runs a full launch sequence from the pad to orbit
Implemented as a state machine with an enum and a switch statement
Lets you inject one fault to see how the system aborts
Shows speed and altitude every 10 seconds of flight
Has a real-time mode with timed delays and a fast mode with no waiting
Works on Windows, Linux and macOS
State machine
IDLE -> PRE-LAUNCH CHECKS -> COUNTDOWN -> IGNITION -> LIFTOFF
     -> STAGE SEPARATION -> ORBIT INSERTION -> MISSION SUCCESS

Any failing state -> MISSION ABORTED

Each state is a separate function that does its work and returns the next state. The function run_mission() keeps calling the function for the current state until the mission ends in success or abort.

State	What happens
IDLE	Vehicle waits on the pad
PRE-LAUNCH CHECKS	Five checks: fuel tank pressure, guidance, communication link, weather, range clearance
COUNTDOWN	Counts from T-10 to T-1
IGNITION	Engine start command, then a thrust check
LIFTOFF	Stage 1 burns for 120 seconds
STAGE SEPARATION	Stage 1 detaches and stage 2 ignites
ORBIT INSERTION	Stage 2 burns until orbital speed of 7800 m/s is reached
MISSION SUCCESS	Orbit achieved
MISSION ABORTED	Reason and status are printed
Fault options
Fault	Where it stops the mission	Result
Low fuel pressure	Pre-launch checks	Launch scrubbed, vehicle safe on the pad
Engine ignition failure	Ignition	Launch scrubbed, vehicle safe on the pad
Stage separation failure	Stage separation	Vehicle lost in flight at T+120 s
The flight model

The model is deliberately simple:

Stage 1 accelerates at 15 m/s^2 for 120 seconds
Stage 2 accelerates at 25 m/s^2 until the speed reaches 7800 m/s
The share of the speed that goes into climbing falls steadily from 100% to 0% over 360 seconds, which imitates the rocket turning from vertical to horizontal
Air drag and changing gravity are not modelled
Build and run

You need a C compiler such as gcc. No extra libraries are needed.

gcc LaunchSequencer.c -o LaunchSequencer.exe
.\LaunchSequencer.exe

On Linux or macOS:

gcc LaunchSequencer.c -o launch
./launch

Real-time mode takes about 35 seconds for a full launch. Fast mode finishes instantly.

Limitations
The flight model uses constant acceleration and is not based on a real rocket
Only one fault can be injected per run
Faults are chosen by the user, not triggered by sensor values
It is a learning project and is not real flight software
Possible improvements
Add random faults with a probability for each check
Add a hold state that pauses the countdown and resumes it
Add more stages or a different launch vehicle profile
Save the flight log to a file
Concepts used

C programming, enums and switch for a state machine, structs, functions that return the next state, cross-platform timed delays, input validation.
