#include <stdio.h>
#include <stdlib.h>
#include "Var_str.h"

// Tạo một Hàng đợi ưu tiên mới (rỗng)
Queue* create_queue() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    if (!q) {
        perror("Failed to allocate memory for Queue");
        exit(EXIT_FAILURE);
    }
    q->front = q->rear = NULL;
    return q;
}

// Thêm một nút vào hàng đợi ưu tiên theo f_cost tăng dần
void enqueue_with_priority(Queue* q, Node* data) {
    QueueNode* new_node = (QueueNode*)malloc(sizeof(QueueNode));
    if (!new_node) {
        perror("Failed to allocate memory for Queue Node");
        exit(EXIT_FAILURE);
    }
    new_node->data = data;
    new_node->next = NULL;

    // Nếu hàng đợi rỗng
    if (!q->front) {
        q->front = q->rear = new_node;
        return;
    }

    // Chèn vào đầu nếu f_cost nhỏ hơn node đầu tiên
    if (data->f_cost < q->front->data->f_cost) {
        new_node->next = q->front;
        q->front = new_node;
        return;
    }

    // Tìm vị trí thích hợp để chèn
    QueueNode* current = q->front;
    while (current->next && current->next->data->f_cost <= data->f_cost) {
        current = current->next;
    }

    new_node->next = current->next;
    current->next = new_node;
    if (!new_node->next) {
        q->rear = new_node;
    }
}

// Lấy và xóa nút có f_cost nhỏ nhất từ hàng đợi
Node* dequeue_min(Queue* q) {
    if (!q->front) return NULL;

    QueueNode* temp = q->front;
    Node* data = temp->data;
    q->front = q->front->next;

    if (!q->front) q->rear = NULL;

    free(temp);
    return data;
}

// Kiểm tra xem hàng đợi có rỗng không
bool is_queue_empty(Queue* q) {
    return q->front == NULL;
}

// Giải phóng toàn bộ bộ nhớ của hàng đợi
void free_queue(Queue* q) {
    while (!is_queue_empty(q)) {
        Node* node = dequeue_min(q);
        free(node);
    }
    free(q);
}