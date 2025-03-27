/********************************************************************
 * Name         : [Your Name]
 * Roll Number  : [Your Roll Number]
 * Assignment   : 9
 *
 * Description  : This program reads an undirected graph whose vertices 
 *                are colored red (r) or blue (b). The graph is stored 
 *                in an adjacency-list representation along with an array 
 *                of vertex colors and original vertex numbers.
 *
 *                It then:
 *                  (a) Prints the original graph.
 *                  (b) Builds and prints the red subgraph and blue subgraph.
 *                  (c) Runs DFS on each subgraph, printing cycles (i.e.,
 *                      back edges) along with the vertex colors.
 *                  (d) Constructs the graph GRB = (V, FRR ∪ FBB ∪ ERB) where
 *                      FRR and FBB are the DFS forest edges in the red and blue
 *                      subgraphs and ERB is the set of edges joining vertices of 
 *                      different colors.
 *                  (e) Runs DFS on GRB to detect (nonmonochromatic) cycles.
 *
 *                No global or static variables are used.
 ********************************************************************/

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 
 #define MAX_CYCLE 100
 
 /* Structure for an adjacency-list node */
 typedef struct AdjListNode {
     int vertex;                 // Index of the neighbor (in the graph's numbering)
     struct AdjListNode *next;
 } AdjListNode;
 
 /* Graph data type */
 typedef struct Graph {
     int n;                      // Number of vertices in the graph
     char *colors;               // Array of vertex colors (of size n), 'r' or 'b'
     int *orig;                  // For subgraphs: original vertex numbers (for G, orig[i] = i)
     AdjListNode **adj;          // Array (size n) of pointers to adjacency-list nodes
 } Graph;
 
 /* Function prototypes */
 Graph *createGraph(int n);
 void addEdge(Graph *G, int u, int v);
 void prngraph(Graph *G, const char *title);
 void freeGraph(Graph *G);
 Graph *readgraph(void);
 Graph *getcolgraph(Graph *G, char col);
 void DFS(Graph *G, int u, int parent, int level, int *visited, int *parentArr, int *levelArr);
 int *multiDFS(Graph *G);
 void printCycle(int *parentArr, int u, int v, Graph *G);
 Graph *getrbgraph(Graph *G, Graph *GR, int *parentR, Graph *GB, int *parentB);
 
 /*----------------------- Graph Construction -------------------------*/
 
 /* createGraph: Allocates a graph with n vertices */
 Graph *createGraph(int n) {
     Graph *G = (Graph *)malloc(sizeof(Graph));
     G->n = n;
     G->colors = (char *)malloc(n * sizeof(char));
     G->orig = (int *)malloc(n * sizeof(int));
     G->adj = (AdjListNode **)malloc(n * sizeof(AdjListNode *));
     for (int i = 0; i < n; i++) {
         G->orig[i] = i;      // For the original graph, the vertex number equals the index.
         G->adj[i] = NULL;
     }
     return G;
 }
 
 /* addEdge: Adds an undirected edge between vertices u and v (u and v are indices in G) */
 void addEdge(Graph *G, int u, int v) {
     // Add v to u's list
     AdjListNode *newNode = (AdjListNode *)malloc(sizeof(AdjListNode));
     newNode->vertex = v;
     newNode->next = G->adj[u];
     G->adj[u] = newNode;
     
     // Add u to v's list
     newNode = (AdjListNode *)malloc(sizeof(AdjListNode));
     newNode->vertex = u;
     newNode->next = G->adj[v];
     G->adj[v] = newNode;
 }
 
 /* prngraph: Prints the graph in the format shown in sample I/O */
 void prngraph(Graph *G, const char *title) {
     printf("+++ %s\n", title);
     for (int i = 0; i < G->n; i++) {
         printf("[%c] %d -> ", G->colors[i], G->orig[i]);
         AdjListNode *temp = G->adj[i];
         if (!temp) {
             printf("None");
         } else {
             int first = 1;
             while (temp) {
                 if (!first)
                     printf(", ");
                 printf("%d", G->orig[temp->vertex]);
                 first = 0;
                 temp = temp->next;
             }
         }
         printf("\n");
     }
     printf("\n");
 }
 
 /* freeGraph: Frees all memory allocated for the graph */
 void freeGraph(Graph *G) {
     for (int i = 0; i < G->n; i++) {
         AdjListNode *temp = G->adj[i];
         while (temp) {
             AdjListNode *next = temp->next;
             free(temp);
             temp = next;
         }
     }
     free(G->adj);
     free(G->colors);
     free(G->orig);
     free(G);
 }
 
 /* readgraph: Reads the graph from user input.
    First, the user enters the number of vertices.
    Then the colors of the vertices (separated by spaces).
    Then the list of edges, each specified by a pair (u, v).
    Input ends when -1 is entered as u.
 */
 Graph *readgraph(void) {
     int n;
     printf("Enter number of vertices: ");
     scanf("%d", &n);
     Graph *G = createGraph(n);
     
     printf("Enter the colors of vertices (r or b) separated by spaces:\n");
     for (int i = 0; i < n; i++) {
         scanf(" %c", &G->colors[i]);
     }
     
     printf("Enter the edges (u v) (undirected). End with -1 as u:\n");
     while (1) {
         int u, v;
         scanf("%d", &u);
         if (u == -1)
             break;
         scanf("%d", &v);
         if (u < 0 || u >= n || v < 0 || v >= n) {
             printf("Invalid edge. Try again.\n");
             continue;
         }
         addEdge(G, u, v);
     }
     
     return G;
 }
 
 /*------------------- Subgraph Construction --------------------------*/
 
 /* getcolgraph: Given graph G and a color (r or b), constructs and returns the induced
    subgraph consisting of vertices of that color. The new graph’s vertices are renumbered
    from 0 to m-1; the original vertex numbers are stored in the orig array.
 */
 Graph *getcolgraph(Graph *G, char col) {
     int count = 0;
     for (int i = 0; i < G->n; i++) {
         if (G->colors[i] == col)
             count++;
     }
     Graph *H = createGraph(count);
     H->n = count;
     // Allocate new colors and orig arrays for H
     free(H->colors); 
     H->colors = (char *)malloc(count * sizeof(char));
     free(H->orig);
     H->orig = (int *)malloc(count * sizeof(int));
     
     // Map original vertex number to new index; initialize mapping array (size = G->n)
     int *mapping = (int *)malloc(G->n * sizeof(int));
     for (int i = 0; i < G->n; i++)
         mapping[i] = -1;
     
     int idx = 0;
     for (int i = 0; i < G->n; i++) {
         if (G->colors[i] == col) {
             mapping[i] = idx;
             H->colors[idx] = col;
             H->orig[idx] = i; // store original number
             idx++;
         }
     }
     
     // For each edge in G between vertices of the given color, add edge to H (avoid duplicates)
     for (int i = 0; i < G->n; i++) {
         if (G->colors[i] == col) {
             AdjListNode *temp = G->adj[i];
             while (temp) {
                 int j = temp->vertex;
                 if (G->colors[j] == col && i < j) {
                     // add edge between mapping[i] and mapping[j]
                     addEdge(H, mapping[i], mapping[j]);
                 }
                 temp = temp->next;
             }
         }
     }
     free(mapping);
     return H;
 }
 
 /*------------------ DFS and Cycle Detection ---------------------------*/
 
 /* printCycle: Given a back edge from u to an ancestor v (i.e. level[v] < level[u]),
    this function reconstructs and prints the cycle by following the parent array.
    The cycle is printed along with the colors of its vertices.
 */
 void printCycle(int *parentArr, int u, int v, Graph *G) {
     int cycle[MAX_CYCLE];
     int len = 0;
     int cur = u;
     // Collect vertices from u up to v (inclusive)
     while (cur != v && cur != -1 && len < MAX_CYCLE) {
         cycle[len++] = cur;
         cur = parentArr[cur];
     }
     if (cur == -1) return; // safety check
     cycle[len++] = v;
     
     // Print cycle in order (starting at u, ending at v)
     printf("(");
     for (int i = 0; i < len; i++) {
         printf("%d", G->orig ? G->orig[cycle[i]] : cycle[i]); // if orig exists, print original number
         if (i < len - 1)
             printf(", ");
     }
     printf("), Colors: (");
     for (int i = 0; i < len; i++) {
         printf("%c", G->colors[cycle[i]]);
         if (i < len - 1)
             printf(", ");
     }
     printf(")\n");
 }
 
 /* DFS: Recursive DFS that marks visited vertices, records parent and level, and
    whenever a back edge is found (to an ancestor), prints the cycle.
 */
 void DFS(Graph *G, int u, int parent, int level, int *visited, int *parentArr, int *levelArr) {
     visited[u] = 1;
     parentArr[u] = parent;
     levelArr[u] = level;
     
     AdjListNode *temp = G->adj[u];
     while (temp) {
         int v = temp->vertex;
         if (!visited[v]) {
             DFS(G, v, u, level + 1, visited, parentArr, levelArr);
         } else if (v != parent && levelArr[v] < levelArr[u]) {
             // Back edge found from u to v; print the cycle.
             printCycle(parentArr, u, v, G);
         }
         temp = temp->next;
     }
 }
 
 /* multiDFS: Performs DFS on all vertices of graph G (in case G is disconnected).
    It prints cycles detected (via back edges) and returns the parent array (of size G->n)
    representing the DFS forest.
 */
 int *multiDFS(Graph *G) {
     int n = G->n;
     int *visited = (int *)calloc(n, sizeof(int));
     int *parentArr = (int *)malloc(n * sizeof(int));
     int *levelArr = (int *)malloc(n * sizeof(int));
     for (int i = 0; i < n; i++) {
         parentArr[i] = -1;
         levelArr[i] = -1;
     }
     for (int i = 0; i < n; i++) {
         if (!visited[i]) {
             // For each new tree in the DFS forest, start at level 0.
             DFS(G, i, -1, 0, visited, parentArr, levelArr);
         }
     }
     free(visited);
     free(levelArr);
     return parentArr;
 }
 
 /*------------------ Construction of GRB ------------------------------*/
 /* getrbgraph: Constructs and returns the graph GRB = (V, FRR ∪ FBB ∪ ERB)
    where V is the vertex set of the original graph G;
    FRR: DFS forest edges from the red subgraph (GR)
    FBB: DFS forest edges from the blue subgraph (GB)
    ERB: All edges from G whose endpoints have different colors.
    
    Arguments:
      G       : Original graph.
      GR      : Red subgraph (with vertices re-indexed; GR->orig stores original numbers).
      parentR : Parent array from multiDFS on GR.
      GB      : Blue subgraph.
      parentB : Parent array from multiDFS on GB.
 */
 Graph *getrbgraph(Graph *G, Graph *GR, int *parentR, Graph *GB, int *parentB) {
     int n = G->n;
     Graph *R = createGraph(n);
     // For GRB, vertices remain numbered 0 to n-1; copy colors and orig from G.
     for (int i = 0; i < n; i++) {
         R->colors[i] = G->colors[i];
         R->orig[i] = i;
     }
     
     // Helper function to add an edge if not already present.
     // (We simply add edges; duplicate edges are acceptable for our purposes.)
     
     // Add DFS forest edges from the red subgraph GR.
     for (int i = 0; i < GR->n; i++) {
         if (parentR[i] != -1) {
             int u = GR->orig[i];
             int v = GR->orig[parentR[i]];
             addEdge(R, u, v);
         }
     }
     // Add DFS forest edges from the blue subgraph GB.
     for (int i = 0; i < GB->n; i++) {
         if (parentB[i] != -1) {
             int u = GB->orig[i];
             int v = GB->orig[parentB[i]];
             addEdge(R, u, v);
         }
     }
     // Add all nonmonochromatic edges from G (edges joining vertices of different colors).
     for (int u = 0; u < G->n; u++) {
         AdjListNode *temp = G->adj[u];
         while (temp) {
             int v = temp->vertex;
             if (u < v && G->colors[u] != G->colors[v]) {
                 addEdge(R, u, v);
             }
             temp = temp->next;
         }
     }
     return R;
 }
 
 /*--------------------------- main() -----------------------------------*/
 
 int main(void) {
     /* Part 1: Read and print the original graph G */
     Graph *G = readgraph();
     prngraph(G, "Original graph (G)");
     
     /* Part 2: Get and print the red and blue subgraphs */
     Graph *GR = getcolgraph(G, 'r');
     prngraph(GR, "Red subgraph (GR)");
     
     Graph *GB = getcolgraph(G, 'b');
     prngraph(GB, "Blue subgraph (GB)");
     
     /* Part 3: DFS traversal on GR and GB to detect cycles.
        multiDFS prints the cycles (back-edge cycles) and returns the DFS forest (parent array).
     */
     printf("+++ Red cycles\n");
     int *parentR = multiDFS(GR);
     // (If no cycle, nothing is printed.)
     
     printf("+++ Blue cycles\n");
     int *parentB = multiDFS(GB);
     
     /* Part 4: Construct the nonmonochromatic graph GRB.
        GRB is built from the DFS forest edges of GR and GB and the edges connecting vertices
        of different colors in G.
     */
     Graph *GRB = getrbgraph(G, GR, parentR, GB, parentB);
     prngraph(GRB, "Nonmonochromatic graph (GRB)");
     
     /* Part 5: Run DFS on GRB to detect nonmonochromatic cycles */
     printf("+++ Multi-color cycles\n");
     (void) multiDFS(GRB);
     
     /* Free all allocated memory */
     free(parentR);
     free(parentB);
     freeGraph(G);
     freeGraph(GR);
     freeGraph(GB);
     freeGraph(GRB);
     
     return 0;
 } 