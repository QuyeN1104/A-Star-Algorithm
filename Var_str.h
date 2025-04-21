#ifndef VAR_STR_H
#define VAR_STR_H

#include <stdbool.h> // Để sử dụng kiểu bool

// Định nghĩa cấu trúc cho một nút trên lưới (ô bản đồ)
// Giữ nguyên cấu trúc Node vì Alg.c vẫn cần thông tin này
typedef struct Node {
    int x, y;          // Tọa độ
    double g_cost;     // Chi phí thực tế từ A đến nút này
    double h_cost;     // Chi phí ước tính (heuristic) từ nút này đến B
    double f_cost;     // Tổng chi phí (f = g + h) - Có thể không dùng để ưu tiên nữa
    struct Node* parent; // Con trỏ tới nút cha để truy vết đường đi
} Node;

// Định nghĩa cấu trúc cho một phần tử trong Hàng đợi (FIFO)
typedef struct QueueNode {
    Node* data;             // Con trỏ tới dữ liệu Node
    struct QueueNode* next; // Con trỏ tới phần tử tiếp theo
} QueueNode;

// Định nghĩa cấu trúc Hàng đợi (FIFO)
typedef struct Queue {
    QueueNode* front; // Con trỏ tới đầu hàng đợi (để lấy ra - dequeue)
    QueueNode* rear;  // Con trỏ tới cuối hàng đợi (để thêm vào - enqueue)
} Queue;

// Khai báo các hàm cho Hàng đợi (FIFO)
Queue* create_queue();
void enqueue(Queue* q, Node* data); // Thêm vào cuối hàng đợi
Node* dequeue(Queue* q);            // Lấy ra từ đầu hàng đợi
bool is_queue_empty(Queue* q);
void free_queue(Queue* q);          // Giải phóng bộ nhớ

#endif // VAR_STR_H