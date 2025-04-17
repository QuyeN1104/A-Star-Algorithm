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

// Định nghĩa cấu trúc Hàng đợi ưu tiên (Triển khai bằng DSLK sắp xếp)
typedef struct PriorityQueue {
    QueueNode* head; // Con trỏ tới đầu hàng đợi
} PriorityQueue;

// Khai báo các hàm cho Hàng đợi ưu tiên
PriorityQueue* create_priority_queue();
void push(PriorityQueue* pq, Node* data); // Thêm vào và duy trì thứ tự f_cost
Node* pop(PriorityQueue* pq);             // Lấy ra phần tử có f_cost thấp nhất
bool is_empty(PriorityQueue* pq);
void free_priority_queue(PriorityQueue* pq); // Giải phóng bộ nhớ

#endif // VAR_STR_H