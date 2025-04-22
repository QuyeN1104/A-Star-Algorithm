#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Var_str.h"
#include "gen.c"
#include "Alg.c"

// Đọc map từ file và tự tính chiều cao, chiều rộng
char** load_map_from_file(const char* filename, int* height, int* width) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("Failed to open file");
        return NULL;
    }

    char line[1024];
    *height = 0;
    *width = 0;

    // Đếm số dòng và xác định chiều rộng
    while (fgets(line, sizeof(line), fp)) {
        if (*width == 0) {
            *width = (int)strcspn(line, "\r\n"); // Chiều rộng là độ dài dòng đầu tiên
        }
        (*height)++;
    }

    rewind(fp); // Quay lại đầu file để đọc nội dung

    char** map = (char**)malloc(*height * sizeof(char*));
    for (int i = 0; i < *height; ++i) {
        map[i] = (char*)malloc((*width + 1) * sizeof(char));
        fgets(line, sizeof(line), fp);
        strncpy(map[i], line, *width);
        map[i][*width] = '\0'; // Đảm bảo kết thúc chuỗi
    }

    fclose(fp);
    return map;
}

// Lưu map vào file
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

// Hiển thị map lên màn hình
void display_map(char** map, int height, int width) {
    printf("Bản đồ:\n");
    for (int i = 0; i < height; ++i) {
        printf("%s\n", map[i]);
    }
}

// Tìm vị trí góc trên bên trái của drone A hoặc B trong map
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

// Hàm main
int main() {
    const char* map_file = "map.txt";
    const char* result_file = "result_map.txt";

    int height, width, drone_size, num_obstacles;

    // Nhập kích thước bản đồ
    printf("Nhập kích thước bản đồ (chiều cao chiều rộng): ");
    scanf("%d %d", &height, &width);
    // Xác định kích thước vật thể (không quá 1/6 kích thước bản đồ)
    int max_drone_size = (height < width ? height : width) / 6;
    do {
        printf("Nhập kích thước vật thể (tối đa %d): ", max_drone_size);
        scanf("%d", &drone_size);
    } while (drone_size > max_drone_size);

    // Nhập số lượng vật cản
    printf("Nhập số lượng vật cản: ");
    scanf("%d", &num_obstacles);

    // Tạo map và lưu vào file
    char** map = generate_random_map(height, width, drone_size, num_obstacles);
    save_map_to_file(map_file, map, height, width);
    printf("Bản đồ đã được lưu vào file '%s'.\n", map_file);

    // Hiển thị bản đồ ban đầu
    display_map(map, height, width);

    // Hỏi người dùng có muốn nhập điểm A, B từ console không
    char choice;
    printf("Bạn có muốn nhập điểm A, B từ console không? (y/n): ");
    scanf(" %c", &choice);

    int ax, ay, bx, by;

    if (choice == 'y' || choice == 'Y') {
        // Nhập và kiểm tra vị trí drone A
        printf("Nhập tọa độ góc trái trên của drone A (theo định dạng x y): ");
        while (scanf("%d %d", &ax, &ay) != 2 || !can_place_drone(map, height, width, ax, ay, drone_size)) {
            printf("Vị trí không hợp lệ hoặc bị chiếm. Nhập lại x y: ");
            while (getchar() != '\n'); // Xóa bộ đệm nhập nếu cần
        }
        place_drone(map, ax, ay, drone_size, 'A');

        // Nhập và kiểm tra vị trí drone B
        printf("Nhập tọa độ góc trái trên của drone B (theo định dạng x y): ");
        while (scanf("%d %d", &bx, &by) != 2 || !can_place_drone(map, height, width, bx, by, drone_size)) {
            printf("Vị trí không hợp lệ hoặc bị chiếm. Nhập lại x y: ");
            while (getchar() != '\n');
        }
        place_drone(map, bx, by, drone_size, 'B');

    } else {
        // Giải phóng map ban đầu
        free_map(map, height);

        // Load map từ file
        map = load_map_from_file(map_file, &height, &width);
        if (!map) {
            fprintf(stderr, "Không thể tải bản đồ từ file '%s'.\n", map_file);
            return EXIT_FAILURE;
        }

        // Hiển thị bản đồ đã tải
        display_map(map, height, width);

        // Tìm vị trí drone A
        if (!find_drone_position(map, height, width, drone_size, 'A', &ax, &ay)) {
            fprintf(stderr, "Không tìm thấy drone A trên bản đồ.\n");
            free_map(map, height);
            return EXIT_FAILURE;
        }

        // Tìm vị trí drone B
        if (!find_drone_position(map, height, width, drone_size, 'B', &bx, &by)) {
            fprintf(stderr, "Không tìm thấy drone B trên bản đồ.\n");
            free_map(map, height);
            return EXIT_FAILURE;
        }
    }

    // Hiển thị bản đồ sau khi đặt drone A và B
    printf("Bản đồ sau khi đặt drone A và B:\n");
    display_map(map, height, width);

    // Gọi thuật toán A* để tìm đường đi
    astar_find_path(map, height, width, drone_size, ax, ay, bx, by);

    // Lưu bản đồ kết quả vào file
    save_map_to_file(result_file, map, height, width);
    printf("Bản đồ kết quả đã được lưu vào file '%s'.\n", result_file);

    // Giải phóng bộ nhớ
    free_map(map, height);
    return 0;
}
