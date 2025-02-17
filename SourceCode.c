// TRAFFIC SIGNAL ALGORITHM
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Vehicle structure to hold vehicle details
typedef struct Vehicle {
    int id;             // Vehicle ID
    int priority;       // 0 for regular, 1 for emergency
    char direction[4];  // "N", "E", "S", "W"
    struct Vehicle *next;
} Vehicle;

// Queue structure for lane queues
typedef struct {
    Vehicle *front;     // Pointer to the first vehicle in the queue
    Vehicle *rear;      // Pointer to the last vehicle in the queue
} Queue;

// Log structure to log processed vehicles
typedef struct Log {
    int id;             // Vehicle ID
    int priority;       // 0 for regular, 1 for emergency
    char direction[4];  // "N", "E", "S", "W"
    struct Log *next;   // Pointer to the next log entry
} Log;

Queue lanes[4];                  // Array of queues for four lanes: 0: North, 1: East, 2: South, 3: West
Log *traffic_log = NULL;         // Head of the log linked list
int emergency_vehicle_count = 0; // Count of emergency vehicles in the queues

// Initialize the lanes as empty queues
void create_lanes() {
    for (int i = 0; i < 4; i++) {
        lanes[i].front = NULL;  // Initialize front of each lane as NULL
        lanes[i].rear = NULL;   // Initialize rear of each lane as NULL
    }
}

// Check if a queue is empty
int is_empty(Queue *queue) {
    return queue->front == NULL;  // Returns 1 if the queue is empty
}

// Get the lane index from the direction
int get_lane_index(char direction[]) {
    if (strcmp(direction, "N") == 0) return 0;  // North lane
    if (strcmp(direction, "E") == 0) return 1;  // East lane
    if (strcmp(direction, "S") == 0) return 2;  // South lane
    if (strcmp(direction, "W") == 0) return 3;  // West lane
    return -1;  // Invalid direction
}

// Check if a vehicle with a given ID exists in any lane
int vehicle_exists(int id) {
    for (int i = 0; i < 4; i++) {  // Iterate over each lane
        Vehicle *temp = lanes[i].front;
        while (temp != NULL) {
            if (temp->id == id) return 1;  // Vehicle exists
            temp = temp->next;
        }
    }
    return 0;  // Vehicle does not exist
}

// Add a vehicle to the appropriate lane
void enqueue_vehicle(int id, int priority, char direction[]) {
    int lane_index = get_lane_index(direction);  // Get the lane index- 0: North, 1: East, 2: South, 3: West
    if (lane_index == -1) {
        printf("\033[1;30mWARNING: Invalid direction. Use N, S, E, W only.\n\033[0m");
        return;
    }

    Vehicle *new_vehicle = (Vehicle *)malloc(sizeof(Vehicle));
    if (new_vehicle == NULL) {
        printf("\033[1;30mMemory allocation failed.\n\033[0m");
        exit(EXIT_FAILURE);
    }

    new_vehicle->id = id;
    new_vehicle->priority = priority;
    strcpy(new_vehicle->direction, direction);
    new_vehicle->next = NULL;

    if (is_empty(&lanes[lane_index])) {
        lanes[lane_index].front = new_vehicle;
        lanes[lane_index].rear = new_vehicle;
    } else {
        lanes[lane_index].rear->next = new_vehicle;
        lanes[lane_index].rear = new_vehicle;
    }

    if (priority == 1) emergency_vehicle_count++;
}

// Create traffic data by enqueuing vehicles
void create() {
    while (1) {
        char input[10];         // Input buffer for vehicle ID
        int id, priority;       // Vehicle ID and priority
        char direction[4];      // Vehicle direction

        printf("Enter Vehicle ID (3-digit), Priority (0 or 1), and Direction (N, E, S, W) or '$' to stop: ");
        scanf("%s", input);

        if (strcmp(input, "$") == 0) break;

        id = atoi(input);       // Convert input to integer (ASCII to integer => atoi)
        scanf("%d %s", &priority, direction);

        for (int i = 0; direction[i] != '\0'; i++)
            direction[i] = toupper(direction[i]);  // Convert direction to uppercase

        if (id < 100 || id > 999) {
            printf("\033[1;30mWARNING: Vehicle ID must be a 3-digit number.\n\033[0m");
            continue;
        }

        if (priority != 0 && priority != 1) {
            printf("\033[1;30mWARNING: Priority must be 0 or 1.\n\033[0m");
            continue;
        }

        if (vehicle_exists(id)) {
            printf("\033[1;30mWARNING: Vehicle with ID %d already exists. Enter a different ID.\n\033[0m", id);
            continue;
        }

        enqueue_vehicle(id, priority, direction);  // Enqueue the vehicle
    }
}

// Display all vehicles in all lanes
void display() {
    const char *directions[4] = {"North", "East", "South", "West"};
    for (int i = 0; i < 4; i++) {
        printf("%s Lane:\n", directions[i]);
        if (is_empty(&lanes[i])) {
            printf("\033[1;30mNO VEHICLES IN THIS LANE.\n\033[0m");
        } else {
            Vehicle *temp = lanes[i].front;
            while (temp != NULL) {
                printf("  Vehicle ID: %d, Priority: %s\n", temp->id, temp->priority ? "Emergency" : "Regular");
                temp = temp->next;
            }
        }
    }
}

// Count the number of vehicles in a given lane
int count_vehicles_in_lane(int lane_index) {
    int count = 0;              // Vehicle count of that particular lane
    Vehicle *temp = lanes[lane_index].front;
    while (temp != NULL) {
        count++;
        temp = temp->next;
    }
    return count;
}

// Add a vehicle to the log of processed vehicles
void add_to_log(int id, int priority, char direction[]) {
    Log *new_log = (Log *)malloc(sizeof(Log));
    if (new_log == NULL) {
        printf("\033[1;30mMemory allocation failed.\n\033[0m");
        exit(EXIT_FAILURE);
    }
    new_log->id = id;
    new_log->priority = priority;
    strcpy(new_log->direction, direction);
    new_log->next = NULL;

    if (traffic_log == NULL) {
        traffic_log = new_log;
    } else {
        Log *temp = traffic_log;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = new_log;
    }
}

// Free the memory allocated for vehicles in a lane
void free_vehicles(Queue *lane) {
    Vehicle *temp;
    while (lane->front != NULL) {
        temp = lane->front;
        lane->front = lane->front->next;
        free(temp);
    }
}

// Process and dequeue emergency vehicles first
void process_emergency_vehicles(const char *directions[]) {
    while (emergency_vehicle_count > 0) {
        int emergency_count[4] = {0};  // Array to store emergency vehicle counts for each lane
        for (int i = 0; i < 4; i++) {
            Vehicle *temp = lanes[i].front;
            while (temp != NULL) {
                if (temp->priority == 1)
                    emergency_count[i]++;
                temp = temp->next;
            }
        }

        int total_emergency_vehicles = 0;  // Total count of emergency vehicles
        for (int i = 0; i < 4; i++)
            total_emergency_vehicles += emergency_count[i];

        if (total_emergency_vehicles == 0) break;

        int lane_order[4] = {0, 1, 2, 3};  // Lane order array for prioritizing lanes using Bubble Sort
        for (int i = 0; i < 4 - 1; i++) {
            for (int j = 0; j < 4 - i - 1; j++) {
                if (emergency_count[lane_order[j]] == emergency_count[lane_order[j + 1]]) {
                    if (lane_order[j] > lane_order[j + 1]) {
                        int temp = lane_order[j];
                        lane_order[j] = lane_order[j + 1];
                        lane_order[j + 1] = temp;
                    }
                } else if (emergency_count[lane_order[j]] < emergency_count[lane_order[j + 1]]) {
                    int temp = lane_order[j];
                    lane_order[j] = lane_order[j + 1];
                    lane_order[j + 1] = temp;
                }
            }
        }

        for (int i = 0; i < 4; i++) {
            int lane_index = lane_order[i];  // Lane index to process
            if (emergency_count[lane_index] > 0) {
                Vehicle *temp = lanes[lane_index].front;
                Vehicle *farthest_emergency = NULL;
                while (temp != NULL) {
                    if (temp->priority == 1)
                        farthest_emergency = temp;
                    temp = temp->next;
                }

                printf("\033[1;32mGreen light for %s lane\n\033[0m", directions[lane_index]);

                while (lanes[lane_index].front != NULL) {
                    Vehicle *dequeue_vehicle = lanes[lane_index].front;
                    lanes[lane_index].front = lanes[lane_index].front->next;

                    printf("  Vehicle ID: %d, Priority: %s\n", dequeue_vehicle->id, dequeue_vehicle->priority ? "Emergency" : "Regular");

                    add_to_log(dequeue_vehicle->id, dequeue_vehicle->priority, dequeue_vehicle->direction);

                    if (dequeue_vehicle->priority == 1)
                        emergency_vehicle_count--;

                    free(dequeue_vehicle);

                    if (dequeue_vehicle == farthest_emergency)
                        break;
                }

                if (lanes[lane_index].front == NULL)
                    lanes[lane_index].rear = NULL;

                break;
            }
        }
    }
}

// Process and dequeue regular vehicles
void process_regular_vehicles(const char *directions[], int total_vehicles) {
    for (int i = 0; i < 4; i++) {
        if (!is_empty(&lanes[i]))
            printf("\033[1;32mGreen light for %s lane\n\033[0m", directions[i]);

        int regular_vehicle_count = 0;  // Count of regular vehicles in the current lane
        Vehicle *temp = lanes[i].front;
        while (temp != NULL) {
            if (temp->priority == 0)
                regular_vehicle_count++;
            temp = temp->next;
        }

        int other_lanes_have_vehicles = 0;  // Check if other lanes have vehicles
        for (int j = 0; j < 4; j++) {
            if (j != i && !is_empty(&lanes[j])) {
                other_lanes_have_vehicles = 1;
                break;
            }
        }

        int allowed_to_dequeue = regular_vehicle_count;
        if (regular_vehicle_count > 1 && other_lanes_have_vehicles)
            allowed_to_dequeue = (int)(regular_vehicle_count * 0.7); // 70% of regular vehicles can be dequeued

        int dequeued_count = 0;  // Count of dequeued vehicles
        while (lanes[i].front != NULL && dequeued_count < allowed_to_dequeue) {
            Vehicle *dequeue_vehicle = lanes[i].front;
            lanes[i].front = lanes[i].front->next;

            printf("  Vehicle ID: %d, Priority: %s\n", dequeue_vehicle->id, dequeue_vehicle->priority ? "Emergency" : "Regular");

            add_to_log(dequeue_vehicle->id, dequeue_vehicle->priority, dequeue_vehicle->direction);

            free(dequeue_vehicle);

            dequeued_count++;
        }
        if (lanes[i].front == NULL) {
            lanes[i].rear = NULL;
        } else {
            printf("\033[1;33mRemaining vehicles in %s lane will be dequeued in the next turn.\n\033[0m", directions[i]);
        }
    }
}

// Manage overall traffic by coordinating emergency and regular vehicle processing
void manage_traffic() {
    int total_vehicles = 0;  // Total count of vehicles in all lanes
    const char *directions[4] = {"North", "East", "South", "West"};
    for (int i = 0; i < 4; i++)
        total_vehicles += count_vehicles_in_lane(i);

    if (total_vehicles == 0) {
        printf("\033[1;30mNO VEHICLES IN ANY LANE.\n\033[0m");
        return;
    } else {
        printf("\033[1;31mALL LANES ARE RED\n\033[0m");
    }

    process_emergency_vehicles(directions);
    process_regular_vehicles(directions, total_vehicles);

    for (int i = 0; i < 4; i++) {
        if (!is_empty(&lanes[i])) {
            printf("\033[1;32mGreen light for %s lane\n\033[0m", directions[i]);

            while (lanes[i].front != NULL) {
                Vehicle *dequeue_vehicle = lanes[i].front;
                lanes[i].front = lanes[i].front->next;

                printf("  Vehicle ID: %d, Priority: %s\n", dequeue_vehicle->id, dequeue_vehicle->priority ? "Emergency" : "Regular");

                add_to_log(dequeue_vehicle->id, dequeue_vehicle->priority, dequeue_vehicle->direction);

                free(dequeue_vehicle);
            }

            if (lanes[i].front == NULL)
                lanes[i].rear = NULL;
        }
    }

    for (int i = 0; i < 4; i++)
        free_vehicles(&lanes[i]);
}

// Print the log of processed vehicles
void print_traffic_log() {
    if (traffic_log == NULL) {
        printf("\033[1;30mNO VEHICLES IN TRAFFIC LOG.\n\033[0m");
    } else {
        Log *temp = traffic_log;
        while (temp != NULL) {
            printf("Vehicle ID: %d, Direction: %s, Priority: %s\n", temp->id, temp->direction, temp->priority ? "Emergency" : "Regular");
            temp = temp->next;
        }
    }
}

// Free the memory allocated for the log
void free_log() {
    Log *temp;
    while (traffic_log != NULL) {
        temp = traffic_log;
        traffic_log = traffic_log->next;
        free(temp);
    }
}

// Main function: provides menu for user interaction
int main() {
    create_lanes();

    while (1) {
        int choice;      // User's menu choice
        char input[10];  // Input buffer for user's choice

        printf("\033[1;34m\nMENU:\n");
        printf("1. Create Traffic\n");
        printf("2. Display All Vehicles\n");
        printf("3. Manage Traffic\n");
        printf("4. Traffic Log\n");
        printf("Press $ to exit.\n");

        printf("Enter your choice: \033[0m");
        scanf("%s", input);
        if (strcmp(input, "$") == 0) {
            printf("\033[1;30mEXITING...\n\n\033[0m");
            break;
        }
        choice = atoi(input);
        switch (choice) {
        case 1:
            create();
            break;
        case 2:
            display();
            break;
        case 3:
            manage_traffic();
            break;
        case 4:
            print_traffic_log();
            break;
        default:
            printf("\033[1;30mInvalid choice. Please try again.\n\033[0m");
            break;
        }
    }

    for (int i = 0; i < 4; i++)
        free_vehicles(&lanes[i]);
    free_log();

    return 0;
}
