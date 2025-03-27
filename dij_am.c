#include <stdio.h>
#include <limits.h>
#include <stdbool.h>

#define V 9            // Number of vertices in the graph
#define INF INT_MAX    // A large value to represent infinity

// Function to find the vertex with the minimum distance value from
// the set of vertices not yet included in the shortest path tree.
int minDistance(int dist[], bool sptSet[]) {
    int min = INF, min_index = -1;
    for (int v = 0; v < V; v++) {
        if (!sptSet[v] && dist[v] <= min) {
            min = dist[v];
            min_index = v;
        }
    }
    return min_index;
}

// Recursive function to print the path from source to j using the parent array.
void printPath(int parent[], int j) {
    if (parent[j] == -1) {
        printf("%d", j);
        return;
    }
    printPath(parent, parent[j]);
    printf(" -> %d", j);
}

// Dijkstra's algorithm using adjacency matrix representation.
void dijkstraMatrix(int graph[V][V], int src) {
    int dist[V];      // dist[i] will hold the shortest distance from src to i.
    bool sptSet[V];   // sptSet[i] true if vertex i is finalized.
    int parent[V];    // Array to store the shortest path tree.

    // Initialize all distances as INFINITE, sptSet[] as false, and parent as -1.
    for (int i = 0; i < V; i++) {
        dist[i] = INF;
        sptSet[i] = false;
        parent[i] = -1;
    }
    dist[src] = 0; // Distance of source vertex from itself is 0.

    // Find shortest path for all vertices.
    for (int count = 0; count < V - 1; count++) {
        int u = minDistance(dist, sptSet);
        if(u == -1) break; // Graph not fully connected.
        sptSet[u] = true;

        // Update distance value of the adjacent vertices of the picked vertex.
        for (int v = 0; v < V; v++) {
            if (!sptSet[v] && graph[u][v] && dist[u] != INF 
                && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
                parent[v] = u;  // Set u as parent of v.
            }
        }
    }

    // Print the constructed distance array and paths.
    printf("Dijkstra (Adjacency Matrix) from source %d:\n", src);
    printf("Vertex \t Distance \t Path\n");
    for (int i = 0; i < V; i++) {
        printf("%d \t\t ", i);
        if(dist[i] == INF)
            printf("INF \t\t No path\n");
        else {
            printf("%d \t\t ", dist[i]);
            printPath(parent, i);
            printf("\n");
        }
    }
}

int main() {
    int graph[V][V] = {
      {0, 4, 0, 0, 0, 0, 0, 8, 0},
      {4, 0, 8, 0, 0, 0, 0, 11, 0},
      {0, 8, 0, 7, 0, 4, 0, 0, 2},
      {0, 0, 7, 0, 9, 14, 0, 0, 0},
      {0, 0, 0, 9, 0, 10, 0, 0, 0},
      {0, 0, 4, 14, 10, 0, 2, 0, 0},
      {0, 0, 0, 0, 0, 2, 0, 1, 6},
      {8, 11, 0, 0, 0, 0, 1, 0, 7},
      {0, 0, 2, 0, 0, 0, 6, 7, 0}
    };

    dijkstraMatrix(graph, 0);
    return 0;
}