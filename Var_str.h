#ifndef VAR_STR_H
#define VAR_STR_H

#include <stdbool.h> // To use the bool type

// Define structure for a node on the grid (map cell)
typedef struct Node {
    int x, y, z;       // Coordinates (added z for 3D space)
    double g_cost;     // Actual cost from A to this node
    double h_cost;     // Estimated cost (heuristic) from this node to B
    double f_cost;     // Total cost (f = g + h)
    struct Node* parent; // Pointer to the parent node for path tracing
} Node;

// Define structure for an element in the priority queue
typedef struct QueueNode {
    Node* data;             // Pointer to the Node data
    struct QueueNode* next; // Pointer to the next element
} QueueNode;

// Define structure for the priority queue
typedef struct Queue {
    QueueNode* front; // Pointer to the front of the queue (for dequeue)
    QueueNode* rear;  // Pointer to the rear of the queue (for enqueue)
} Queue;

// Declare functions for the priority queue
Queue* create_queue(); // Create a new queue
void enqueue_with_priority(Queue* q, Node* data); // Add to the queue in f_cost order
Node* dequeue_min(Queue* q); // Remove the node with the smallest f_cost
bool is_queue_empty(Queue* q); // Check if the queue is empty
void free_queue(Queue* q); // Free the memory of the queue

#endif // VAR_STR_H