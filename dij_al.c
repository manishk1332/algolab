#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

// Structure to represent an adjacency list node.
typedef struct AdjListNode {
    int dest;
    int weight;
    struct AdjListNode* next;
} AdjListNode;

// Structure to represent an adjacency list.
typedef struct AdjList {
    AdjListNode* head;
} AdjList;

// Structure to represent a graph.
typedef struct Graph {
    int V;
    AdjList* array;
} Graph;

// Function to create a new adjacency list node.
AdjListNode* newAdjListNode(int dest, int weight) {
    AdjListNode* newNode = (AdjListNode*) malloc(sizeof(AdjListNode));
    newNode->dest = dest;
    newNode->weight = weight;
    newNode->next = NULL;
    return newNode;
}

// Function to create a graph of V vertices.
Graph* createGraph(int V) {
    Graph* graph = (Graph*) malloc(sizeof(Graph));
    graph->V = V;
    graph->array = (AdjList*) malloc(V * sizeof(AdjList));
    for (int i = 0; i < V; i++)
        graph->array[i].head = NULL;
    return graph;
}

// Adds an edge to an undirected graph.
void addEdge(Graph* graph, int src, int dest, int weight) {
    // Add edge from src to dest.
    AdjListNode* newNode = newAdjListNode(dest, weight);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;

    // Since the graph is undirected, add an edge from dest to src.
    newNode = newAdjListNode(src, weight);
    newNode->next = graph->array[dest].head;
    graph->array[dest].head = newNode;
}

// Structure to represent a node in the min-heap.
typedef struct MinHeapNode {
    int v;
    int dist;
} MinHeapNode;

// Structure to represent a min-heap.
typedef struct MinHeap {
    int size;
    int capacity;
    int *pos;               // Needed for decreaseKey()
    MinHeapNode** array;
} MinHeap;

// Function to create a new min-heap node.
MinHeapNode* newMinHeapNode(int v, int dist) {
    MinHeapNode* minHeapNode = (MinHeapNode*) malloc(sizeof(MinHeapNode));
    minHeapNode->v = v;
    minHeapNode->dist = dist;
    return minHeapNode;
}

// Function to create a min-heap.
MinHeap* createMinHeap(int capacity) {
    MinHeap* minHeap = (MinHeap*) malloc(sizeof(MinHeap));
    minHeap->pos = (int *) malloc(capacity * sizeof(int));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (MinHeapNode**) malloc(capacity * sizeof(MinHeapNode*));
    return minHeap;
}

// A utility function to swap two min-heap nodes.
void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b) {
    MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

// Standard min-heapify function.
void minHeapify(MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;
    if (left < minHeap->size && minHeap->array[left]->dist < minHeap->array[smallest]->dist)
        smallest = left;
    if (right < minHeap->size && minHeap->array[right]->dist < minHeap->array[smallest]->dist)
        smallest = right;
    if (smallest != idx) {
        // Update positions.
        MinHeapNode* smallestNode = minHeap->array[smallest];
        MinHeapNode* idxNode = minHeap->array[idx];
        minHeap->pos[smallestNode->v] = idx;
        minHeap->pos[idxNode->v] = smallest;
        // Swap nodes.
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

// Extracts the node with the minimum distance value from the min-heap.
MinHeapNode* extractMin(MinHeap* minHeap) {
    if (minHeap->size == 0)
        return NULL;
    MinHeapNode* root = minHeap->array[0];
    MinHeapNode* lastNode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastNode;

    // Update positions.
    minHeap->pos[root->v] = minHeap->size - 1;
    minHeap->pos[lastNode->v] = 0;
    --minHeap->size;
    minHeapify(minHeap, 0);
    return root;
}

// Decrease distance value of a given vertex v.
void decreaseKey(MinHeap* minHeap, int v, int dist) {
    int i = minHeap->pos[v];
    minHeap->array[i]->dist = dist;

    while (i && minHeap->array[i]->dist < minHeap->array[(i - 1) / 2]->dist) {
        minHeap->pos[minHeap->array[i]->v] = (i - 1) / 2;
        minHeap->pos[minHeap->array[(i - 1) / 2]->v] = i;
        swapMinHeapNode(&minHeap->array[i],  &minHeap->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

// Check if a given vertex is in min-heap.
bool isInMinHeap(MinHeap *minHeap, int v) {
   return minHeap->pos[v] < minHeap->size;
}

// Recursive function to print the path from source to j using the parent array.
void printPathList(int parent[], int j) {
    if (parent[j] == -1) {
        printf("%d", j);
        return;
    }
    printPathList(parent, parent[j]);
    printf(" -> %d", j);
}

// Dijkstra's algorithm using the adjacency list representation.
void dijkstraList(Graph* graph, int src) {
    int V = graph->V; // Number of vertices
    int dist[V];      // dist[i] holds the shortest distance from src to i.
    int parent[V];    // To store shortest path tree

    // Create a min heap.
    MinHeap* minHeap = createMinHeap(V);

    // Initialize min heap with all vertices. Distance value of src is 0.
    for (int v = 0; v < V; ++v) {
        dist[v] = INT_MAX;
        parent[v] = -1;  // Initialize parent as -1.
        minHeap->array[v] = newMinHeapNode(v, dist[v]);
        minHeap->pos[v] = v;
    }
    dist[src] = 0;
    decreaseKey(minHeap, src, dist[src]);

    // Set initial size of min heap.
    minHeap->size = V;

    // Loop until min heap becomes empty.
    while (minHeap->size) {
        MinHeapNode* minHeapNode = extractMin(minHeap);
        int u = minHeapNode->v;

        // Traverse all adjacent vertices of u.
        AdjListNode* crawl = graph->array[u].head;
        while (crawl != NULL) {
            int v = crawl->dest;
            // If v is in min heap and a shorter path is found.
            if (isInMinHeap(minHeap, v) && dist[u] != INT_MAX 
                && crawl->weight + dist[u] < dist[v]) {
                dist[v] = dist[u] + crawl->weight;
                parent[v] = u;  // Update parent.
                decreaseKey(minHeap, v, dist[v]);
            }
            crawl = crawl->next;
        }
    }

    // Print the calculated shortest distances and paths.
    printf("\nDijkstra (Adjacency List) from source %d:\n", src);
    printf("Vertex \t Distance \t Path\n");
    for (int i = 0; i < V; i++) {
        printf("%d \t\t ", i);
        if (dist[i] == INT_MAX)
            printf("INF \t\t No path\n");
        else {
            printf("%d \t\t ", dist[i]);
            printPathList(parent, i);
            printf("\n");
        }
    }
}

int main() {
    int V = 9;
    Graph* graph = createGraph(V);

    // Add edges to the graph.
    addEdge(graph, 0, 1, 4);
    addEdge(graph, 0, 7, 8);
    addEdge(graph, 1, 2, 8);
    addEdge(graph, 1, 7, 11);
    addEdge(graph, 2, 3, 7);
    addEdge(graph, 2, 8, 2);
    addEdge(graph, 2, 5, 4);
    addEdge(graph, 3, 4, 9);
    addEdge(graph, 3, 5, 14);
    addEdge(graph, 4, 5, 10);
    addEdge(graph, 5, 6, 2);
    addEdge(graph, 6, 7, 1);
    addEdge(graph, 6, 8, 6);
    addEdge(graph, 7, 8, 7);

    dijkstraList(graph, 0);
    return 0;
}
