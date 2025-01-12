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

```sh
mkdir -p build
cd build
cmake ..
make

