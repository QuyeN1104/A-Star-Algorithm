#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h> // For memset
#include "Var_str.h"

// --- Khai báo biến toàn cục hoặc cấu trúc để lưu map ---
char** map_grid = NULL;
int map_rows = 0;
int map_cols = 0;
Node* start_node = NULL;
Node* end_node = NULL;
Node*** node_details = NULL; // Mảng 2D chứa con trỏ tới Node cho mỗi ô
bool** closed_list = NULL;   // Mảng 2D đánh dấu các ô đã khám phá

// --- Hàm tiện ích ---

// Kiểm tra tọa độ có hợp lệ trong bản đồ không
bool is_valid(int r, int c) {
    return (r >= 0) && (r < map_rows) && (c >= 0) && (c < map_cols);
}

// Kiểm tra ô có bị chặn (là chướng ngại vật) không
bool is_unblocked(int r, int c) {
    if (!is_valid(r, c)) return false;
    return map_grid[r][c] != '#';
}

// Tính heuristic (Khoảng cách Euclid)
double calculate_h_cost(int r, int c, const Node* dest) {
    return sqrt(pow(r - dest->x, 2) + pow(c - dest->y, 2));
}

// Giải phóng bộ nhớ đã cấp phát cho bản đồ và các cấu trúc liên quan
void cleanup() {
    if (map_grid) {
        for (int i = 0; i < map_rows; ++i) {
            free(map_grid[i]);
            if (node_details) free(node_details[i]);
            if (closed_list) free(closed_list[i]);
        }
        free(map_grid);
        free(node_details);
        free(closed_list);
        map_grid = NULL;
        node_details = NULL;
        closed_list = NULL;
    }
     // start_node và end_node được quản lý trong node_details, không cần free riêng
     // Nếu cấp phát riêng thì cần free ở đây:
     // free(start_node);
     // free(end_node);
}


// Đọc bản đồ từ file
bool load_map(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("Error opening map file");
        return false;
    }

    // Xác định kích thước bản đồ
    char buffer[1024]; // Giả sử mỗi dòng không quá 1023 ký tự
    map_rows = 0;
    map_cols = 0;
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if (map_rows == 0) {
            // Tính chiều rộng từ dòng đầu tiên (loại bỏ ký tự newline nếu có)
             map_cols = strlen(buffer);
             if (map_cols > 0 && buffer[map_cols - 1] == '\n') {
                 buffer[map_cols - 1] = '\0'; // Loại bỏ newline
                 map_cols--; // Giảm chiều rộng
             }
              if (map_cols > 0 && buffer[map_cols - 1] == '\r') { // Xử lý CRLF
                 buffer[map_cols - 1] = '\0';
                 map_cols--;
             }
        }
        map_rows++;
    }

    if (map_rows == 0 || map_cols == 0) {
        fprintf(stderr, "Map file is empty or invalid.\n");
        fclose(fp);
        return false;
    }

     printf("Map dimensions: %d rows x %d cols\n", map_rows, map_cols);


    // Cấp phát bộ nhớ cho bản đồ và các cấu trúc phụ trợ
    map_grid = (char**)malloc(map_rows * sizeof(char*));
    node_details = (Node***)malloc(map_rows * sizeof(Node**));
    closed_list = (bool**)malloc(map_rows * sizeof(bool*));
    if (!map_grid || !node_details || !closed_list) {
        perror("Failed to allocate memory for map structures");
        fclose(fp);
        cleanup(); // Giải phóng những gì đã cấp phát
        return false;
    }

    for (int i = 0; i < map_rows; ++i) {
        map_grid[i] = (char*)malloc((map_cols + 1) * sizeof(char)); // +1 cho null terminator
        node_details[i] = (Node**)malloc(map_cols * sizeof(Node*));
        closed_list[i] = (bool*)malloc(map_cols * sizeof(bool));
        if (!map_grid[i] || !node_details[i] || !closed_list[i]) {
             perror("Failed to allocate memory for map row structures");
             map_rows = i + 1; // Để cleanup giải phóng đúng phần đã cấp phát
             fclose(fp);
             cleanup();
             return false;
        }
        // Khởi tạo node_details và closed_list
        memset(node_details[i], 0, map_cols * sizeof(Node*)); // Khởi tạo con trỏ là NULL
        memset(closed_list[i], false, map_cols * sizeof(bool));
    }


    // Đọc lại file để điền dữ liệu vào bản đồ
    rewind(fp);
    int current_row = 0;
    start_node = NULL; // Reset đề phòng gọi lại hàm load_map
    end_node = NULL;
    bool found_A = false, found_B = false;

    while (fgets(buffer, sizeof(buffer), fp) != NULL && current_row < map_rows) {
         // Xử lý chiều dài và ký tự cuối dòng như trên
         int current_len = strlen(buffer);
         if (current_len > 0 && buffer[current_len - 1] == '\n') buffer[--current_len] = '\0';
         if (current_len > 0 && buffer[current_len - 1] == '\r') buffer[--current_len] = '\0';

         if (current_len != map_cols) {
            fprintf(stderr, "Error: Row %d has inconsistent length (%d vs %d).\n", current_row, current_len, map_cols);
            fclose(fp);
            cleanup();
            return false;
         }

        strncpy(map_grid[current_row], buffer, map_cols);
        map_grid[current_row][map_cols] = '\0'; // Đảm bảo null-terminated

        // Tìm A và B, khởi tạo Node tương ứng
        for (int j = 0; j < map_cols; ++j) {
             if (map_grid[current_row][j] == 'A') {
                start_node = (Node*)malloc(sizeof(Node));
                 if (!start_node) { perror("Malloc failed"); fclose(fp); cleanup(); return false; }
                 start_node->x = current_row;
                 start_node->y = j;
                 start_node->g_cost = 0.0;
                 start_node->h_cost = 0.0; // Sẽ tính sau khi biết B
                 start_node->f_cost = 0.0;
                 start_node->parent = NULL;
                 node_details[current_row][j] = start_node; // Lưu node vào mảng chi tiết
                 found_A = true;
            } else if (map_grid[current_row][j] == 'B') {
                 end_node = (Node*)malloc(sizeof(Node));
                  if (!end_node) { perror("Malloc failed"); fclose(fp); cleanup(); return false; }
                 end_node->x = current_row;
                 end_node->y = j;
                 // Chi phí sẽ được tính khi A* chạy đến
                 end_node->g_cost = __DBL_MAX__;
                 end_node->h_cost = 0.0; // Điểm đích có h_cost = 0
                 end_node->f_cost = __DBL_MAX__;
                 end_node->parent = NULL;
                  node_details[current_row][j] = end_node; // Lưu node vào mảng chi tiết
                 found_B = true;
             }
        }
        current_row++;
    }

    fclose(fp);

    if (!found_A || !found_B) {
        fprintf(stderr, "Error: Start (A) or End (B) node not found in the map.\n");
        cleanup();
        return false;
    }

     // Tính h_cost cho điểm bắt đầu sau khi đã biết điểm kết thúc
     start_node->h_cost = calculate_h_cost(start_node->x, start_node->y, end_node);
     start_node->f_cost = start_node->g_cost + start_node->h_cost;


    printf("Map loaded successfully. Start: (%d, %d), End: (%d, %d)\n",
           start_node->x, start_node->y, end_node->x, end_node->y);

    return true;
}

// Truy vết đường đi từ nút đích về nút nguồn và đánh dấu trên bản đồ
void trace_path(Node* dest) {
    printf("Tracing path...\n");
    Node* current = dest;
    int steps = 0;
    while (current != NULL && current->parent != NULL) { // Dừng khi về đến nút gốc (parent là NULL)
        // Không đánh dấu lại A và B
        if (map_grid[current->x][current->y] != 'B' && map_grid[current->x][current->y] != 'A') {
            map_grid[current->x][current->y] = '+'; // Đánh dấu đường đi
        }
        current = current->parent;
        steps++;
         if (steps > map_rows * map_cols) { // Ngăn lặp vô hạn nếu có lỗi
             fprintf(stderr, "Error: Path tracing seems to be in a loop.\n");
             break;
         }
    }
     if (current && map_grid[current->x][current->y] == 'A') {
         printf("Path traced back to Start (A). Length: %d steps (approx)\n", steps);
     } else {
          printf("Path tracing stopped. Found %d steps.\n", steps);
     }
}


// Giải phóng bộ nhớ cho các Node đã được cấp phát trong node_details
void free_node_details() {
    if (node_details) {
        for (int i = 0; i < map_rows; ++i) {
            if (node_details[i]) {
                for (int j = 0; j < map_cols; ++j) {
                    if (node_details[i][j] != NULL) {
                        free(node_details[i][j]); // Giải phóng từng Node
                        node_details[i][j] = NULL;
                    }
                }
            }
        }
    }
    // start_node và end_node đã được free ở trên vì chúng nằm trong node_details
    start_node = NULL;
    end_node = NULL;
}


// Thuật toán tìm đường A*
bool a_star_search() {
    if (!start_node || !end_node || !map_grid || !node_details || !closed_list) {
        fprintf(stderr, "A* search cannot start: Map or nodes not initialized.\n");
        return false;
    }

    // 1. Khởi tạo open list (Hàng đợi ưu tiên)
    PriorityQueue* open_list = create_priority_queue();

    // Đặt nút bắt đầu vào open list (đã được khởi tạo trong load_map)
    push(open_list, start_node);

    printf("Starting A* Search...\n");

    // Mảng lưu các hướng di chuyển (8 hướng)
    // (dx, dy) và chi phí tương ứng (1 cho ngang/dọc, sqrt(2) cho chéo)
    int dx[] = {-1, 1, 0, 0, -1, -1, 1, 1};
    int dy[] = {0, 0, -1, 1, -1, 1, -1, 1};
    double move_cost[] = {1.0, 1.0, 1.0, 1.0, M_SQRT2, M_SQRT2, M_SQRT2, M_SQRT2}; // M_SQRT2 = sqrt(2)


    // 2. Lặp khi open list không rỗng
    while (!is_empty(open_list)) {
        // 3. Lấy nút có f_cost thấp nhất từ open list
        Node* current_node = pop(open_list);

        // 4. Đánh dấu nút hiện tại vào closed list
        closed_list[current_node->x][current_node->y] = true;

        // 5. Kiểm tra nếu đã đến đích
        if (current_node->x == end_node->x && current_node->y == end_node->y) {
            printf("Goal reached!\n");
            end_node->parent = current_node->parent; // Cập nhật parent cho nút đích để trace path
            end_node->g_cost = current_node->g_cost;
            end_node->f_cost = current_node->f_cost;
            trace_path(end_node); // Truy vết đường đi
            free_priority_queue(open_list); // Giải phóng open list
            return true; // Tìm thấy đường đi
        }

        // 6. Duyệt các nút hàng xóm (8 hướng)
        for (int i = 0; i < 8; ++i) {
            int next_x = current_node->x + dx[i];
            int next_y = current_node->y + dy[i];

            // 7. Kiểm tra tính hợp lệ, không bị chặn và chưa có trong closed list
            if (is_valid(next_x, next_y) && is_unblocked(next_x, next_y) && !closed_list[next_x][next_y]) {

                // Tính toán chi phí mới cho hàng xóm
                double tentative_g_cost = current_node->g_cost + move_cost[i];
                double tentative_h_cost = calculate_h_cost(next_x, next_y, end_node);
                double tentative_f_cost = tentative_g_cost + tentative_h_cost;

                // Lấy hoặc tạo Node cho ô hàng xóm
                Node* neighbor_node = node_details[next_x][next_y];

                // Nếu hàng xóm chưa được khám phá HOẶC tìm thấy đường tốt hơn đến nó
                if (neighbor_node == NULL || tentative_g_cost < neighbor_node->g_cost) {
                    // Nếu là nút mới, cấp phát và khởi tạo
                     if (neighbor_node == NULL) {
                         neighbor_node = (Node*)malloc(sizeof(Node));
                         if (!neighbor_node) {
                              perror("Malloc failed for neighbor node");
                              // Cần xử lý lỗi tốt hơn, ví dụ giải phóng tài nguyên
                              free_priority_queue(open_list);
                              return false;
                         }
                         neighbor_node->x = next_x;
                         neighbor_node->y = next_y;
                         node_details[next_x][next_y] = neighbor_node; // Lưu vào mảng chi tiết
                     }

                    // Cập nhật chi phí và cha cho hàng xóm
                    neighbor_node->parent = current_node;
                    neighbor_node->g_cost = tentative_g_cost;
                    neighbor_node->h_cost = tentative_h_cost;
                    neighbor_node->f_cost = tentative_f_cost;

                    // Thêm hàng xóm vào open list (hàm push sẽ xử lý việc sắp xếp)
                    // Nếu nút đã có trong open list, việc thêm lại với f_cost thấp hơn
                    // về mặt logic là ổn, vì khi pop sẽ lấy ra bản có f_cost thấp nhất.
                    // Các triển khai hiệu quả hơn có thể cập nhật nút tại chỗ trong heap.
                    push(open_list, neighbor_node);
                }
            }
        }
    }

    // Nếu vòng lặp kết thúc mà chưa tìm thấy đích
    printf("Failed to find the goal node.\n");
    free_priority_queue(open_list);
    return false; // Không tìm thấy đường đi
}