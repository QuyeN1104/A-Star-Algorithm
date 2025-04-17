#include <stdio.h>
#include <stdlib.h>
#include "Var_str.h"

// Tạo một Hàng đợi ưu tiên mới (rỗng)
PriorityQueue* create_priority_queue() {
    PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    if (!pq) {
        perror("Failed to allocate memory for Priority Queue");
        exit(EXIT_FAILURE);
    }
    pq->head = NULL;
    return pq;
}

// Thêm một nút vào Hàng đợi ưu tiên, duy trì sắp xếp theo f_cost tăng dần
void push(PriorityQueue* pq, Node* data) {
    QueueNode* new_queue_node = (QueueNode*)malloc(sizeof(QueueNode));
    if (!new_queue_node) {
        perror("Failed to allocate memory for Queue Node");
        // Có thể cần xử lý lỗi tốt hơn ở đây, ví dụ giải phóng data?
        exit(EXIT_FAILURE);
    }
    new_queue_node->data = data;
    new_queue_node->next = NULL;

    // Nếu hàng đợi rỗng hoặc nút mới có f_cost nhỏ hơn nút đầu
    if (pq->head == NULL || data->f_cost < pq->head->data->f_cost) {
        new_queue_node->next = pq->head;
        pq->head = new_queue_node;
    } else {
        // Tìm vị trí thích hợp để chèn
        QueueNode* current = pq->head;
        while (current->next != NULL && current->next->data->f_cost <= data->f_cost) {
            current = current->next;
        }
        new_queue_node->next = current->next;
        current->next = new_queue_node;
    }
}

// Lấy và xóa nút có f_cost thấp nhất (nút đầu tiên) khỏi hàng đợi
Node* pop(PriorityQueue* pq) {
    if (is_empty(pq)) {
        return NULL;
    }
    QueueNode* temp = pq->head;
    Node* data = temp->data;
    pq->head = pq->head->next;
    free(temp); // Giải phóng bộ nhớ của QueueNode, không phải Node data
    return data;
}

// Kiểm tra xem hàng đợi có rỗng không
bool is_empty(PriorityQueue* pq) {
    return pq->head == NULL;
}

// Giải phóng toàn bộ bộ nhớ của hàng đợi ưu tiên
// Lưu ý: Hàm này chỉ giải phóng các QueueNode, không giải phóng các Node data
// Việc giải phóng Node data cần được quản lý riêng biệt (thường là trong Alg.c)
void free_priority_queue(PriorityQueue* pq) {
    QueueNode* current = pq->head;
    QueueNode* next_node;
    while (current != NULL) {
        next_node = current->next;
        // Không free(current->data) ở đây
        free(current);
        current = next_node;
    }
    free(pq);
}