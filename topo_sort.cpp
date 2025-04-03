#include <iostream>
using namespace std;

// Node for the adjacency list
struct Node {
    int vertex;
    Node* next;
};

// Function to add an edge from u to v in the graph
void addEdge(Node** adjList, int u, int v) {
    // Create a new node for v and add it at the beginning of u's list
    Node* newNode = new Node;
    newNode->vertex = v;
    newNode->next = adjList[u];
    adjList[u] = newNode;
}

// ---------------- DFS-based Topological Sort ----------------

// Recursive helper function for DFS
void dfsUtil(int v, bool* visited, int* stack, int &stackTop, Node** adjList) {
    visited[v] = true;
    // Visit all the adjacent vertices
    Node* temp = adjList[v];
    while (temp != nullptr) {
        int adjVertex = temp->vertex;
        if (!visited[adjVertex]) {
            dfsUtil(adjVertex, visited, stack, stackTop, adjList);
        }
        temp = temp->next;
    }
    // Push current vertex to stack after all its neighbors are processed
    stack[stackTop++] = v;
}

void topologicalSortDFS(Node** adjList, int n) {
    bool* visited = new bool[n];
    for (int i = 0; i < n; i++)
         visited[i] = false;

    int* stack = new int[n];  // This array acts as a stack.
    int stackTop = 0;

    // Perform DFS from every vertex not yet visited
    for (int i = 0; i < n; i++) {
         if (!visited[i])
             dfsUtil(i, visited, stack, stackTop, adjList);
    }

    // The vertices are stored in reverse topological order in the stack.
    cout << "Topological Sort (DFS): ";
    for (int i = stackTop - 1; i >= 0; i--) {
         cout << stack[i] << " ";
    }
    cout << "\n";

    delete[] visited;
    delete[] stack;
}

// ---------------- Kahn's Algorithm ----------------

// Custom queue implementation without STL.
struct Queue {
    int* arr;
    int capacity;
    int front;
    int rear;
    int count;
};

Queue* createQueue(int capacity) {
    Queue* queue = new Queue;
    queue->capacity = capacity;
    queue->front = 0;
    queue->rear = capacity - 1;
    queue->count = 0;
    queue->arr = new int[capacity];
    return queue;
}

bool isEmpty(Queue* queue) {
    return (queue->count == 0);
}

bool isFull(Queue* queue) {
    return (queue->count == queue->capacity);
}

void enqueue(Queue* queue, int item) {
    if (isFull(queue)) {
         // For this example, the queue should never overflow.
         return;
    }
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->arr[queue->rear] = item;
    queue->count++;
}

int dequeue(Queue* queue) {
    if (isEmpty(queue)) {
         return -1; // Should not happen in Kahn's algorithm if the graph is a DAG.
    }
    int item = queue->arr[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->count--;
    return item;
}

void freeQueue(Queue* queue) {
    delete[] queue->arr;
    delete queue;
}

void topologicalSortKahn(Node** adjList, int n) {
    // Calculate in-degrees of all vertices.
    int* inDegree = new int[n];
    for (int i = 0; i < n; i++) {
         inDegree[i] = 0;
    }
    for (int i = 0; i < n; i++) {
         Node* temp = adjList[i];
         while (temp != nullptr) {
             inDegree[temp->vertex]++;
             temp = temp->next;
         }
    }

    // Create a queue and enqueue all vertices with in-degree 0.
    Queue* queue = createQueue(n);
    for (int i = 0; i < n; i++) {
         if (inDegree[i] == 0)
             enqueue(queue, i);
    }

    int count = 0;  // Count of visited vertices.
    int* topOrder = new int[n]; // Array to store the topological order.
    int index = 0;

    // Process vertices in the queue.
    while (!isEmpty(queue)) {
         int u = dequeue(queue);
         topOrder[index++] = u;
         count++;

         // Reduce the in-degree of all neighbors of u.
         Node* temp = adjList[u];
         while (temp != nullptr) {
             int v = temp->vertex;
             inDegree[v]--;
             if (inDegree[v] == 0)
                 enqueue(queue, v);
             temp = temp->next;
         }
    }

    // Check if there was a cycle.
    if (count != n) {
         cout << "There exists a cycle in the graph. Topological sort not possible.\n";
    } else {
         cout << "Topological Sort (Kahn's): ";
         for (int i = 0; i < n; i++) {
              cout << topOrder[i] << " ";
         }
         cout << "\n";
    }

    delete[] inDegree;
    delete[] topOrder;
    freeQueue(queue);
}

// ---------------- Main Function ----------------

int main() {
    int n, e;
    cout << "Enter number of vertices: ";
    cin >> n;
    cout << "Enter number of edges: ";
    cin >> e;

    // Allocate memory for the adjacency list.
    Node** adjList = new Node*[n];
    for (int i = 0; i < n; i++)
         adjList[i] = nullptr;

    cout << "Enter edges (u v) where u -> v (vertices numbered from 0 to " << n-1 << "):\n";
    for (int i = 0; i < e; i++) {
         int u, v;
         cin >> u >> v;
         addEdge(adjList, u, v);
    }

    // Run topological sort using DFS.
    topologicalSortDFS(adjList, n);
    // Run topological sort using Kahn's algorithm.
    topologicalSortKahn(adjList, n);

    // Free the memory allocated for the adjacency list.
    for (int i = 0; i < n; i++) {
         Node* temp = adjList[i];
         while (temp != nullptr) {
             Node* next = temp->next;
             delete temp;
             temp = next;
         }
    }
    delete[] adjList;

    return 0;
}
