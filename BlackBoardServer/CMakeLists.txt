# Add the executable for blackboard_server



cmake_minimum_required(VERSION 3.10)
project(BlackBoardServer)

# Add executable
add_executable(BlackBoardServer BlackBoardServer.c)

# Link libraries if necessary
target_link_libraries(BlackBoardServer ncurses)
