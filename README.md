# DroneSimulation

DroneSimulation is a modular simulation project for drones, consisting of various components such as a game window, keyboard manager, target generator, obstacle generator, drone dynamics manager, blackboard server, and watchdog.

## Project Structure

### Communication

The DroneSimulation project uses inter-process communication (IPC) to enable different modules to interact with each other. The primary methods of communication include FIFOs (named pipes), signals, and the `select` method.

### IPC Mechanisms Used in This Project

- **FIFOs (Named Pipes)**: Used for unidirectional or bidirectional communication between processes. They allow different modules to send and receive data in a synchronized manner.
- **Signals**: Employed for sending simple notifications or commands between processes. Signals are used to handle events such as starting, stopping, or pausing the simulation.
- **Select Method**: Utilized to monitor multiple file descriptors, allowing the system to manage multiple IPC channels efficiently. This method helps in handling multiple inputs and outputs without blocking the processes.

By using these IPC mechanisms, the DroneSimulation project achieves modularity and scalability, allowing for easy addition or modification of components without disrupting the overall system.

### Modules

- **BlackBoardServer**: Manages the blackboard server for inter-process communication.
- **DroneDynamicsManager**: Handles the dynamics and physics of the drones.
- **GameWindow**: Provides the game window interface using the ncurses library.
- **KeyboardManager**: Manages keyboard inputs.
- **Obstacle_Generator**: Generates obstacles in the simulation.
- **Targets_Generator**: Generates targets for the drones.
- **WatchDog**: Monitors the system to ensure all components are running correctly.

## Building the Project

To build the project, you need to have CMake installed. Follow these steps:

1. Navigate to the root directory of the project.
2. Run the following commands:

```bash
./run.sh
```





## Components
### **1st module** in subscriber mode
- **BlackBoardServer**: Manages the blackboard server for inter-process communication.
- **DroneDynamicsManager**: Handles the dynamics and physics of the drones.
- **GameWindow**: Provides the game window interface using the ncurses library.
- **KeyboardManager**: Manages keyboard inputs.
- **Obstacle_Generator**: sends  obstacles in to the server 
- **Targets_Generator**: sends  targets in to the server 
- **WatchDog**: Monitors the system to ensure all components are running correctly.



## Test Runs
Here are some videos demonstrating the system in action:

**Basic Movement Demo and signals**  
[![Basic Movement Demo and signals](https://img.youtube.com/vi/xFOQFlg-NGg/0.jpg)](https://youtu.be/xFOQFlg-NGg)

**Test reset and changing parameters**  
[![Test reset and changing parameters](https://img.youtube.com/vi/iVQi3v4p6Ro/0.jpg)](https://youtu.be/iVQi3v4p6Ro?si=XBkHi97a9jSCfM3O)


## Project Structure

example of project structure
```
Drone_game_part2/ 

|── CMakeLists.txt
├── BlackBoardServer/
│   ├── server.h
│   ├── server.c
│   ├── BlackBoardServer.c
|   ├──CMakeLists.txt
│   
|
|
|


each module has the same 
```

## Communication Diagram

Below is a text-based diagram illustrating the communication between different nodes in the drone navigation system:

```
 
+--------------------------------------------------------+  
|                                                        |
|                                                        |
|                                                        |
|            +----------------------+                    |
|            |                      |                    |
|            |WatchDog Monitoring   |                    |
|            |                      |                    |
|            +----------------------+                    |
|                                                        |
|                                                        |
|   +---------+---------+       +---------+---------+    |
|   |                   |       |                   |    |
|   |Obstacle_Generator |       |Targets_Generator  |    |
|   |                   |       |                   |    |
|   +---------+---------+       +---------+---------+    |
|           FI|FO                       FI|FO            |
|             v                           v              |
|        =---------------------------------------+       |
|        |                                       |       +------+---------------------|
|        |                                       |    FIFO      |                     |
|        |                                       |------------> |    GameWindow       |
|        |           BlackBoardServer            |              |                     |
|        |                                       |       +------+---------------------+
|        +-------- _-----------------------------+       |
|          FI|FO  / \                    |               |
|            v   FI|FO                 FI|FO             |
|   +---------------------+     +-------------------+    |
|   |                     |     |                   |    |
|   |DroneDynamicsManager |     | Keyboard          |    |
|   |                     |     |                   |    |
|   +---------+-----------+     +---------+---------+    |
|                                                        |
+--------------------------------------------------------+

```

This diagram shows the flow of data from the generators to the publishers, then to the BlackBoardServer, and finally to the various managers and interfaces.

