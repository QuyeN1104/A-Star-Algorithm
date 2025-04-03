#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Enum để biểu diễn hướng di chuyển của rào chắn
typedef enum {
    DIAGONAL_DOWN_RIGHT = -1,
    HORIZONTAL_RIGHT = 0,
    DIAGONAL_UP_RIGHT = 1,
    VERTICAL_DOWN = 2
} Direction;

// Kiểm tra xem vị trí có va chạm với rào chắn không
int has_collision(char **map, int x, int y, int side_object) {
    for (int dy = -side_object; dy <= side_object; dy++) {
        for (int dx = -side_object; dx <= side_object; dx++) {
            if (map[y + dy][x + dx] == '#') {
                return 1; // Có va chạm
            }
        }
    }
    return 0; // Không có va chạm
}

// Kiểm tra xem rào chắn có thể đặt hợp lệ không
int is_valid_barrier(char **map, int x1, int y1, int x2, int y2, Direction direction, int length_map, int width_map, int side_object) {
    // Kiểm tra nếu rào chắn nằm ngoài biên bản đồ
    if (x1 < side_object || x1 >= width_map - side_object ||
        y1 < side_object || y1 >= length_map - side_object ||
        x2 < side_object || x2 >= width_map - side_object ||
        y2 < side_object || y2 >= length_map - side_object) {
        return 0; // Ngoài biên
    }

    // Kiểm tra va chạm tại điểm đầu và điểm cuối
    if (has_collision(map, x1, y1, side_object) || has_collision(map, x2, y2, side_object)) {
        return 0; // Va chạm
    }

    // Kiểm tra va chạm dọc theo rào chắn
    int dx = (x2 > x1) ? 1 : (x2 < x1 ? -1 : 0);
    int dy = (y2 > y1) ? 1 : (y2 < y1 ? -1 : 0);

    for (int i = x1, j = y1; i != x2 || j != y2; i += dx, j += dy) {
        if (has_collision(map, i, j, side_object)) {
            return 0; // Va chạm
        }
    }

    return 1; // Hợp lệ
}

// Đặt rào chắn lên bản đồ
void place_barrier(char **map, int x1, int y1, int x2, int y2) {
    int dx = (x2 > x1) ? 1 : (x2 < x1 ? -1 : 0);
    int dy = (y2 > y1) ? 1 : (y2 < y1 ? -1 : 0);

    for (int i = x1, j = y1; i != x2 || j != y2; i += dx, j += dy) {
        map[j][i] = '#';
    }
    map[y2][x2] = '#'; // Đánh dấu điểm cuối
}

// Sinh rào chắn ngẫu nhiên và đặt lên bản đồ
void generate_barrier(char **map, int length_map, int width_map, int side_object) {
    int x1, y1, x2, y2, length, attempts = 0;
    const int max_attempts = 1000;
    Direction direction;
    
    do {
        // Chọn ngẫu nhiên vị trí bắt đầu
        x1 = rand() % (width_map - 2 * side_object) + side_object;
        y1 = rand() % (length_map - 2 * side_object) + side_object;
        length = rand() % ((length_map < width_map ? length_map : width_map) / 3) + 1;
        direction = (Direction)(rand() % 4 - 1);

        // Xác định điểm kết thúc tùy theo hướng
        switch (direction) {
            case HORIZONTAL_RIGHT:
                x2 = x1 + length;
                y2 = y1;
                break;
            case VERTICAL_DOWN:
                x2 = x1;
                y2 = y1 + length;
                break;
            case DIAGONAL_DOWN_RIGHT:
                x2 = x1 + length;
                y2 = y1 + length;
                break;
            case DIAGONAL_UP_RIGHT:
                x2 = x1 + length;
                y2 = y1 - length;
                break;
        }

        if (++attempts > max_attempts) return;
    } while (!is_valid_barrier(map, x1, y1, x2, y2, direction, length_map, width_map, side_object));
    
    // Đặt rào chắn lên bản đồ
    place_barrier(map, x1, y1, x2, y2);
}

// Tạo bản đồ
char **generate_map(int length_map, int width_map, int side_object, int num_barriers) {
    char **map = malloc(length_map * sizeof(char *));
    for (int i = 0; i < length_map; i++) {
        map[i] = malloc(width_map * sizeof(char));
        memset(map[i], '.', width_map); // Khởi tạo bản đồ với '.'
    }
    
    srand(time(NULL));
    for (int i = 0; i < num_barriers; i++) {
        generate_barrier(map, length_map, width_map, side_object);
    }
    return map;
}

// Chương trình chính
int main() {
    int length_map, width_map, num_barriers;
    
    // Nhập kích thước bản đồ và số lượng rào chắn
    printf("Enter map length, width, and number of barriers: ");
    scanf("%d %d %d", &length_map, &width_map, &num_barriers);
    
    int side_object = 1; // Kích thước vật thể
    char **map = generate_map(length_map, width_map, side_object, num_barriers);
    
    // In bản đồ
    for (int i = 0; i < length_map; i++) {
        for (int j = 0; j < width_map; j++) {
            printf("%c ", map[i][j]);
        }
        printf("\n");
        free(map[i]);
    }
    free(map);
    return 0;
}
