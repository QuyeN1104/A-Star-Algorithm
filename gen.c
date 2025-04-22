#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

// Initialize empty map
char** init_map(int height, int width) {
    char** map = (char**)malloc(height * sizeof(char*));
    for (int i = 0; i < height; ++i) {
        map[i] = (char*)malloc((width + 1) * sizeof(char));
        memset(map[i], '.', width);
        map[i][width] = '\0';
    }
    return map;
}

// Check if an obstacle can be placed from (x1, y1) to (x2, y2)
bool can_place_obstacle(char** map, int height, int width, int x1, int y1, int x2, int y2, int drone_size) {
    int dx = (x2 > x1) ? 1 : (x2 < x1 ? -1 : 0);
    int dy = (y2 > y1) ? 1 : (y2 < y1 ? -1 : 0);
    int len = (dx != 0) ? abs(x2 - x1) : abs(y2 - y1);
    int x = x1, y = y1;
    for (int i = 0; i <= len; ++i) {
        // Check boundary and minimum distance for drone
        for (int di = -drone_size ; di <= drone_size ; ++di) {
            for (int dj = -drone_size ; dj <= drone_size ; ++dj) {
            int nx = x + di, ny = y + dj;
            if (nx < 0 || nx >= width || ny < 0 || ny >= height || map[ny][nx] == '#') {
                return false;
            }
            }
        }
        x += dx;
        y += dy;
    }
    return true;
}

// Place an obstacle from (x1, y1) to (x2, y2)
void place_obstacle(char** map, int x1, int y1, int x2, int y2) {
    int dx = (x2 > x1) ? 1 : (x2 < x1 ? -1 : 0);
    int dy = (y2 > y1) ? 1 : (y2 < y1 ? -1 : 0);
    int len = (dx != 0) ? abs(x2 - x1) : abs(y2 - y1);
    int x = x1, y = y1;
    for (int i = 0; i <= len; ++i) {
        map[y][x] = '#';
        x += dx;
        y += dy;
    }
}

// Generate random obstacle (only straight lines: horizontal, vertical, diagonal)
bool generate_random_obstacle(char** map, int height, int width, int drone_size) {
    int max_len = (height < width ? height : width) / 3;
    int direction = rand() % 3; // 0: horizontal, 1: vertical, 2: diagonal
    int len = rand() % (max_len - 1) + 2; // Length from 2 to max_len
    int x1, y1, x2, y2;
    int attempts = 0, max_attempts = 100;
    while (attempts++ < max_attempts) {
        if (direction == 0) { // horizontal (straight line)
            y1 = rand() % height;
            x1 = rand() % (width - len);
            x2 = x1 + len;
            y2 = y1;
        } else if (direction == 1) { // vertical (straight line)
            x1 = rand() % width;
            y1 = rand() % (height - len);
            x2 = x1;
            y2 = y1 + len;
        } else { // diagonal (straight line)
            x1 = rand() % (width - len);
            y1 = rand() % (height - len);
            x2 = x1 + len;
            y2 = y1 + len;
        }
        // Do not create complex shaped obstacles, only straight lines
        if (can_place_obstacle(map, height, width, x1, y1, x2, y2, drone_size)) {
            place_obstacle(map, x1, y1, x2, y2);
            return true;
        }
    }
    return false;
}

// Generate random map with desired number of obstacles, ensuring minimum ratio
char** generate_random_map(int height, int width, int drone_size, int num_obstacles) {
    char** map = init_map(height, width);
    int placed = 0;
    srand(time(NULL));
    int max_try = num_obstacles * 10;
    int tries = 0;
    while (placed < num_obstacles && tries < max_try) {
        if (generate_random_obstacle(map, height, width, drone_size)) {
            placed++;
        }
        tries++;
    }

    // If not enough 65% area, add non-overlapping straight line obstacles
    int total_cells = height * width;
    int obstacle_cells = 0;
    for (int i = 0; i < height; ++i)
        for (int j = 0; j < width; ++j)
            if (map[i][j] == '#') obstacle_cells++;

    int max_attempts = 1000; // Limit number of attempts to avoid infinite loop
    int attempts = 0;
    while ((double)obstacle_cells / total_cells < 0.65 && attempts < max_attempts) {
        if (generate_random_obstacle(map, height, width, drone_size)) {
            obstacle_cells = 0; // Recalculate number of cells occupied by obstacles
            for (int i = 0; i < height; ++i)
                for (int j = 0; j < width; ++j)
                    if (map[i][j] == '#') obstacle_cells++;
        }
        attempts++;
    }


    return map;
}

// Free map
void free_map(char** map, int height) {
    for (int i = 0; i < height; ++i) free(map[i]);
    free(map);
}

// Check if a drone can be placed at position (x, y) with size drone_size
bool can_place_drone(char** map, int height, int width, int x, int y, int drone_size) {
    if (x < 0 || y < 0 || x + drone_size > width || y + drone_size > height)
        return false;
    for (int i = 0; i < drone_size; ++i) {
        for (int j = 0; j < drone_size; ++j) {
            if (map[y + i][x + j] != '.')
                return false;
        }
    }
    return true;
}

// Mark drone on map at position (x, y) with character ch (A or B)
void place_drone(char** map, int x, int y, int drone_size, char ch) {
    for (int i = 0; i < drone_size; ++i)
        for (int j = 0; j < drone_size; ++j)
            map[y + i][x + j] = ch;
}

// Initialize empty 3D map
char*** init_map_3d(int height, int width, int depth) {
    char*** map = (char***)malloc(height * sizeof(char**));
    for (int i = 0; i < height; ++i) {
        map[i] = (char**)malloc(width * sizeof(char*));
        for (int j = 0; j < width; ++j) {
            map[i][j] = (char*)malloc((depth + 1) * sizeof(char));
            memset(map[i][j], '.', depth);
            map[i][j][depth] = '\0';
        }
    }
    return map;
}

// Free 3D map
void free_map_3d(char*** map, int height, int width) {
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            free(map[i][j]);
        }
        free(map[i]);
    }
    free(map);
}

// Check if an obstacle can be placed from (x1, y1, z1) to (x2, y2, z2) in 3D space
bool can_place_obstacle_3d(char*** map, int height, int width, int depth, int x1, int y1, int z1, int x2, int y2, int z2, int drone_size) {
    int dx = (x2 > x1) ? 1 : (x2 < x1 ? -1 : 0);
    int dy = (y2 > y1) ? 1 : (y2 < y1 ? -1 : 0);
    int dz = (z2 > z1) ? 1 : (z2 < z1 ? -1 : 0);
    int len = (dx != 0) ? abs(x2 - x1) : ((dy != 0) ? abs(y2 - y1) : abs(z2 - z1));
    int x = x1, y = y1, z = z1;
    for (int i = 0; i <= len; ++i) {
        for (int di = -drone_size; di <= drone_size; ++di) {
            for (int dj = -drone_size; dj <= drone_size; ++dj) {
                for (int dk = -drone_size; dk <= drone_size; ++dk) {
                    int nx = x + di, ny = y + dj, nz = z + dk;
                    if (nx < 0 || ny < 0 || nz < 0 || nx >= width || ny >= height || nz >= depth || map[ny][nx][nz] == '#') {
                        return false;
                    }
                }
            }
        }
        x += dx;
        y += dy;
        z += dz;
    }
    return true;
}

// Place an obstacle from (x1, y1, z1) to (x2, y2, z2) in 3D space
void place_obstacle_3d(char*** map, int x1, int y1, int z1, int x2, int y2, int z2) {
    int dx = (x2 > x1) ? 1 : (x2 < x1 ? -1 : 0);
    int dy = (y2 > y1) ? 1 : (y2 < y1 ? -1 : 0);
    int dz = (z2 > z1) ? 1 : (z2 < z1 ? -1 : 0);
    int len = (dx != 0) ? abs(x2 - x1) : ((dy != 0) ? abs(y2 - y1) : abs(z2 - z1));
    int x = x1, y = y1, z = z1;
    for (int i = 0; i <= len; ++i) {
        map[y][x][z] = '#';
        x += dx;
        y += dy;
        z += dz;
    }
}

// Generate random obstacle in 3D space
bool generate_random_obstacle_3d(char*** map, int height, int width, int depth, int drone_size) {
    int max_len = (height < width ? (height < depth ? height : depth) : (width < depth ? width : depth)) / 3;
    int direction = rand() % 3; // 0: horizontal, 1: vertical, 2: diagonal
    int len = rand() % (max_len - 1) + 2; // Length from 2 to max_len
    int x1, y1, z1, x2, y2, z2;
    int attempts = 0, max_attempts = 100;
    while (attempts++ < max_attempts) {
        if (direction == 0) { // horizontal
            y1 = rand() % height;
            z1 = rand() % depth;
            x1 = rand() % (width - len);
            x2 = x1 + len;
            y2 = y1;
            z2 = z1;
        } else if (direction == 1) { // vertical
            x1 = rand() % width;
            z1 = rand() % depth;
            y1 = rand() % (height - len);
            x2 = x1;
            y2 = y1 + len;
            z2 = z1;
        } else { // diagonal
            x1 = rand() % (width - len);
            y1 = rand() % (height - len);
            z1 = rand() % (depth - len);
            x2 = x1 + len;
            y2 = y1 + len;
            z2 = z1 + len;
        }
        if (can_place_obstacle_3d(map, height, width, depth, x1, y1, z1, x2, y2, z2, drone_size)) {
            place_obstacle_3d(map, x1, y1, z1, x2, y2, z2);
            return true;
        }
    }
    return false;
}

// Generate random 3D map with desired number of obstacles
char*** generate_random_map_3d(int height, int width, int depth, int drone_size, int num_obstacles) {
    char*** map = init_map_3d(height, width, depth);
    int placed = 0;
    srand(time(NULL));
    int max_try = num_obstacles * 10;
    int tries = 0;
    while (placed < num_obstacles && tries < max_try) {
        if (generate_random_obstacle_3d(map, height, width, depth, drone_size)) {
            placed++;
        }
        tries++;
    }

    // If not enough 53% volume, add obstacles
    int total_cells = height * width * depth;
    int obstacle_cells = 0;
    for (int i = 0; i < height; ++i)
        for (int j = 0; j < width; ++j)
            for (int k = 0; k < depth; ++k)
                if (map[i][j][k] == '#') obstacle_cells++;
    int max_attempts = 1000;
    int attempts = 0;
    while ((double)obstacle_cells / total_cells < 0.53 / 3 && attempts < max_attempts) {
        if (generate_random_obstacle_3d(map, height, width, depth, drone_size)) {
            obstacle_cells = 0;
            for (int i = 0; i < height; ++i)
                for (int j = 0; j < width; ++j)
                    for (int k = 0; k < depth; ++k)
                        if (map[i][j][k] == '#') obstacle_cells++;
        }
        attempts++;
    }

    return map;
}

