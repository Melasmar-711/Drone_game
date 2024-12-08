


konsole -e ./build/BlackBoardServer/BlackBoardServer &
konsole -e ./build/DroneDynamicsManager/DroneDynamicsManager &
konsole -p 'TerminalColumns=400' -p 'TerminalRows=50' -geometry +60-60 -e  ./build/GameWindow/GameWindow &
konsole -e ./build/KeyboardManager/KeyboardManager &
konsole -e ./build/Targets_Generator/Targets_Generator &
konsole -e ./build/Obstacle_Generator/Obstacle_Generator &

#gnome-terminal -- ./build/BlackBoardServer/BlackBoardServer &
#gnome-terminal -- ./build/DroneDynamicsManager/DroneDynamicsManager &
#gnome-terminal --geometry=110x40 -- bash -c "./build/GameWindow/GameWindow; exec bash" &
#gnome-terminal -- ./build/KeyboardManager/KeyboardManager &
#gnome-terminal -- ./build/Targets_Generator/Targets_Generator &
#gnome-terminal -- ./build/Obstacle_Generator/Obstacle_Generator &
