#ifndef VAR_STR_H
#define VAR_STR_H

#include <stdbool.h> // Để sử dụng kiểu bool

// Định nghĩa cấu trúc cho một nút trên lưới (ô bản đồ)
typedef struct Node {
    int x, y;          // Tọa độ
    double g_cost;     // Chi phí thực tế từ A đến nút này
    double h_cost;     // Chi phí ước tính (heuristic) từ nút này đến B
    double f_cost;     // Tổng chi phí (f = g + h)
    struct Node* parent; // Con trỏ tới nút cha để truy vết đường đi
} Node;

// Định nghĩa cấu trúc cho một phần tử trong Hàng đợi ưu tiên
typedef struct QueueNode {
    Node* data;             // Con trỏ tới dữ liệu Node
    struct QueueNode* next; // Con trỏ tới phần tử tiếp theo
} QueueNode;

// Định nghĩa cấu trúc Hàng đợi ưu tiên
typedef struct Queue {
    QueueNode* front; // Con trỏ tới đầu hàng đợi (để lấy ra - dequeue)
    QueueNode* rear;  // Con trỏ tới cuối hàng đợi (để thêm vào - enqueue)
} Queue;

// Khai báo các hàm cho Hàng đợi ưu tiên
Queue* create_queue(); // Tạo một hàng đợi mới
void enqueue_with_priority(Queue* q, Node* data); // Thêm vào hàng đợi theo thứ tự f_cost
Node* dequeue_min(Queue* q); // Lấy ra nút có f_cost nhỏ nhất
bool is_queue_empty(Queue* q); // Kiểm tra xem hàng đợi có rỗng không
void free_queue(Queue* q); // Giải phóng bộ nhớ của hàng đợi

#endif // VAR_STR_H