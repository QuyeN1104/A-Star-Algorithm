#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "Var_str.h"
#include "gen.c"

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
void mark_path(char** map, int** parent_x, int** parent_y, int ax, int ay, int bx, int by, int drone_size) {
    int x = bx, y = by;
    while (!(x == ax && y == ay)) {
        // Chỉ đánh dấu nếu là ô góc trên bên trái và chưa phải là A/B
        if (map[y][x] == '.') {
            for (int i = 0; i < drone_size; ++i)
                for (int j = 0; j < drone_size; ++j)
                    if (map[y + i][x + j] == '.')
                        map[y + i][x + j] = '+';
        }
        int px = parent_x[y][x], py = parent_y[y][x];
        x = px; y = py;
    }
}

// Cấu trúc cho hàng đợi ưu tiên đơn giản (A* open list)
typedef struct PQNode {
    int x, y;
    double f, g;
    struct PQNode* parent;
    struct PQNode* next;
} PQNode;

// Thêm vào hàng đợi ưu tiên theo f tăng dần
void pq_push(PQNode** head, PQNode* node) {
    if (!*head || node->f < (*head)->f) {
        node->next = *head;
        *head = node;
        return;
    }
    PQNode* cur = *head;
    while (cur->next && cur->next->f <= node->f) cur = cur->next;
    node->next = cur->next;
    cur->next = node;
}

// Lấy node có f nhỏ nhất
PQNode* pq_pop(PQNode** head) {
    if (!*head) return NULL;
    PQNode* node = *head;
    *head = node->next;
    return node;
}

// Đọc map từ file map.txt
char** load_map(const char* filename, int* height, int* width) {
    FILE* fp = fopen(filename, "r");
    if (!fp) return NULL;
    char line[1024];
    int rows = 0, cols = 0;
    // Đếm số dòng và số cột
    while (fgets(line, sizeof(line), fp)) {
        if (cols == 0) cols = (int)strcspn(line, "\r\n");
        rows++;
    }
    *height = rows;
    *width = cols;
    rewind(fp);
    char** map = (char**)malloc(rows * sizeof(char*));
    for (int i = 0; i < rows; ++i) {
        map[i] = (char*)malloc((cols + 1) * sizeof(char));
        if (fgets(line, sizeof(line), fp)) {
            strncpy(map[i], line, cols);
            map[i][cols] = '\0';
        } else {
            map[i][0] = '\0';
        }
    }
    fclose(fp);
    return map;
}

// Thuật toán A* biến thể 1, dùng trực tiếp map và tham số
void astar_find_path(char** map, int height, int width, int drone_size, int ax, int ay, int bx, int by) {
    bool** visited = (bool**)malloc(height * sizeof(bool*));
    int** parent_x = (int**)malloc(height * sizeof(int*));
    int** parent_y = (int**)malloc(height * sizeof(int*));
    double** g_cost = (double**)malloc(height * sizeof(double*));
    for (int i = 0; i < height; ++i) {
        visited[i] = (bool*)calloc(width, sizeof(bool));
        parent_x[i] = (int*)malloc(width * sizeof(int));
        parent_y[i] = (int*)malloc(width * sizeof(int));
        g_cost[i] = (double*)malloc(width * sizeof(double));
        for (int j = 0; j < width; ++j) {
            parent_x[i][j] = -1;
            parent_y[i][j] = -1;
            g_cost[i][j] = 1e9;
        }
    }

    PQNode* open = NULL;
    PQNode* start = (PQNode*)malloc(sizeof(PQNode));
    start->x = ax; start->y = ay; start->g = 0; start->f = heuristic(ax, ay, bx, by); start->parent = NULL; start->next = NULL;
    pq_push(&open, start);
    g_cost[ay][ax] = 0;
    visited[ay][ax] = true;

    int dx[] = {-1 * drone_size, -1 * drone_size, -1 * drone_size, 0, 0, 1* drone_size, 1* drone_size, 1* drone_size};
    int dy[] = {-1* drone_size, 0, 1, -1* drone_size, 1* drone_size, -1* drone_size, 0, 1* drone_size};
    bool found = false;

    while (open) {
        PQNode* cur = pq_pop(&open);
        int x = cur->x, y = cur->y;
        if (x == bx && y == by) {
            // Đánh dấu đường đi
            int tx = x, ty = y;
            while (cur->parent) {
                parent_x[ty][tx] = cur->parent->x;
                parent_y[ty][tx] = cur->parent->y;
                tx = cur->parent->x;
                ty = cur->parent->y;
                cur = cur->parent;
            }
            found = true;
            break;
        }
        for (int dir = 0; dir < 8; ++dir) {
            int nx = x + dx[dir], ny = y + dy[dir];
            if (can_move(map, height, width, nx, ny, drone_size)) {
                double ng = g_cost[y][x] + 1.0;
                if (!visited[ny][nx] || ng < g_cost[ny][nx]) {
                    g_cost[ny][nx] = ng;
                    parent_x[ny][nx] = x;
                    parent_y[ny][nx] = y;
                    PQNode* next = (PQNode*)malloc(sizeof(PQNode));
                    next->x = nx; next->y = ny;
                    next->g = ng;
                    next->f = ng + heuristic(nx, ny, bx, by);
                    next->parent = cur;
                    next->next = NULL;
                    pq_push(&open, next);
                    visited[ny][nx] = true;
                }
            }
        }
    }

    if (found) {
        mark_path(map, parent_x, parent_y, ax, ay, bx, by, drone_size);
        printf("Tim thay duong di!\n");
    } else {
        printf("Khong tim thay duong di!\n");
    }

    // In map kết quả
    for (int i = 0; i < height; ++i) {
        printf("%s\n", map[i]);
    }

    // Giải phóng bộ nhớ
    for (int i = 0; i < height; ++i) {
        free(visited[i]);
        free(parent_x[i]);
        free(parent_y[i]);
        free(g_cost[i]);
    }
    free(visited);
    free(parent_x);
    free(parent_y);
    free(g_cost);
    // Giải phóng open list còn lại nếu có
    while (open) {
        PQNode* tmp = open;
        open = open->next;
        free(tmp);
    }
}

// Ví dụ sử dụng: truyền map, height, width, drone_size, ax, ay, bx, by từ gen.c
int main() {
    int height = 20, width = 20, drone_size = 2, num_obstacles = 30;
    char** map = generate_random_map(height, width, drone_size, num_obstacles);

    // In bản đồ để người dùng chọn vị trí
    printf("Ban do ngau nhien:\n");
    for (int i = 0; i < height; ++i) {
        printf("%s\n", map[i]);
    }

    // Nhập và kiểm tra vị trí drone A
    int ax, ay;
    printf("Nhap toa do goc trai tren cua drone A (theo dinh dang x y): ");
    while (scanf("%d %d", &ax, &ay) != 2 || !can_place_drone(map, height, width, ax, ay, drone_size)) {
        printf("Vi tri khong hop le hoac bi chiem. Nhap lai x y: ");
        while (getchar() != '\n'); // Xóa bộ đệm nhập nếu cần
    }
    place_drone(map, ax, ay, drone_size, 'A');

    // Nhập và kiểm tra vị trí drone B
    int bx, by;
    printf("Nhap toa do goc trai tren cua drone B (theo dinh dang x y): ");
    while (scanf("%d %d", &bx, &by) != 2 || !can_place_drone(map, height, width, bx, by, drone_size)) {
        printf("Vi tri khong hop le hoac bi chiem. Nhap lai x y: ");
        while (getchar() != '\n');
    }
    place_drone(map, bx, by, drone_size, 'B');

    // In lại bản đồ với drone A, B
    printf("Ban do sau khi dat drone A va B:\n");
    for (int i = 0; i < height; ++i) {
        printf("%s\n", map[i]);
    }

    // Gọi thuật toán A* để tìm đường đi
    astar_find_path(map, height, width, drone_size, ax, ay, bx, by);

    // Giải phóng bộ nhớ
    free_map(map, height);
    return 0;
}
