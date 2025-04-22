#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Var_str.h"
#include "gen.c"
#include "Alg.c"

// Read map from file and calculate height and width
char** load_map_from_file(const char* filename, int* height, int* width) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("Failed to open file");
        return NULL;
    }

    char line[1024];
    *height = 0;
    *width = 0;

    // Count lines and determine width
    while (fgets(line, sizeof(line), fp)) {
        if (*width == 0) {
            *width = (int)strcspn(line, "\r\n"); // Width is the length of the first line
        }
        (*height)++;
    }

    rewind(fp); // Go back to the beginning of the file to read content

    char** map = (char**)malloc(*height * sizeof(char*));
    for (int i = 0; i < *height; ++i) {
        map[i] = (char*)malloc((*width + 1) * sizeof(char));
        fgets(line, sizeof(line), fp);
        strncpy(map[i], line, *width);
        map[i][*width] = '\0'; // Ensure null-terminated string
    }

    fclose(fp);
    return map;
}

// Save map to file
void save_map_to_file(const char* filename, char** map, int height, int width) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("Failed to open file for writing");
        return;
    }

    for (int i = 0; i < height; ++i) {
        fprintf(fp, "%s\n", map[i]);
    }

    fclose(fp);
}

// Display map on the screen
void display_map(char** map, int height, int width) {
    printf("Map:\n");
    for (int i = 0; i < height; ++i) {
        printf("%s\n", map[i]);
    }
}

// Display 3D projections (XOY, XOZ, YOZ)
void display_3d_projections(char*** map, int height, int width, int depth) {
    printf("Projection XOY:\n");
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            char top = '.';
            for (int z = 0; z < depth; ++z) {
                if (map[y][x][z] != '.') {
                    top = map[y][x][z];
                    break;
                }
            }
            printf("%c", top);
        }
        printf("\n");
    }

    printf("\nProjection XOZ:\n");
    for (int z = 0; z < depth; ++z) {
        for (int x = 0; x < width; ++x) {
            char top = '.';
            for (int y = 0; y < height; ++y) {
                if (map[y][x][z] != '.') {
                    top = map[y][x][z];
                    break;
                }
            }
            printf("%c", top);
        }
        printf("\n");
    }

    printf("\nProjection YOZ:\n");
    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            char top = '.';
            for (int x = 0; x < width; ++x) {
                if (map[y][x][z] != '.') {
                    top = map[y][x][z];
                    break;
                }
            }
            printf("%c", top);
        }
        printf("\n");
    }
}

// Save all 3D projections (XOY, XOZ, YOZ) into a single file
void save_3d_projections_to_file(char*** map, int height, int width, int depth, const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("Failed to open projection file for writing");
        return;
    }

    // Save XOY projection
    fprintf(fp, "Projection XOY:\n");
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            char top = '.';
            for (int z = 0; z < depth; ++z) {
                if (map[y][x][z] != '.') {
                    top = map[y][x][z];
                    break;
                }
            }
            fprintf(fp, "%c", top);
        }
        fprintf(fp, "\n");
    }

    // Save XOZ projection
    fprintf(fp, "\nProjection XOZ:\n");
    for (int z = 0; z < depth; ++z) {
        for (int x = 0; x < width; ++x) {
            char top = '.';
            for (int y = 0; y < height; ++y) {
                if (map[y][x][z] != '.') {
                    top = map[y][x][z];
                    break;
                }
            }
            fprintf(fp, "%c", top);
        }
        fprintf(fp, "\n");
    }

    // Save YOZ projection
    fprintf(fp, "\nProjection YOZ:\n");
    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            char top = '.';
            for (int x = 0; x < width; ++x) {
                if (map[y][x][z] != '.') {
                    top = map[y][x][z];
                    break;
                }
            }
            fprintf(fp, "%c", top);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
    printf("Projections have been saved to '%s'.\n", filename);
}

// Main function
int main() {
    const char* map_file = "map.txt";
    const char* result_file = "map.txt";
    const char* projection_file = "map.txt";

    int version;
    printf("Choose version to run (2D = 1, 3D = 2): ");
    scanf("%d", &version);

    if (version == 1) {
        // Run 2D version
        int height, width, drone_size, num_obstacles;

        // Input map dimensions
        printf("Enter map dimensions (height width): ");
        scanf("%d %d", &height, &width);
        // Determine drone size (no more than 1/6 of the smallest dimension)
        int max_drone_size = (height < width ? height : width) / 6;
        do {
            printf("Enter drone size (maximum %d): ", max_drone_size);
            scanf("%d", &drone_size);
        } while (drone_size > max_drone_size);

        // Input number of obstacles
        printf("Enter number of obstacles: ");
        scanf("%d", &num_obstacles);

        // Generate map and save to file
        char** map = generate_random_map(height, width, drone_size, num_obstacles);
        save_map_to_file(map_file, map, height, width);
        printf("Map has been saved to file '%s'.\n", map_file);

        // Display initial map
        display_map(map, height, width);

        // Ask user if they want to input points A and B from console
        char choice;
        printf("Do you want to input points A and B from console? \nIf not, please ensure valid points are added to the file before confirming. (y/n): ");
        scanf(" %c", &choice);

        int ax, ay, bx, by;

        if (choice == 'y' || choice == 'Y') {
            // Input and validate drone A position
            printf("Enter top-left corner coordinates of drone A (x y): ");
            while (scanf("%d %d", &ax, &ay) != 2 || !can_place_drone(map, height, width, ax, ay, drone_size)) {
                printf("Invalid or occupied position. Enter again (x y): ");
                while (getchar() != '\n'); // Clear input buffer if needed
            }
            place_drone(map, ax, ay, drone_size, 'A');

            // Input and validate drone B position
            printf("Enter top-left corner coordinates of drone B (x y): ");
            while (scanf("%d %d", &bx, &by) != 2 || !can_place_drone(map, height, width, bx, by, drone_size)) {
                printf("Invalid or occupied position. Enter again (x y): ");
                while (getchar() != '\n');
            }
            place_drone(map, bx, by, drone_size, 'B');

        } else {
            // Free initial map
            free_map(map, height);

            // Load map from file
            map = load_map_from_file(map_file, &height, &width);
            if (!map) {
                fprintf(stderr, "Cannot load map from file '%s'.\n", map_file);
                return EXIT_FAILURE;
            }

            // Display loaded map
            display_map(map, height, width);

            // Find drone A position
            if (!find_drone_position(map, height, width, drone_size, 'A', &ax, &ay)) {
                fprintf(stderr, "Drone A not found on the map.\n");
                free_map(map, height);
                return EXIT_FAILURE;
            }

            // Find drone B position
            if (!find_drone_position(map, height, width, drone_size, 'B', &bx, &by)) {
                fprintf(stderr, "Drone B not found on the map.\n");
                free_map(map, height);
                return EXIT_FAILURE;
            }
        }

        // Display map after placing drones A and B
        printf("Map after placing drones A and B:\n");
        display_map(map, height, width);

        // Call A* algorithm to find the path
        astar_find_path(map, height, width, drone_size, ax, ay, bx, by);

        // Save result map to file
        save_map_to_file(result_file, map, height, width);
        printf("Result map has been saved to file '%s'.\n", result_file);

        // Display map after finding the path
        printf("Map after finding the path:\n");
        display_map(map, height, width);

        // Free memory
        free_map(map, height);
    } else if (version == 2) {
        // Run 3D version
        int height, width, depth, drone_size, num_obstacles;

        // Input map dimensions
        printf("Enter map dimensions (height width depth): ");
        scanf("%d %d %d", &height, &width, &depth);
        // Determine drone size (no more than 1/6 of the smallest dimension)
        int max_drone_size = (height < width ? (height < depth ? height : depth) : (width < depth ? width : depth)) / 6;
        do {
            printf("Enter drone size (maximum %d): ", max_drone_size);
            scanf("%d", &drone_size);
        } while (drone_size > max_drone_size);

        // Input number of obstacles
        printf("Enter number of obstacles: ");
        scanf("%d", &num_obstacles);

        // Generate 3D map
        char*** map = generate_random_map_3d(height, width, depth, drone_size, num_obstacles);

        // Display 3D projections
        display_3d_projections(map, height, width, depth);

        // Save 3D projections to a single file
        save_3d_projections_to_file(map, height, width, depth, projection_file);

        // Input and validate drone A position
        int ax, ay, az, bx, by, bz;
        printf("Enter top-left corner coordinates of drone A (x y z): ");
        while (scanf("%d %d %d", &ax, &ay, &az) != 3 || !can_move_3d(map, height, width, depth, ax, ay, az, drone_size)) {
            printf("Invalid or occupied position. Enter again (x y z): ");
            while (getchar() != '\n'); // Clear input buffer if needed
        }
        for (int i = 0; i < drone_size; ++i)
            for (int j = 0; j < drone_size; ++j)
                for (int k = 0; k < drone_size; ++k)
                    map[ay + i][ax + j][az + k] = 'A';

        // Input and validate drone B position
        printf("Enter top-left corner coordinates of drone B (x y z): ");
        while (scanf("%d %d %d", &bx, &by, &bz) != 3 || !can_move_3d(map, height, width, depth, bx, by, bz, drone_size)) {
            printf("Invalid or occupied position. Enter again (x y z): ");
            while (getchar() != '\n');
        }
        for (int i = 0; i < drone_size; ++i)
            for (int j = 0; j < drone_size; ++j)
                for (int k = 0; k < drone_size; ++k)
                    map[by + i][bx + j][bz + k] = 'B';

        // Display projections after placing drones A and B
        printf("Projections after placing drones A and B:\n");
        display_3d_projections(map, height, width, depth);

        // Call A* algorithm to find the path in 3D
        astar_find_path_3d(map, height, width, depth, drone_size, ax, ay, az, bx, by, bz);

        // Display projections after finding the path
        printf("Projections after finding the path:\n");
        display_3d_projections(map, height, width, depth);

        // Save final 3D projections to the same file
        save_3d_projections_to_file(map, height, width, depth, projection_file);

        // Free memory
        free_map_3d(map, height, width);
    } else {
        printf("Invalid choice.\n");
    }

    return 0;
}
