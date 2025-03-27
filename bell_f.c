#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

#define INF INT_MAX

// Structure to represent an edge in the graph.
typedef struct {
    int src, dest, weight;
} Edge;

// Structure to represent a graph.
typedef struct {
    int V, E;
    Edge* edge;
} Graph;

// Function to create a graph with V vertices and E edges.
Graph* createGraph(int V, int E) {
    Graph* graph = (Graph*) malloc(sizeof(Graph));
    graph->V = V;
    graph->E = E;
    graph->edge = (Edge*) malloc(E * sizeof(Edge));
    return graph;
}

// Bellman-Ford algorithm that computes shortest paths from src.
// It prints "-INF" for vertices whose distances are affected by a negative cycle.
void bellmanFord(Graph* graph, int src) {
    int V = graph->V;
    int E = graph->E;
    
    // Allocate memory for distances, parent, and a marker for negative cycle.
    int *dist = (int*) malloc(V * sizeof(int));
    int *parent = (int*) malloc(V * sizeof(int));
    bool *negative = (bool*) malloc(V * sizeof(bool));
    
    // Initialization
    for (int i = 0; i < V; i++) {
        dist[i] = INF;
        parent[i] = -1;
        negative[i] = false;
    }
    dist[src] = 0;

    // Relax all edges V-1 times.
    for (int i = 1; i <= V - 1; i++) {
        for (int j = 0; j < E; j++) {
            int u = graph->edge[j].src;
            int v = graph->edge[j].dest;
            int weight = graph->edge[j].weight;
            if (dist[u] != INF && dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                parent[v] = u;
            }
        }
    }
    
    // Check for negative-weight cycles.
    // Mark any vertex v that can still be relaxed.
    for (int j = 0; j < E; j++) {
        int u = graph->edge[j].src;
        int v = graph->edge[j].dest;
        int weight = graph->edge[j].weight;
        if (dist[u] != INF && dist[u] + weight < dist[v]) {
            negative[v] = true;
        }
    }
    
    // Propagate the negative cycle effect to all vertices reachable from a negative cycle.
    for (int i = 0; i < V - 1; i++) {
        for (int j = 0; j < E; j++) {
            int u = graph->edge[j].src;
            int v = graph->edge[j].dest;
            if (negative[u])
                negative[v] = true;
        }
    }
    
    // Helper function to print the path from src to v.
    // (Only prints if there is no negative cycle affecting the vertex.)
    void printPath(int parent[], int v) {
        if (v == -1)
            return;
        printPath(parent, parent[v]);
        if (parent[v] != -1)
            printf(" -> ");
        printf("%d", v);
    }
    
    // Print the result.
    printf("Bellman-Ford (Single Source Shortest Paths) from source %d:\n", src);
    printf("Vertex\tDistance\tPath\n");
    for (int i = 0; i < V; i++) {
        printf("%d\t", i);
        if (negative[i]) {
            printf("-INF\t\t");
        } else if (dist[i] == INF) {
            printf("INF\t\t");
        } else {
            printf("%d\t\t", dist[i]);
        }
        if (i == src)
            printf("%d", src);
        else if (dist[i] != INF && !negative[i])
            printPath(parent, i);
        else
            printf("No path");
        printf("\n");
    }
    
    free(dist);
    free(parent);
    free(negative);
}

int main() {
    // Example graph with 5 vertices and 8 edges.
    // This graph contains a negative cycle that affects vertices 1 and 3.
    int V = 5, E = 8;
    Graph* graph = createGraph(V, E);

    // Define the edges: {source, destination, weight}
    graph->edge[0] = (Edge){0, 1, 6};
    graph->edge[1] = (Edge){0, 2, 7};
    graph->edge[2] = (Edge){1, 2, 8};
    graph->edge[3] = (Edge){1, 3, 5};
    graph->edge[4] = (Edge){1, 4, -4};
    graph->edge[5] = (Edge){2, 3, -3};
    graph->edge[6] = (Edge){2, 4, 9};
    // Edge that introduces a negative cycle:
    graph->edge[7] = (Edge){3, 1, -10};  // Cycle: 1 -> 3 -> 1 gives a total weight of -5.

    bellmanFord(graph, 0);

    free(graph->edge);
    free(graph);
    return 0;
}