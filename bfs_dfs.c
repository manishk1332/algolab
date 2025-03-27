#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_QUEUE 100

/********************** Adjacency Matrix Implementation **********************/

/* 
   BFS for adjacency matrix.
   n: number of vertices
   graph: n x n matrix (0 means no edge, nonzero means an edge exists)
   start: starting vertex
*/
void BFS_matrix(int n, int graph[n][n], int start) {
    bool visited[n];
    for (int i = 0; i < n; i++)
        visited[i] = false;

    int queue[MAX_QUEUE];
    int front = 0, rear = 0;
    
    visited[start] = true;
    queue[rear++] = start;
    
    printf("BFS (Adjacency Matrix): ");
    while (front < rear) {
        int u = queue[front++];
        printf("%d ", u);
        for (int v = 0; v < n; v++) {
            if (graph[u][v] && !visited[v]) {
                visited[v] = true;
                queue[rear++] = v;
            }
        }
    }
    printf("\n");
}

/* 
   DFS helper for adjacency matrix.
   visited: array to track visited vertices.
*/
void DFS_matrix_util(int n, int graph[n][n], int u, bool visited[]) {
    visited[u] = true;
    printf("%d ", u);
    for (int v = 0; v < n; v++) {
        if (graph[u][v] && !visited[v]) {
            DFS_matrix_util(n, graph, v, visited);
        }
    }
}

/* DFS for adjacency matrix starting at vertex start */
void DFS_matrix(int n, int graph[n][n], int start) {
    bool visited[n];
    for (int i = 0; i < n; i++)
        visited[i] = false;
    printf("DFS (Adjacency Matrix): ");
    DFS_matrix_util(n, graph, start, visited);
    printf("\n");
}

/********************** Adjacency List Implementation **********************/

/* Structure for a node in the adjacency list */
typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

/* Structure for the graph */
typedef struct {
    int numVertices;
    Node** adjLists;  // Array of pointers to adjacency lists
} Graph;

/* Create a new node */
Node* createNode(int vertex) {
    Node* newNode = malloc(sizeof(Node));
    if (!newNode) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    newNode->vertex = vertex;
    newNode->next = NULL;
    return newNode;
}

/* Create a graph with n vertices */
Graph* createGraph(int n) {
    Graph* graph = malloc(sizeof(Graph));
    if (!graph) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    graph->numVertices = n;
    graph->adjLists = malloc(n * sizeof(Node*));
    if (!graph->adjLists) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n; i++)
        graph->adjLists[i] = NULL;
    return graph;
}

/* Add an undirected edge between src and dest */
void addEdge(Graph* graph, int src, int dest) {
    // Add edge from src to dest
    Node* newNode = createNode(dest);
    newNode->next = graph->adjLists[src];
    graph->adjLists[src] = newNode;
    
    // Since the graph is undirected, add edge from dest to src as well
    newNode = createNode(src);
    newNode->next = graph->adjLists[dest];
    graph->adjLists[dest] = newNode;
}

/* BFS for adjacency list */
void BFS_list(Graph* graph, int start) {
    int n = graph->numVertices;
    bool *visited = calloc(n, sizeof(bool));
    int *queue = malloc(MAX_QUEUE * sizeof(int));
    int front = 0, rear = 0;
    
    visited[start] = true;
    queue[rear++] = start;
    
    printf("BFS (Adjacency List): ");
    while (front < rear) {
        int u = queue[front++];
        printf("%d ", u);
        Node* temp = graph->adjLists[u];
        while (temp != NULL) {
            int v = temp->vertex;
            if (!visited[v]) {
                visited[v] = true;
                queue[rear++] = v;
            }
            temp = temp->next;
        }
    }
    printf("\n");
    free(queue);
    free(visited);
}

/* DFS helper for adjacency list */
void DFS_list_util(Graph* graph, int u, bool *visited) {
    visited[u] = true;
    printf("%d ", u);
    Node* temp = graph->adjLists[u];
    while (temp != NULL) {
        int v = temp->vertex;
        if (!visited[v]) {
            DFS_list_util(graph, v, visited);
        }
        temp = temp->next;
    }
}

/* DFS for adjacency list starting at vertex start */
void DFS_list(Graph* graph, int start) {
    int n = graph->numVertices;
    bool *visited = calloc(n, sizeof(bool));
    printf("DFS (Adjacency List): ");
    DFS_list_util(graph, start, visited);
    printf("\n");
    free(visited);
}

/* Function to free the adjacency list graph */
void freeGraph(Graph* graph) {
    for (int i = 0; i < graph->numVertices; i++) {
        Node* temp = graph->adjLists[i];
        while (temp) {
            Node* next = temp->next;
            free(temp);
            temp = next;
        }
    }
    free(graph->adjLists);
    free(graph);
}

/********************** Main Function **********************/
int main(void) {
    /* Example graph: 5 vertices with the following edges:
         0-1, 0-2, 1-3, 1-4, 2-3, 3-4
       We'll build both representations and run BFS and DFS from vertex 0.
    */
    int n = 5;
    
    /* Adjacency Matrix Representation */
    int matrix[5][5] = {
        {0, 1, 1, 0, 0},
        {1, 0, 0, 1, 1},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 0, 1},
        {0, 1, 0, 1, 0}
    };
    
    printf("=== Using Adjacency Matrix ===\n");
    BFS_matrix(n, matrix, 0);
    DFS_matrix(n, matrix, 0);
    
    /* Adjacency List Representation */
    Graph* graph = createGraph(n);
    addEdge(graph, 0, 1);
    addEdge(graph, 0, 2);
    addEdge(graph, 1, 3);
    addEdge(graph, 1, 4);
    addEdge(graph, 2, 3);
    addEdge(graph, 3, 4);
    
    printf("\n=== Using Adjacency List ===\n");
    BFS_list(graph, 0);
    DFS_list(graph, 0);
    
    freeGraph(graph);
    
    return 0;
}
