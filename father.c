#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<signal.h>
#include<time.h>

#define MAX_PROCESSES 100
#define LOG_PATH_LEN 256

const char *processes[] = {
    "./BlackBoardServer/BlackBoardServer",
    "./DroneDynamicsManager/DroneDynamicsManager",
    "./Targets_Generator/Targets_Generator",
    "./Obstacle_Generator/Obstacle_Generator"
};

#define GAME_WINDOW "./GameWindow/GameWindow"
#define GAME_WINDOW_GEOMETRY "100x30+100+100"

int main() {
    pid_t pids[6];
    char pid_args[400] = "";

    // Start processes in separate terminals
    for (int i = 0; i < 4; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            execlp("gnome-terminal", "gnome-terminal", "--", processes[i], NULL);
            perror("execlp failed");
            exit(1);
        } else if (pid > 0) {
            pids[i] = pid;
            char pid_str[16];
            sprintf(pid_str, "%d ", pid);
            strcat(pid_args, pid_str);
        } else {
            perror("fork failed");
            exit(1);
        }
    }

    // Start GameWindow with specified size
    pid_t game_window_pid = fork();
    if (game_window_pid == 0) {
        execlp("gnome-terminal", "gnome-terminal", "--geometry", GAME_WINDOW_GEOMETRY, "--", GAME_WINDOW, NULL);
        perror("execlp failed");
        exit(1);
    }

    sleep(2); // Give processes time to initialize


    

    printf("PIDs: %s\n", pid_args);
    // Start KeyboardManager
    pid_t keyboard_pid = fork();
    if (keyboard_pid == 0) {
        printf("Starting KeyboardManager with args: %s\n", pid_args);
        execlp("gnome-terminal", "gnome-terminal", "--", "./KeyboardManager/KeyboardManager", pid_args, NULL);

        perror("execlp failed");
        exit(1);
    }

    sleep(2); // Allow KeyboardManager to initialize

    // Start WatchDog
    char watchdog_args[300];
    sprintf(watchdog_args, "%s%d", pid_args, keyboard_pid);
    pid_t watchdog_pid = fork();
    if (watchdog_pid == 0) {
        execlp("gnome-terminal", "gnome-terminal", "--", "./WatchDog/WatchDog", watchdog_args, NULL);
        perror("execlp failed");
        exit(1);
    }




}
