cmake_minimum_required(VERSION 3.10)

# Project name and version
project(DroneSimulation VERSION 1.0)

# Add subdirectories for each module
add_subdirectory(KeyboardManager)
add_subdirectory(Obstacle_Targets_Generator)
add_subdirectory(GameWindow)
add_subdirectory(DroneDynamicsManager)
add_subdirectory(BlackBoardServer)

