/********************************************************************
 * Name         : [Your Name]
 * Roll Number  : [Your Roll Number]
 * Assignment   : 10
 *
 * Description  : This program processes the flight-schedule graph for
 *                Foobarland. The graph is stored as an n×n matrix where
 *                each entry (i,j) stores the ticket price and operator:
 *                  - 'a' if there is an AI flight,
 *                  - 'n' if there is a non-AI flight,
 *                  - '-' if there is no flight (for i ≠ j), and
 *                  - 's' for i == j.
 *
 *                The program then builds three charts:
 *
 *                  C1: Cheapest prices using only AI flights (via Floyd–Warshall).
 *                  C2: Cheapest prices when at most one non-AI leg is allowed.
 *                  C3: Cheapest prices when any number of non-AI legs is allowed.
 *
 *                If an entry is ∞ (no route) it is printed as “–”.
 *
 * Note: No global variables are used.
 ********************************************************************/

 #include <stdio.h>
 #include <stdlib.h>
 
 #define INF 100000000
 
 /* Graph type storing the number of cities, an operator matrix, and a cost matrix */
 typedef struct {
     int n;      // number of cities
     char **op;  // n x n operator matrix
     int **cost; // n x n cost matrix
 } Graph;
 
 /* Function to allocate a new n x n matrix of char */
 char **allocateCharMatrix(int n) {
     char **mat = (char **)malloc(n * sizeof(char *));
     for (int i = 0; i < n; i++) {
         mat[i] = (char *)malloc(n * sizeof(char));
     }
     return mat;
 }
 
 /* Function to allocate a new n x n matrix of int */
 int **allocateIntMatrix(int n) {
     int **mat = (int **)malloc(n * sizeof(int *));
     for (int i = 0; i < n; i++) {
         mat[i] = (int *)malloc(n * sizeof(int));
     }
     return mat;
 }
 
 /* Function: readgraph
    Reads the number of cities and then quadruples (i, j, cost, operator)
    until -1 is entered as i. Returns a pointer to a dynamically allocated graph.
    Vertices are numbered 0 to n-1.
 */
 Graph *readgraph() {
     int n;
     printf("Write n: ");
     scanf("%d", &n);
     
     Graph *G = (Graph *)malloc(sizeof(Graph));
     G->n = n;
     G->op = allocateCharMatrix(n);
     G->cost = allocateIntMatrix(n);
     
     // Initialize the matrices.
     for (int i = 0; i < n; i++) {
         for (int j = 0; j < n; j++) {
             if (i == j) {
                 G->op[i][j] = 's';   // stay put
                 G->cost[i][j] = 0;
             } else {
                 G->op[i][j] = '-';   // no flight
                 G->cost[i][j] = INF;
             }
         }
     }
     
     printf("Write the flight entries (format: i j cost operator), end with -1 as i:\n");
     while (1) {
         int i, j, c;
         char a;
         scanf("%d", &i);
         if (i == -1)
             break;
         scanf("%d %d %c", &j, &c, &a);
         /* Set the entry for the flight from i to j */
         G->op[i][j] = a;
         G->cost[i][j] = c;
     }
     
     return G;
 }
 
 /* Function to print a graph in the required format */
 void printGraph(Graph *G, const char *title) {
     printf("+++ %s\n", title);
     for (int i = 0; i < G->n; i++) {
         printf("%d -> ", i);
         int printed = 0;
         for (int j = 0; j < G->n; j++) {
             if (i != j && G->op[i][j] != '-') {
                 printf("%d (%d, %c) ", j, G->cost[i][j], G->op[i][j]);
                 printed = 1;
             }
         }
         if (!printed)
             printf("None");
         printf("\n");
     }
     printf("\n");
 }
 
 /* Function: getAIgraph
    Returns a new graph that is the subgraph of G consisting only of AI flights.
 */
 Graph *getAIgraph(Graph *G) {
     int n = G->n;
     Graph *H = (Graph *)malloc(sizeof(Graph));
     H->n = n;
     H->op = allocateCharMatrix(n);
     H->cost = allocateIntMatrix(n);
     
     for (int i = 0; i < n; i++) {
         for (int j = 0; j < n; j++) {
             if (i == j) {
                 H->op[i][j] = 's';
                 H->cost[i][j] = 0;
             } else if (G->op[i][j] == 'a') {
                 H->op[i][j] = 'a';
                 H->cost[i][j] = G->cost[i][j];
             } else {
                 H->op[i][j] = '-';
                 H->cost[i][j] = INF;
             }
         }
     }
     return H;
 }
 
 /* Function: APSP
    Implements the Floyd–Warshall algorithm on the graph H.
    Returns an n x n matrix of the cheapest prices.
 */
 int **APSP(Graph *H) {
     int n = H->n;
     int **D = allocateIntMatrix(n);
     
     // Initialize D with the cost matrix of H.
     for (int i = 0; i < n; i++) {
         for (int j = 0; j < n; j++) {
             D[i][j] = H->cost[i][j];
         }
     }
     
     for (int k = 0; k < n; k++) {
         for (int i = 0; i < n; i++) {
             if (D[i][k] == INF) continue;
             for (int j = 0; j < n; j++) {
                 if (D[k][j] == INF) continue;
                 if (D[i][k] + D[k][j] < D[i][j])
                     D[i][j] = D[i][k] + D[k][j];
             }
         }
     }
     return D;
 }
 
 /* Utility function to print an n x n matrix.
    If an entry is INF, prints "-" instead.
    The header prints the city numbers.
 */
 void printMatrix(int **M, int n, const char *title) {
     printf("+++ %s\n", title);
     // Print header
     for (int j = 0; j < n; j++) {
         printf("%-7d", j);
     }
     printf("\n\n");
     for (int i = 0; i < n; i++) {
         printf("%d -> ", i);
         for (int j = 0; j < n; j++) {
             if (M[i][j] == INF)
                 printf("%-7s", "-");
             else
                 printf("%-7d", M[i][j]);
         }
         printf("\n");
     }
     printf("\n");
 }
 
 /* Function: APSPone
    Builds chart C2. For each pair (i, j), if an AI route exists (C1[i][j] < INF)
    then C2[i][j] = C1[i][j]. Otherwise, consider every non-AI flight (k, l) in G.
    If C1[i][k] and C1[l][j] are both finite, compute candidate cost:
        candidate = C1[i][k] + (cost of non-AI flight from k to l) + C1[l][j]
    and take the minimum over all such non-AI flights.
 */
 int **APSPone(Graph *G, int **C1) {
     int n = G->n;
     int **C2 = allocateIntMatrix(n);
     
     for (int i = 0; i < n; i++) {
         for (int j = 0; j < n; j++) {
             if (i == j)
                 C2[i][j] = 0;
             else if (C1[i][j] < INF)
                 C2[i][j] = C1[i][j];
             else {
                 int best = INF;
                 // Try every non-AI flight (k, l)
                 for (int k = 0; k < n; k++) {
                     for (int l = 0; l < n; l++) {
                         if (G->op[k][l] == 'n' && C1[i][k] < INF && C1[l][j] < INF) {
                             int candidate = C1[i][k] + G->cost[k][l] + C1[l][j];
                             if (candidate < best)
                                 best = candidate;
                         }
                     }
                 }
                 C2[i][j] = best;
             }
         }
     }
     return C2;
 }
 
 /* Function: APSPany
    Builds chart C3. For each pair (i,j), if an AI route exists (C1[i][j] < INF),
    then C3[i][j] = C1[i][j]. Otherwise, we compute the APSP on the entire graph G
    (which uses both AI and non-AI flights). Then, for pairs with no AI route, we use
    the computed value from the full-flight APSP.
 */
 int **APSPany(Graph *G, int **C1) {
     int n = G->n;
     int **D = allocateIntMatrix(n);
     
     // Build the complete flight matrix D from G.
     for (int i = 0; i < n; i++) {
         for (int j = 0; j < n; j++) {
             if (i == j)
                 D[i][j] = 0;
             else if (G->op[i][j] != '-')
                 D[i][j] = G->cost[i][j];
             else
                 D[i][j] = INF;
         }
     }
     
     // Run Floyd–Warshall on D.
     for (int k = 0; k < n; k++) {
         for (int i = 0; i < n; i++) {
             if (D[i][k] == INF) continue;
             for (int j = 0; j < n; j++) {
                 if (D[k][j] == INF) continue;
                 if (D[i][k] + D[k][j] < D[i][j])
                     D[i][j] = D[i][k] + D[k][j];
             }
         }
     }
     
     // Build C3: if an AI route exists (C1 finite) then use it; otherwise, use D.
     int **C3 = allocateIntMatrix(n);
     for (int i = 0; i < n; i++) {
         for (int j = 0; j < n; j++) {
             if (C1[i][j] < INF)
                 C3[i][j] = C1[i][j];
             else
                 C3[i][j] = D[i][j];
         }
     }
     
     // Free the temporary matrix D.
     for (int i = 0; i < n; i++) {
         free(D[i]);
     }
     free(D);
     
     return C3;
 }
 
 /* Main function */
 int main() {
     /* Part 1: Read the graph */
     Graph *G = readgraph();
     printf("\n+++ Original graph\n");
     printGraph(G, "Original graph");
     
     /* Part 2: Build the AI subgraph */
     Graph *H = getAIgraph(G);
     printGraph(H, "AI subgraph");
     
     /* Part 3: APSP in the AI subgraph (chart C1) */
     int **C1 = APSP(H);
     printMatrix(C1, G->n, "Cheapest AI prices");
     
     /* Part 4: APSP with at most one non-AI leg allowed (chart C2) */
     int **C2 = APSPone(G, C1);
     printMatrix(C2, G->n, "Cheapest prices with at most one non-AI leg");
     
     /* Part 5: APSP with any number of non-AI legs allowed (chart C3) */
     int **C3 = APSPany(G, C1);
     printMatrix(C3, G->n, "Cheapest prices with any number of non-AI legs");
     
     /* Free allocated memory */
     for (int i = 0; i < G->n; i++) {
         free(G->op[i]);
         free(G->cost[i]);
         free(H->op[i]);
         free(H->cost[i]);
         free(C1[i]);
         free(C2[i]);
         free(C3[i]);
     }
     free(G->op);
     free(G->cost);
     free(H->op);
     free(H->cost);
     free(G);
     free(H);
     free(C1);
     free(C2);
     free(C3);
     
     return 0;
 } 