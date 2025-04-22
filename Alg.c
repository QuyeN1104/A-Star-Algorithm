#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "Var_str.h"

// Kiểm tra drone có thể di chuyển vào vị trí (x, y)
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

// Đánh dấu đường đi trên map
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

// Thuật toán A* sử dụng Queue từ Var_str.c
void astar_find_path(char** map, int height, int width, int drone_size, int ax, int ay, int bx, int by) {
    Queue* open = create_queue();
    bool** visited = (bool**)malloc(height * sizeof(bool*));
    for (int i = 0; i < height; ++i) {
        visited[i] = (bool*)calloc(width, sizeof(bool));
    }

    // Tạo node bắt đầu
    Node* start = (Node*)malloc(sizeof(Node));
    start->x = ax;
    start->y = ay;
    start->g_cost = 0;
    start->h_cost = heuristic(ax, ay, bx, by);
    start->f_cost = start->g_cost + start->h_cost;
    start->parent = NULL;

    enqueue_with_priority(open, start);
    visited[ay][ax] = true;

    // 8 hướng di chuyển
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
        printf("Tim thay duong di!\n");
    } else {
        printf("Khong tim thay duong di!\n");
    }

    // Giải phóng bộ nhớ
    for (int i = 0; i < height; ++i) {
        free(visited[i]);
    }
    free(visited);
    free_queue(open);
}
