#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h> // for memset

// Định nghĩa cấu trúc lưu thông tin vật cản (hình chữ nhật)
typedef struct {
    int x1, y1; // Tọa độ góc trên trái
    int x2, y2; // Tọa độ góc dưới phải
} Obstacle;

// Hàm kiểm tra xem hai hình chữ nhật có chồng lấn HOẶC quá gần nhau không
// (min_gap: khoảng cách tối thiểu giữa 2 vật cản, tính cả biên)
bool check_overlap_or_too_close(Obstacle r1, Obstacle r2, int min_gap) {
    // Mở rộng r1 theo min_gap để kiểm tra chồng lấn
    int expanded_x1 = r1.x1 - min_gap;
    int expanded_y1 = r1.y1 - min_gap;
    int expanded_x2 = r1.x2 + min_gap;
    int expanded_y2 = r1.y2 + min_gap;

    // Kiểm tra chồng lấn giữa r2 và r1 đã mở rộng
    bool x_overlap = (r2.x1 <= expanded_x2) && (r2.x2 >= expanded_x1);
    bool y_overlap = (r2.y1 <= expanded_y2) && (r2.y2 >= expanded_y1);

    return x_overlap && y_overlap;
}


int main() {
    int map_height, map_width, drone_size, max_obstacles;
    double obstacle_percentage_target;

    // --- Nhập tham số từ Console ---
    printf("Nhap kich thuoc ban do (chieu cao chieu rong): ");
    if (scanf("%d %d", &map_height, &map_width) != 2 || map_height <= 0 || map_width <= 0) {
        fprintf(stderr, "Kich thuoc ban do khong hop le!\n");
        return 1;
    }

    int max_drone_size = (map_height < map_width ? map_height : map_width) / 6;
    printf("Nhap kich thuoc thiet bi bay (hinh vuong, canh <= %d): ", max_drone_size);
    if (scanf("%d", &drone_size) != 1 || drone_size <= 0 || drone_size > max_drone_size) {
        fprintf(stderr, "Kich thuoc thiet bi bay khong hop le!\n");
        return 1;
    }

    printf("Nhap ty le phan tram vat can (vd: 65 cho 65%%): ");
    if (scanf("%lf", &obstacle_percentage_target) != 1 || obstacle_percentage_target <= 0 || obstacle_percentage_target >= 100) {
        fprintf(stderr, "Ty le phan tram khong hop le!\n");
        return 1;
    }

    // Tính toán số ô vật cản mục tiêu
    long long total_cells = (long long)map_height * map_width;
    long long target_obstacle_cells = (long long)(total_cells * (obstacle_percentage_target / 100.0));
    long long current_obstacle_cells = 0;

    printf("Ban do: %d x %d\n", map_height, map_width);
    printf("Drone: %d x %d\n", drone_size, drone_size);
    printf("Muc tieu vat can: > %.2f%% (%lld o)\n", obstacle_percentage_target, target_obstacle_cells);

    // --- Khởi tạo bản đồ ---
    char** map = (char**)malloc(map_height * sizeof(char*));
    if (!map) {
        perror("Khong the cap phat bo nho cho ban do (hang)");
        return 1;
    }
    for (int i = 0; i < map_height; ++i) {
        map[i] = (char*)malloc((map_width + 1) * sizeof(char)); // +1 cho '\0'
        if (!map[i]) {
            perror("Khong the cap phat bo nho cho ban do (cot)");
            // Giải phóng bộ nhớ đã cấp phát
            for(int k=0; k<i; ++k) free(map[k]);
            free(map);
            return 1;
        }
        memset(map[i], '.', map_width); // Khởi tạo tất cả là ô trống '.'
        map[i][map_width] = '\0';
    }

    // --- Sinh vật cản ---
    srand(time(NULL)); // Khởi tạo bộ sinh số ngẫu nhiên
    Obstacle* obstacles = NULL; // Mảng lưu các vật cản đã tạo
    int obstacle_count = 0;
    int max_obstacle_list_size = 0; // Kích thước hiện tại của mảng obstacles

    int max_placement_attempts = map_height * map_width * 2; // Giới hạn số lần thử đặt vật cản
    int attempts = 0;

    printf("Dang tao vat can...\n");

    // Khoảng cách tối thiểu giữa các vật cản (để drone đi qua)
    // Đơn giản hóa: đảm bảo có ít nhất 1 ô trống giữa 2 vật cản biên
    // Để đảm bảo drone_size, gap thực tế cần là drone_size
    int min_gap_between_obstacles = drone_size;


    while (current_obstacle_cells < target_obstacle_cells && attempts < max_placement_attempts) {
        attempts++;

        // 1. Tạo kích thước ngẫu nhiên cho vật cản mới
        int obs_height = (rand() % (map_height / 2)) + 1; // Chiều cao ngẫu nhiên, tối đa 1/2 bản đồ
        int obs_width = (rand() % (map_width / 2)) + 1;   // Chiều rộng ngẫu nhiên, tối đa 1/2 bản đồ

        // Kiểm tra ràng buộc 1/3 (KHÔNG ĐƯỢC BẰNG)
        if (map_height >= 3 && obs_height == map_height / 3) {
             // printf("Attempt %d: Skip h=%d (1/3 height)\n", attempts, obs_height);
             continue; // Thử lại nếu bằng 1/3
        }
        if (map_width >= 3 && obs_width == map_width / 3) {
             // printf("Attempt %d: Skip w=%d (1/3 width)\n", attempts, obs_width);
             continue; // Thử lại nếu bằng 1/3
        }


        // 2. Tạo vị trí ngẫu nhiên (góc trên trái)
        if (map_height - obs_height <= 0 || map_width - obs_width <= 0) continue; // Kích thước quá lớn
        int r = rand() % (map_height - obs_height);
        int c = rand() % (map_width - obs_width);

        Obstacle new_obs;
        new_obs.x1 = r;
        new_obs.y1 = c;
        new_obs.x2 = r + obs_height - 1;
        new_obs.y2 = c + obs_width - 1;

        // 3. Kiểm tra tính hợp lệ (chồng lấn / quá gần vật cản khác)
        bool valid_placement = true;
        for (int i = 0; i < obstacle_count; ++i) {
            if (check_overlap_or_too_close(new_obs, obstacles[i], min_gap_between_obstacles)) {
                valid_placement = false;
                break;
            }
        }

        // 4. Nếu hợp lệ, đặt vật cản vào bản đồ và lưu lại
        if (valid_placement) {
            // printf("Attempt %d: Placing obstacle [%d,%d] to [%d,%d] (size %dx%d)\n", attempts, new_obs.x1, new_obs.y1, new_obs.x2, new_obs.y2, obs_width, obs_height);
            for (int i = new_obs.x1; i <= new_obs.x2; ++i) {
                for (int j = new_obs.y1; j <= new_obs.y2; ++j) {
                    if (map[i][j] == '.') { // Chỉ thay đổi ô trống, tránh ghi đè A/B nếu có
                         map[i][j] = '#';
                         current_obstacle_cells++;
                    }
                }
            }

             // Lưu vật cản vào danh sách
             if (obstacle_count >= max_obstacle_list_size) {
                 // Mở rộng mảng obstacles nếu cần
                 max_obstacle_list_size = (max_obstacle_list_size == 0) ? 10 : max_obstacle_list_size * 2;
                 Obstacle* temp = realloc(obstacles, max_obstacle_list_size * sizeof(Obstacle));
                 if (!temp) {
                     perror("Khong the cap phat lai bo nho cho danh sach vat can");
                     // Giải phóng bộ nhớ trước khi thoát
                      free(obstacles);
                       for(int i=0; i<map_height; ++i) free(map[i]);
                       free(map);
                       return 1;
                 }
                 obstacles = temp;
             }
             obstacles[obstacle_count++] = new_obs;
             attempts = 0; // Reset số lần thử không thành công

        }
        // else {
            // printf("Attempt %d: Failed placement [%d,%d] to [%d,%d]\n", attempts, new_obs.x1, new_obs.y1, new_obs.x2, new_obs.y2);
        // }

        // Dừng nếu đã đạt đủ số ô vật cản
         if (current_obstacle_cells >= target_obstacle_cells) break;
    }

     printf("Da tao %d vat can, chiem %lld/%lld o (%.2f%%).\n",
            obstacle_count, current_obstacle_cells, total_cells,
            (double)current_obstacle_cells * 100.0 / total_cells);

     free(obstacles); // Giải phóng danh sách vật cản sau khi dùng xong

    // --- Đặt điểm A và B ---
    int ax, ay, bx, by;
    int place_attempts = 0;
    const int max_place_ab_attempts = map_height * map_width * 2;

    // Đặt A
    do {
        ax = rand() % map_height;
        ay = rand() % map_width;
        place_attempts++;
    } while (map[ax][ay] != '.' && place_attempts < max_place_ab_attempts);

    if (map[ax][ay] == '.') {
        map[ax][ay] = 'A';
        printf("Dat diem A tai (%d, %d)\n", ax, ay);
    } else {
         fprintf(stderr, "Khong the tim duoc vi tri trong cho A!\n");
         // Tiếp tục mà không có A hoặc thoát? Tạm thời tiếp tục.
    }


    // Đặt B
    place_attempts = 0;
     do {
        bx = rand() % map_height;
        by = rand() % map_width;
        place_attempts++;
    } while ((map[bx][by] != '.' || (bx == ax && by == ay)) && place_attempts < max_place_ab_attempts); // Đảm bảo khác A và trống

     if (map[bx][by] == '.') {
        map[bx][by] = 'B';
         printf("Dat diem B tai (%d, %d)\n", bx, by);
    } else {
         fprintf(stderr, "Khong the tim duoc vi tri trong cho B!\n");
    }


    // --- Ghi ra file map.txt ---
    FILE *fp = fopen("map.txt", "w");
    if (!fp) {
        perror("Khong the mo file map.txt de ghi");
         // Giải phóng bộ nhớ
        for(int i=0; i<map_height; ++i) free(map[i]);
        free(map);
        return 1;
    }

    printf("Dang ghi ban do ra file map.txt...\n");
    for (int i = 0; i < map_height; ++i) {
        fprintf(fp, "%s\n", map[i]);
    }

    fclose(fp);
    printf("Tao file map.txt thanh cong!\n");

    // --- Giải phóng bộ nhớ ---
    for (int i = 0; i < map_height; ++i) {
        free(map[i]);
    }
    free(map);

    return 0;
}