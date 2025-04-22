#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "Var_str.h"

// Check if the drone can move to position (x, y)
bool can_move(char** map, int height, int width, int x, int y, int drone_size) {
    if (x < 0 || y < 0 || x + drone_size > width || y + drone_size > height)
        return false;
    for (int i = 0; i < drone_size; ++i)
        for (int j = 0; j < drone_size; ++j)
            if (map[y + i][x + j] == '#')
                return false;
    return true;
}

// Heuristic Euclid
double heuristic(int x, int y, int bx, int by) {
    return sqrt((x - bx) * (x - bx) + (y - by) * (y - by));
}

// Mark the path on the map
void mark_path(char** map, Node* goal, int drone_size) {
    Node* current = goal;
    while (current) {
        int x = current->x, y = current->y;
        for (int i = 0; i < drone_size; ++i)
            for (int j = 0; j < drone_size; ++j)
                if (map[y + i][x + j] == '.')
                    map[y + i][x + j] = '+';
        current = current->parent;
    }
}

// Find the top-left position of drone A or B on the map
bool find_drone_position(char** map, int height, int width, int drone_size, char drone_char, int* out_x, int* out_y) {
    for (int y = 0; y <= height - drone_size; ++y) {
        for (int x = 0; x <= width - drone_size; ++x) {
            bool match = true;
            for (int i = 0; i < drone_size && match; ++i) {
                for (int j = 0; j < drone_size && match; ++j) {
                    if (map[y + i][x + j] != drone_char) {
                        match = false;
                    }
                }
            }
            if (match) {
                *out_x = x;
                *out_y = y;
                return true;
            }
        }
    }
    return false;
}

// A* algorithm using Queue from Var_str.c
void astar_find_path(char** map, int height, int width, int drone_size, int ax, int ay, int bx, int by) {
    Queue* open = create_queue();
    bool** visited = (bool**)malloc(height * sizeof(bool*));
    for (int i = 0; i < height; ++i) {
        visited[i] = (bool*)calloc(width, sizeof(bool));
    }

    // Create start node
    Node* start = (Node*)malloc(sizeof(Node));
    start->x = ax;
    start->y = ay;
    start->g_cost = 0;
    start->h_cost = heuristic(ax, ay, bx, by);
    start->f_cost = start->g_cost + start->h_cost;
    start->parent = NULL;

    enqueue_with_priority(open, start);
    visited[ay][ax] = true;

    // 8 directions of movement
    int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    bool found = false;
    Node* goal = NULL;

    while (!is_queue_empty(open)) {
        Node* current = dequeue_min(open);
        if (current->x == bx && current->y == by) {
            goal = current;
            found = true;
            break;
        }

        for (int dir = 0; dir < 8; ++dir) {
            int nx = current->x + dx[dir];
            int ny = current->y + dy[dir];
            if (can_move(map, height, width, nx, ny, drone_size) && !visited[ny][nx]) {
                visited[ny][nx] = true;

                Node* neighbor = (Node*)malloc(sizeof(Node));
                neighbor->x = nx;
                neighbor->y = ny;
                neighbor->g_cost = current->g_cost + sqrt(dx[dir] * dx[dir] + dy[dir] * dy[dir]);
                neighbor->h_cost = heuristic(nx, ny, bx, by);
                neighbor->f_cost = neighbor->g_cost + neighbor->h_cost;
                neighbor->parent = current;

                enqueue_with_priority(open, neighbor);
            }
        }
    }

    if (found) {
        mark_path(map, goal, drone_size);
        printf("Path found!\n");
    } else {
        printf("Path not found!\n");
    }

    // Free memory
    for (int i = 0; i < height; ++i) {
        free(visited[i]);
    }
    free(visited);
    free_queue(open);
}

// Check if the drone can move to position (x, y, z) in 3D space
bool can_move_3d(char*** map, int height, int width, int depth, int x, int y, int z, int drone_size) {
    if (x < 0 || y < 0 || z < 0 || x + drone_size > width || y + drone_size > height || z + drone_size > depth)
        return false;
    for (int i = 0; i < drone_size; ++i)
        for (int j = 0; j < drone_size; ++j)
            for (int k = 0; k < drone_size; ++k)
                if (map[y + i][x + j][z + k] == '#')
                    return false;
    return true;
}

// Heuristic Euclid in 3D space
double heuristic_3d(int x, int y, int z, int bx, int by, int bz) {
    return sqrt((x - bx) * (x - bx) + (y - by) * (y - by) + (z - bz) * (z - bz));
}

// Mark the path on the 3D map
void mark_path_3d(char*** map, Node* goal, int drone_size) {
    Node* current = goal;
    while (current) {
        int x = current->x, y = current->y, z = current->z;
        for (int i = 0; i < drone_size; ++i)
            for (int j = 0; j < drone_size; ++j)
                for (int k = 0; k < drone_size; ++k)
                    if (map[y + i][x + j][z + k] == '.')
                        map[y + i][x + j][z + k] = '+';
        current = current->parent;
    }
}

// A* algorithm in 3D space
void astar_find_path_3d(char*** map, int height, int width, int depth, int drone_size, int ax, int ay, int az, int bx, int by, int bz) {
    Queue* open = create_queue();
    bool*** visited = (bool***)malloc(height * sizeof(bool**));
    for (int i = 0; i < height; ++i) {
        visited[i] = (bool**)malloc(width * sizeof(bool*));
        for (int j = 0; j < width; ++j) {
            visited[i][j] = (bool*)calloc(depth, sizeof(bool));
        }
    }

    // Create start node
    Node* start = (Node*)malloc(sizeof(Node));
    start->x = ax;
    start->y = ay;
    start->z = az;
    start->g_cost = 0;
    start->h_cost = heuristic_3d(ax, ay, az, bx, by, bz);
    start->f_cost = start->g_cost + start->h_cost;
    start->parent = NULL;

    enqueue_with_priority(open, start);
    visited[ay][ax][az] = true;

    // 26 directions of movement in 3D space
    int dx[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    int dy[] = {-1, -1, -1, 0, 0, 0, 1, 1, 1, -1, -1, 0, 0, 1, 1, -1, -1, -1, 0, 0, 0, 1, 1, 1, 1, 1};
    int dz[] = {-1, 0, 1, -1, 0, 1, -1, 0, 1, -1, 1, -1, 1, -1, 1, -1, 0, 1, -1, 0, 1, -1, 0, 1, -1, 1};
    bool found = false;
    Node* goal = NULL;

    while (!is_queue_empty(open)) {
        Node* current = dequeue_min(open);
        if (current->x == bx && current->y == by && current->z == bz) {
            goal = current;
            found = true;
            break;
        }

        for (int dir = 0; dir < 26; ++dir) {
            int nx = current->x + dx[dir];
            int ny = current->y + dy[dir];
            int nz = current->z + dz[dir];
            if (can_move_3d(map, height, width, depth, nx, ny, nz, drone_size) && !visited[ny][nx][nz]) {
                visited[ny][nx][nz] = true;

                Node* neighbor = (Node*)malloc(sizeof(Node));
                neighbor->x = nx;
                neighbor->y = ny;
                neighbor->z = nz;
                neighbor->g_cost = current->g_cost + sqrt(dx[dir] * dx[dir] + dy[dir] * dy[dir] + dz[dir] * dz[dir]);
                neighbor->h_cost = heuristic_3d(nx, ny, nz, bx, by, bz);
                neighbor->f_cost = neighbor->g_cost + neighbor->h_cost;
                neighbor->parent = current;

                enqueue_with_priority(open, neighbor);
            }
        }
    }

    if (found) {
        mark_path_3d(map, goal, drone_size);
        printf("Path found in 3D!\n");
    } else {
        printf("Path not found in 3D!\n");
    }

    // Free memory
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            free(visited[i][j]);
        }
        free(visited[i]);
    }
    free(visited);
    free_queue(open);
}
