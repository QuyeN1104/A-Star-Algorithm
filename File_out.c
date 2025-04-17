#include <stdio.h>
#include <stdlib.h>
#include "Var_str.h" // Để dùng các khai báo như Node, map_grid,...
#include <stdbool.h>

// --- Khai báo các biến toàn cục từ Alg.c ---
// extern để báo cho trình biên dịch biết biến này được định nghĩa ở file khác
extern char** map_grid;
extern int map_rows;
extern int map_cols;
extern Node* start_node;
extern Node* end_node;


// --- Khai báo các hàm từ Alg.c ---
extern bool load_map(const char* filename);
extern bool a_star_search();
extern void cleanup(); // Hàm dọn dẹp tài nguyên
extern void free_node_details(); // Hàm giải phóng các Node


// Hiển thị bản đồ ra console
void display_map() {
    if (!map_grid) {
        printf("Map not loaded.\n");
        return;
    }
    printf("\nFinal Map with Path:\n");
    for (int i = 0; i < map_rows; ++i) {
        for (int j = 0; j < map_cols; ++j) {
            printf("%c", map_grid[i][j]);
        }
        printf("\n");
    }
}

int main() {
    const char* map_filename = "map.txt"; // Tên file bản đồ

    // Đăng ký hàm cleanup để tự động gọi khi chương trình kết thúc
    // và hàm free_node_details để giải phóng các Node
    atexit(cleanup);
    atexit(free_node_details);


    // 1. Tải bản đồ
    if (!load_map(map_filename)) {
        fprintf(stderr, "Failed to load the map '%s'. Exiting.\n", map_filename);
        return EXIT_FAILURE;
    }

    // Hiển thị bản đồ ban đầu (tùy chọn)
    // printf("Initial Map:\n");
    // display_map();


    // 2. Thực hiện tìm kiếm A*
    if (a_star_search()) {
        printf("\nPath found successfully!\n");
        // 3. Hiển thị bản đồ với đường đi
        display_map();
    } else {
        printf("\nCould not find a path from A to B.\n");
         // Hiển thị bản đồ gốc nếu không tìm thấy đường
         printf("\nOriginal Map:\n");
         display_map();
    }


    // Cleanup sẽ được gọi tự động bởi atexit() khi main kết thúc

    return EXIT_SUCCESS;
}