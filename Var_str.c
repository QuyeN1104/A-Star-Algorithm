#include <stdio.h>
#include <stdlib.h>
#include "Var_str.h"

// Create a new (empty) priority queue
Queue* create_queue() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    if (!q) {
        perror("Failed to allocate memory for Queue");
        exit(EXIT_FAILURE);
    }
    q->front = q->rear = NULL;
    return q;
}

// Add a node to the priority queue in ascending order of f_cost
void enqueue_with_priority(Queue* q, Node* data) {
    QueueNode* new_node = (QueueNode*)malloc(sizeof(QueueNode));
    if (!new_node) {
        perror("Failed to allocate memory for Queue Node");
        exit(EXIT_FAILURE);
    }
    new_node->data = data;
    new_node->next = NULL;

    // If the queue is empty
    if (!q->front) {
        q->front = q->rear = new_node;
        return;
    }

    // Insert at the front if f_cost is smaller than the first node
    if (data->f_cost < q->front->data->f_cost) {
        new_node->next = q->front;
        q->front = new_node;
        return;
    }

    // Find the appropriate position to insert
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

// Remove and return the node with the smallest f_cost from the queue
Node* dequeue_min(Queue* q) {
    if (!q->front) return NULL;

    QueueNode* temp = q->front;
    Node* data = temp->data;
    q->front = q->front->next;

    if (!q->front) q->rear = NULL;

    free(temp);
    return data;
}

// Check if the queue is empty
bool is_queue_empty(Queue* q) {
    return q->front == NULL;
}

// Free all memory of the queue
void free_queue(Queue* q) {
    while (!is_queue_empty(q)) {
        Node* node = dequeue_min(q);
        free(node);
    }
    free(q);
}