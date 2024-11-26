cmake_minimum_required(VERSION 3.10)
project(KeyboardManager)


# Add ncurses library
find_package(Curses REQUIRED)

# Add executable
add_executable(KeyboardManager KeyboardManager.c)

# Link ncurses
target_link_libraries(KeyboardManager ${CURSES_LIBRARIES})



