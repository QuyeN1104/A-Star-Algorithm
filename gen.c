#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

// Khởi tạo map rỗng
char** init_map(int height, int width) {
    char** map = (char**)malloc(height * sizeof(char*));
    for (int i = 0; i < height; ++i) {
        map[i] = (char*)malloc((width + 1) * sizeof(char));
        memset(map[i], '.', width);
        map[i][width] = '\0';
    }
    return map;
}

// Kiểm tra có thể đặt vật cản từ (x1, y1) đến (x2, y2) không
bool can_place_obstacle(char** map, int height, int width, int x1, int y1, int x2, int y2, int drone_size) {
    int dx = (x2 > x1) ? 1 : (x2 < x1 ? -1 : 0);
    int dy = (y2 > y1) ? 1 : (y2 < y1 ? -1 : 0);
    int len = (dx != 0) ? abs(x2 - x1) : abs(y2 - y1);
    int x = x1, y = y1;
    for (int i = 0; i <= len; ++i) {
        // Kiểm tra biên và khoảng cách tối thiểu cho drone
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

// Đặt vật cản từ (x1, y1) đến (x2, y2)
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

// Sinh vật cản ngẫu nhiên (chỉ là đường thẳng: ngang, dọc, chéo)
bool generate_random_obstacle(char** map, int height, int width, int drone_size) {
    int max_len = (height < width ? height : width) / 3;
    int direction = rand() % 3; // 0: ngang, 1: dọc, 2: chéo
    int len = rand() % (max_len - 1) + 2; // Độ dài từ 2 đến max_len
    int x1, y1, x2, y2;
    int attempts = 0, max_attempts = 100;
    while (attempts++ < max_attempts) {
        if (direction == 0) { // ngang (đường thẳng)
            y1 = rand() % height;
            x1 = rand() % (width - len);
            x2 = x1 + len;
            y2 = y1;
        } else if (direction == 1) { // dọc (đường thẳng)
            x1 = rand() % width;
            y1 = rand() % (height - len);
            x2 = x1;
            y2 = y1 + len;
        } else { // chéo (đường thẳng)
            x1 = rand() % (width - len);
            y1 = rand() % (height - len);
            x2 = x1 + len;
            y2 = y1 + len;
        }
        // Không tạo vật cản hình dạng phức tạp, chỉ đường thẳng
        if (can_place_obstacle(map, height, width, x1, y1, x2, y2, drone_size)) {
            place_obstacle(map, x1, y1, x2, y2);
            return true;
        }
    }
    return false;
}

// Sinh map ngẫu nhiên với số lượng vật cản mong muốn, đảm bảo tỷ lệ tối thiểu
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

    // Nếu chưa đủ 65% diện tích, thêm các vật cản đường thẳng không chồng lên nhau
    int total_cells = height * width;
    int obstacle_cells = 0;
    for (int i = 0; i < height; ++i)
        for (int j = 0; j < width; ++j)
            if (map[i][j] == '#') obstacle_cells++;
    int max_attempts = 1000; // Giới hạn số lần thử để tránh chạy vô hạn
    int attempts = 0;
    while ((double)obstacle_cells / total_cells < 0.65 && attempts < max_attempts) {
        if (generate_random_obstacle(map, height, width, drone_size)) {
            obstacle_cells = 0; // Tính lại số ô bị chiếm bởi vật cản
            for (int i = 0; i < height; ++i)
                for (int j = 0; j < width; ++j)
                    if (map[i][j] == '#') obstacle_cells++;
        }
        attempts++;
    }


    return map;
}

// Hàm giải phóng map
void free_map(char** map, int height) {
    for (int i = 0; i < height; ++i) free(map[i]);
    free(map);
}

// Kiểm tra xem có thể đặt drone tại vị trí (x, y) với kích thước drone_size không
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

// Đánh dấu drone trên map tại vị trí (x, y) với ký tự ch (A hoặc B)
void place_drone(char** map, int x, int y, int drone_size, char ch) {
    for (int i = 0; i < drone_size; ++i)
        for (int j = 0; j < drone_size; ++j)
            map[y + i][x + j] = ch;
}
