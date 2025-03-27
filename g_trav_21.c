#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/* Structure for an adjacency list node */
typedef struct Node {
    int vertex;
    int cost;
    struct Node* next;
} Node;

/* Structure for a graph (using 1-indexed vertices) */
typedef struct {
    int V;      // Number of vertices
    Node** array;
} Graph;

/* Structure for an edge (used for the brute-force method) */
typedef struct {
    int u;
    int v;
    int cost;
} Edge;

/* Function to create a new adjacency list node */
Node* createNode(int vertex, int cost) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->vertex = vertex;
    newNode->cost = cost;
    newNode->next = NULL;
    return newNode;
}

/* Function to create a graph with V vertices */
Graph* createGraph(int V) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->V = V;
    // Allocate array for vertices 1..V
    graph->array = (Node**)malloc((V + 1) * sizeof(Node*));
    for (int i = 1; i <= V; i++) {
        graph->array[i] = NULL;
    }
    return graph;
}

/* Function to add an undirected edge to the graph */
void addEdge(Graph* graph, int u, int v, int cost) {
    // Add edge from u to v
    Node* newNode = createNode(v, cost);
    newNode->next = graph->array[u];
    graph->array[u] = newNode;
    
    // Add edge from v to u
    newNode = createNode(u, cost);
    newNode->next = graph->array[v];
    graph->array[v] = newNode;
}

/*-----------------------------------------------------------
Brute Force Method (O(n²)):
For each edge, remove it (simulate removal by skipping it
during DFS), compute the sum of vertex weights in one
connected component, and then compute vulnerability:

    γ(e) = cost(e) - | sum(component) - (totalSum - sum(component)) |
-----------------------------------------------------------*/

/* DFS function that computes the sum of weights for the component
reachable from 'current'. The edge (exclude_u, exclude_v) is skipped.
'visited' is an array (1-indexed) and comp_sum accumulates the sum. */
void dfs_exclude(Graph* graph, int current, int exclude_u, int exclude_v,
                int* visited, int* vertexWeights, int* comp_sum) {
    visited[current] = 1;
    *comp_sum += vertexWeights[current];
    
    Node* temp = graph->array[current];
    while (temp != NULL) {
        int adj = temp->vertex;
        // Skip the edge if it is the excluded edge (in either direction)
        if ((current == exclude_u && adj == exclude_v) ||
            (current == exclude_v && adj == exclude_u)) {
            temp = temp->next;
            continue;
        }
        if (!visited[adj]) {
            dfs_exclude(graph, adj, exclude_u, exclude_v, visited, vertexWeights, comp_sum);
        }
        temp = temp->next;
    }
}

/* Brute force method to compute the edge with the best vulnerability.
'edges' is the array of n-1 edges, total_sum is the sum of all vertex weights. */
void bruteForceMethod(Graph* graph, Edge* edges, int numEdges, int n,
                    int* vertexWeights, int total_sum) {
    int best_vuln = INT_MIN;
    int best_u = -1, best_v = -1;
    
    for (int i = 0; i < numEdges; i++) {
        int u = edges[i].u;
        int v = edges[i].v;
        int cost = edges[i].cost;
        
        // Create a visited array for DFS (1-indexed)
        int* visited = (int*)calloc(n + 1, sizeof(int));
        int comp_sum = 0;
        // Start DFS from u while excluding the edge (u, v)
        dfs_exclude(graph, u, u, v, visited, vertexWeights, &comp_sum);
        free(visited);
        
        int other_sum = total_sum - comp_sum;
        int diff = comp_sum - other_sum;
        if (diff < 0)
            diff = -diff;
        int vuln = cost - diff;
        
        if (vuln > best_vuln) {
            best_vuln = vuln;
            best_u = u;
            best_v = v;
        }
    }
    
    printf("Edge with the smallest vulnerability computed using first method: %d %d\n", best_u, best_v);
}

/*-----------------------------------------------------------
Linear Time Algorithm using DFS:
We choose an arbitrary root (vertex 1) and compute the sum of
the weights in every subtree. For each edge (u,v) where v is a
child of u, one component has sum = S (subtree sum of v) and
the other component has sum = total_sum - S.
Vulnerability is computed as:

    γ(e) = cost(e) - | S - (total_sum - S) |

This DFS-based approach runs in O(n) time.
-----------------------------------------------------------*/
int dfs_tree(Graph* graph, int u, int parent, int* vertexWeights, int total_sum,
            int* best_vuln, int* best_u, int* best_v) {
    int subtree_sum = vertexWeights[u];
    Node* temp = graph->array[u];
    while (temp != NULL) {
        int v = temp->vertex;
        int edge_cost = temp->cost;
        if (v == parent) {
            temp = temp->next;
            continue;
        }
        int child_sum = dfs_tree(graph, v, u, vertexWeights, total_sum, best_vuln, best_u, best_v);
        int diff = child_sum - (total_sum - child_sum);
        if (diff < 0)
            diff = -diff;
        int vuln = edge_cost - diff;
        if (vuln > *best_vuln) {
            *best_vuln = vuln;
            *best_u = u;
            *best_v = v;
        }
        subtree_sum += child_sum;
        temp = temp->next;
    }
    return subtree_sum;
}

/*-----------------------------------------------------------
Main function:
1. Reads number of vertices, weights, and edges (n-1).
2. Builds the graph (adjacency list) and stores edges.
3. Computes total weight sum.
4. Uses both methods to compute the edge with smallest vulnerability.
-----------------------------------------------------------*/
int main() {
    int n;
    printf("Write n: ");
    scanf("%d", &n);
    
    // Allocate vertex weights array (1-indexed)
    int* vertexWeights = (int*)malloc((n + 1) * sizeof(int));
    printf("Write weights of vertices: ");
    for (int i = 1; i <= n; i++) {
        scanf("%d", &vertexWeights[i]);
    }
    
    // There are exactly n-1 edges in a tree.
    int numEdges = n - 1;
    Edge* edges = (Edge*)malloc(numEdges * sizeof(Edge));
    
    // Create graph (1-indexed vertices)
    Graph* graph = createGraph(n);
    
    printf("Write the edges and their weights (format: u v cost):\n");
    for (int i = 0; i < numEdges; i++) {
        int u, v, cost;
        scanf("%d %d %d", &u, &v, &cost);
        edges[i].u = u;
        edges[i].v = v;
        edges[i].cost = cost;
        addEdge(graph, u, v, cost);
    }
    
    // Compute total sum of vertex weights
    int total_sum = 0;
    for (int i = 1; i <= n; i++) {
        total_sum += vertexWeights[i];
    }
    
    // Compute using Brute-force method
    bruteForceMethod(graph, edges, numEdges, n, vertexWeights, total_sum);
    
    // Compute using Linear-Time (DFS) method
    int best_vuln = INT_MIN;
    int best_u = -1, best_v = -1;
    // Start DFS from vertex 1 (assuming vertices are connected)
    dfs_tree(graph, 1, -1, vertexWeights, total_sum, &best_vuln, &best_u, &best_v);
    printf("Edge with the smallest vulnerability computed using second method: %d %d\n", best_u, best_v);
    
    /* Free allocated memory */
    // Free the adjacency list nodes
    for (int i = 1; i <= n; i++) {
        Node* temp = graph->array[i];
        while (temp != NULL) {
            Node* next = temp->next;
            free(temp);
            temp = next;
        }
    }
    free(graph->array);
    free(graph);
    free(edges);
    free(vertexWeights);
    
    return 0;
}
