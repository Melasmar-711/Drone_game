#include "Generator_functions.h"

int main() {
    // Seed random number generator
    srand(time(NULL));

    // Create FIFO
    int fd_obstacle_generator_to_server = create_and_open_fifo("/tmp/obstacle_generator_to_server", O_WRONLY);

    while (1) {
        int num_obstacles = MAX_OBSTACLES; 
        int obstacles[MAX_OBSTACLES][2];

        // Generate random obstacles within specified boundaries
        for (int i = 0; i < num_obstacles; i++) {
            obstacles[i][0] = rand() % (MAX_X - 2) + 1;  // X coordinate
            obstacles[i][1] = rand() % (MAX_Y - 2) + 1;  // Y coordinate
        }

        // Send the obstacles array to the server
        ssize_t bytes_written = write(fd_obstacle_generator_to_server, obstacles, sizeof(obstacles));
        if (bytes_written == -1) {
            perror("Error writing to FIFO");
            close(fd_obstacle_generator_to_server);
            return 1;
        }

        printf("Generated and sent %d obstacles.\n", num_obstacles);

        // Wait before generating new obstacles
        sleep(10);
    }

    close(fd_obstacle_generator_to_server);
    return 0;
}
