#include <stdio.h>
#include <stdlib.h>
#include "Var_str.h"

// Tạo một Hàng đợi mới (rỗng)
Queue* create_queue() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    if (!q) {
        perror("Failed to allocate memory for Queue");
        exit(EXIT_FAILURE);
    }
    q->front = q->rear = NULL;
    return q;
}

// Thêm một nút vào cuối hàng đợi (FIFO)
void enqueue(Queue* q, Node* data) {
    QueueNode* new_queue_node = (QueueNode*)malloc(sizeof(QueueNode));
    if (!new_queue_node) {
        perror("Failed to allocate memory for Queue Node");
        exit(EXIT_FAILURE);
    }
    new_queue_node->data = data;
    new_queue_node->next = NULL;

    // Nếu hàng đợi rỗng, nút mới là cả front và rear
    if (q->rear == NULL) {
        q->front = q->rear = new_queue_node;
        return;
    }

    // Thêm vào sau nút rear hiện tại và cập nhật rear
    q->rear->next = new_queue_node;
    q->rear = new_queue_node;
}

// Lấy và xóa nút từ đầu hàng đợi (FIFO)
Node* dequeue(Queue* q) {
    // Nếu hàng đợi rỗng
    if (q->front == NULL) {
        return NULL;
    }

    // Lưu lại nút front hiện tại và dữ liệu của nó
    QueueNode* temp = q->front;
    Node* data = temp->data;

    // Di chuyển front tới nút tiếp theo
    q->front = q->front->next;

    // Nếu front trở thành NULL (hàng đợi rỗng sau khi dequeue), cập nhật cả rear
    if (q->front == NULL) {
        q->rear = NULL;
    }

    free(temp); // Giải phóng bộ nhớ của QueueNode cũ
    return data;
}

// Kiểm tra xem hàng đợi có rỗng không
bool is_queue_empty(Queue* q) {
    return q->front == NULL;
}

// Giải phóng toàn bộ bộ nhớ của hàng đợi
// Chỉ giải phóng QueueNode, không giải phóng Node data
void free_queue(Queue* q) {
    QueueNode* current = q->front;
    QueueNode* next_node;
    while (current != NULL) {
        next_node = current->next;
        // Không free(current->data) ở đây
        free(current);
        current = next_node;
    }
    free(q);
}