#!/bin/bash

# Clean up any existing blackboard pipe (if it exists)
if [ -e "blackboard_pipe" ]; then
    echo "Removing existing blackboard_pipe..."
    rm blackboard_pipe
fi

# Define the log directory (update the path if necessary)
LOG_DIR="logs"  # Assuming your log files are in a directory called "logs"

# Clean up log files (delete all .log files in the logs directory)
if [ -d "$LOG_DIR" ]; then
    echo "Cleaning up existing log files in $LOG_DIR..."
    rm -f "$LOG_DIR"/*.log  # Remove all .log files in the logs folder
fi

# Define the build directory
BUILD_DIR="build"

# Clean previous build files if necessary
if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning up previous build..."
    rm -rf "$BUILD_DIR"
fi

# Create a fresh build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure the project with CMake
echo "Configuring the project with CMake..."
cmake ..

# Build the project using the generated Makefiles
echo "Building the project..."
cmake --build .

# Check if the build was successful
if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

# Start the processes (you can specify paths for log files or they will be generated later)
gnome-terminal -- ./BlackBoardServer/BlackBoardServer &
gnome-terminal -- ./DroneDynamicsManager/DroneDynamicsManager &
gnome-terminal --geometry=110x40 -- bash -c "./GameWindow/GameWindow; exec bash" &
gnome-terminal -- ./KeyboardManager/KeyboardManager &
gnome-terminal -- ./Targets_Generator/Targets_Generator &
gnome-terminal -- ./Obstacle_Generator/Obstacle_Generator &
gnome-terminal -- ./WatchDog/WatchDog &

