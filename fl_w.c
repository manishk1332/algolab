#include <stdio.h>
#include <limits.h>
#include <stdbool.h>

#define V 4
#define INF 100000000  // A large value to represent infinity

// Floyd–Warshall algorithm with path reconstruction.
// If a path is affected by a negative cycle, the corresponding distance is marked as -INF.
void floydWarshallWithPath(int graph[V][V]) {
    int dist[V][V];      // Distance matrix
    int next[V][V];      // Next matrix for path reconstruction
    bool negative[V][V]; // Matrix to mark paths affected by negative cycles

    // Initialize the matrices.
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            dist[i][j] = graph[i][j];
            if (i == j)
                dist[i][j] = 0;
            if (graph[i][j] != INF)
                next[i][j] = j;
            else
                next[i][j] = -1;
            negative[i][j] = false;
        }
    }

    // Standard Floyd–Warshall updates.
    for (int k = 0; k < V; k++) {
        for (int i = 0; i < V; i++) {
            for (int j = 0; j < V; j++) {
                if (dist[i][k] != INF && dist[k][j] != INF &&
                    dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    next[i][j] = next[i][k];
                }
            }
        }
    }

    // Detect negative cycles.
    // If any vertex k has a negative self-distance, then any path that can reach k (and from k) is affected.
    for (int k = 0; k < V; k++) {
        if (dist[k][k] < 0) {
            for (int i = 0; i < V; i++) {
                for (int j = 0; j < V; j++) {
                    if (dist[i][k] != INF && dist[k][j] != INF)
                        negative[i][j] = true;
                }
            }
        }
    }

    // Print the distance matrix.
    printf("Floyd-Warshall distance matrix:\n");
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (negative[i][j])
                printf("%7s", "-INF");
            else if (dist[i][j] == INF)
                printf("%7s", "INF");
            else
                printf("%7d", dist[i][j]);
        }
        printf("\n");
    }

    // Function to print the path from node 'u' to node 'v' using the next matrix.
    void printPathFloyd(int u, int v) {
        if (next[u][v] == -1) {
            printf("No path exists from %d to %d\n", u, v);
            return;
        }
        if (negative[u][v]) {
            printf("Path from %d to %d is affected by a negative cycle (distance: -INF)\n", u, v);
            return;
        }
        printf("Path from %d to %d: %d", u, v, u);
        while (u != v) {
            u = next[u][v];
            printf(" -> %d", u);
        }
        printf("\n");
    }

    // Get start and end nodes from the user.
    int start, end;
    printf("\nEnter start and end nodes: ");
    scanf("%d %d", &start, &end);
    
    printPathFloyd(start, end);
}

int main() {
    // Define a sample graph (as an adjacency matrix) with 4 vertices.
    // Use INF where no direct edge exists.
    int graph[V][V] = {
        {0,   3,   INF, 2},
        {INF, 0,   -2,  INF},
        {INF, INF, 0,   2},
        {1,   INF, INF, 0}
    };

    // Introduce a negative cycle:
    // Changing the weight from vertex 3 to 0 to -2 creates a cycle:
    // 0 -> 1 -> 2 -> 3 -> 0 with total weight: 3 + (-5) + 2 + (-2) = -2.
    graph[3][0] = -2;
    
    floydWarshallWithPath(graph);
    
    return 0;
}
