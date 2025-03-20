/*
 * Name         : Your Name
 * Roll Number  : YourRollNumber
 * Assignment   : 8
 *
 * Description:
 *   This program implements a multi-heap (max-heap) in which each node stores up to p keys.
 *   The multi-heap is stored in a contiguous array of nodes. Except for possibly the last node,
 *   every node is full (contains exactly p keys). The heap-ordering property is maintained:
 *   for every key k in any node and every key k' in any of its child nodes, k > k'.
 *
 *   The following functions are implemented:
 *     - initheap(p, nmax): Initializes an empty multi-heap with node capacity p and room for at most nmax keys.
 *     - insert(H, x): Inserts key x into the multi-heap H. If the insertion disturbs the ordering property,
 *                     it is repaired by "bubbling up" using the following rule:
 *                       Combine the keys of the current node (q) and its parent (r),
 *                       then assign the largest p keys to r and the rest to q.
 *     - findmax(H): Returns the maximum key from the root node (linear scan over up to p keys).
 *     - heapify(H, i): Repairs the heap–ordering property starting at node i by checking its child nodes.
 *     - delmax(H): Removes the maximum key from the root node. If the root is not the last node,
 *                  a key from the last node is moved to the root and then heapify is called.
 *     - prnheap(H): Prints the multi-heap, one node per line.
 *
 *   The main() function reads the node capacity p and the total number n of keys.
 *   It then reads n keys into an array, inserts them one by one into the multi-heap, and prints the
 *   multi-heap. Next, it repeatedly finds and deletes the maximum key (storing the deletions in reverse
 *   order) until the heap is empty, and then prints the sorted array.
 *
 * Compile with: gcc -o multiheap multiheap.c -lm
 */

 #include <stdio.h>
 #include <stdlib.h>
 
 /* ---------------- Data Structures ---------------- */
 
 /* Each node stores an array of keys and a count of keys currently in the node */
 typedef struct {
     int *keys;   // dynamic array of keys (size = p)
     int count;   // number of keys currently stored in this node
 } Node;
 
 /* The multi-heap structure */
 typedef struct {
     Node *nodes; // array of nodes (the heap is stored in contiguous representation)
     int p;       // node capacity (maximum keys per node)
     int nmax;    // maximum number of keys that the heap can store
     int n;       // current total number of keys in the heap
     int N;       // current number of nodes used (always N = ceil(n/p))
 } MultiHeap;
 
 /* ---------------- Utility Function ---------------- */
 
 /* Comparator for qsort in descending order */
 int cmp_desc(const void *a, const void *b) {
     int ia = *(const int *)a;
     int ib = *(const int *)b;
     return ib - ia; // descending order
 }
 
 /* ---------------- Multi-Heap Functions ---------------- */
 
 /* initheap: Allocate and initialize a multi-heap with node capacity p and maximum nmax keys */
 MultiHeap* initheap(int p, int nmax) {
     MultiHeap *H = (MultiHeap *) malloc(sizeof(MultiHeap));
     if (!H) {
         fprintf(stderr, "Memory allocation error\n");
         exit(1);
     }
     H->p = p;
     H->nmax = nmax;
     H->n = 0;
     H->N = 0;
     /* Maximum possible number of nodes = ceil(nmax / p) */
     int maxNodes = (nmax + p - 1) / p;
     H->nodes = (Node *) malloc(sizeof(Node) * maxNodes);
     if (!H->nodes) {
         fprintf(stderr, "Memory allocation error\n");
         exit(1);
     }
     for (int i = 0; i < maxNodes; i++) {
         H->nodes[i].keys = (int *) malloc(sizeof(int) * p);
         if (!H->nodes[i].keys) {
             fprintf(stderr, "Memory allocation error\n");
             exit(1);
         }
         H->nodes[i].count = 0;
     }
     return H;
 }
 
 /* insert: Insert key x into the multi-heap H */
 void insert(MultiHeap *H, int x) {
     /* If no node exists yet, create the first node */
     if (H->N == 0) {
         H->N = 1;
         H->nodes[0].count = 0;
     }
     int last = H->N - 1;
     /* If the last node is full, start a new node */
     if (H->nodes[last].count == H->p) {
         H->N++;
         last = H->N - 1;
         H->nodes[last].count = 0;
     }
     /* Append x to the last node */
     H->nodes[last].keys[H->nodes[last].count] = x;
     H->nodes[last].count++;
     H->n++;
 
     /* Repair the heap property by moving upward from node q */
     int q = last;
     while (q > 0) {
         int r = (q - 1) / 2;  // parent index (using 0-based indexing)
         /* Compute minimum key in parent node r */
         int rmin = H->nodes[r].keys[0];
         for (int i = 1; i < H->nodes[r].count; i++) {
             if (H->nodes[r].keys[i] < rmin)
                 rmin = H->nodes[r].keys[i];
         }
         /* Compute maximum key in current node q */
         int qmax = H->nodes[q].keys[0];
         for (int i = 1; i < H->nodes[q].count; i++) {
             if (H->nodes[q].keys[i] > qmax)
                 qmax = H->nodes[q].keys[i];
         }
         /* If parent's minimum is greater than child's maximum, heap property holds */
         if (rmin > qmax)
             break;
         /* Otherwise, combine keys from parent (r) and current node (q) */
         int total = H->nodes[r].count + H->nodes[q].count;
         int *temp = (int *) malloc(total * sizeof(int));
         if (!temp) {
             fprintf(stderr, "Memory allocation error\n");
             exit(1);
         }
         int idx = 0;
         for (int i = 0; i < H->nodes[r].count; i++)
             temp[idx++] = H->nodes[r].keys[i];
         for (int i = 0; i < H->nodes[q].count; i++)
             temp[idx++] = H->nodes[q].keys[i];
         /* Sort combined keys in descending order */
         qsort(temp, total, sizeof(int), cmp_desc);
         /* Redistribute: parent gets the largest p keys; child gets the rest */
         H->nodes[r].count = H->p;
         for (int i = 0; i < H->p; i++)
             H->nodes[r].keys[i] = temp[i];
         H->nodes[q].count = total - H->p;
         for (int i = 0; i < H->nodes[q].count; i++)
             H->nodes[q].keys[i] = temp[H->p + i];
         free(temp);
         q = r;  // move upward
     }
 }
 
 /* findmax: Return the maximum key from the root node */
 int findmax(MultiHeap *H) {
     if (H->n <= 0) {
         fprintf(stderr, "Heap is empty\n");
         exit(1);
     }
     Node *root = &H->nodes[0];
     int max = root->keys[0];
     for (int i = 1; i < root->count; i++) {
         if (root->keys[i] > max)
             max = root->keys[i];
     }
     return max;
 }
 
 /* heapify: Restore the heap property starting at node index i (used by delmax) */
 void heapify(MultiHeap *H, int i) {
     while (1) {
         /* Compute the minimum key in the current node i */
         int qmin = H->nodes[i].keys[0];
         for (int k = 1; k < H->nodes[i].count; k++) {
             if (H->nodes[i].keys[k] < qmin)
                 qmin = H->nodes[i].keys[k];
         }
         int child_index = -1;
         int max_child_val = -1;
         int left = 2 * i + 1;
         if (left < H->N) {
             int left_max = H->nodes[left].keys[0];
             for (int k = 1; k < H->nodes[left].count; k++) {
                 if (H->nodes[left].keys[k] > left_max)
                     left_max = H->nodes[left].keys[k];
             }
             if (left_max > max_child_val) {
                 max_child_val = left_max;
                 child_index = left;
             }
         }
         int right = 2 * i + 2;
         if (right < H->N) {
             int right_max = H->nodes[right].keys[0];
             for (int k = 1; k < H->nodes[right].count; k++) {
                 if (H->nodes[right].keys[k] > right_max)
                     right_max = H->nodes[right].keys[k];
             }
             if (right_max > max_child_val) {
                 max_child_val = right_max;
                 child_index = right;
             }
         }
         /* If no child exists or the current node’s minimum is larger than the maximum key in both children, stop */
         if (child_index == -1 || qmin > max_child_val)
             break;
         /* Otherwise, combine keys of current node i and child node child_index */
         int total = H->nodes[i].count + H->nodes[child_index].count;
         int *temp = (int *) malloc(total * sizeof(int));
         if (!temp) {
             fprintf(stderr, "Memory allocation error\n");
             exit(1);
         }
         int idx = 0;
         for (int k = 0; k < H->nodes[i].count; k++)
             temp[idx++] = H->nodes[i].keys[k];
         for (int k = 0; k < H->nodes[child_index].count; k++)
             temp[idx++] = H->nodes[child_index].keys[k];
         qsort(temp, total, sizeof(int), cmp_desc);
         /* Redistribute: node i gets the largest p keys (it must be full) */
         H->nodes[i].count = H->p;
         for (int k = 0; k < H->p; k++)
             H->nodes[i].keys[k] = temp[k];
         /* Child gets the remaining keys */
         H->nodes[child_index].count = total - H->p;
         for (int k = 0; k < H->nodes[child_index].count; k++)
             H->nodes[child_index].keys[k] = temp[H->p + k];
         free(temp);
         i = child_index; // continue heapifying downward
     }
 }
 
 /* delmax: Delete the maximum key from the heap (from the root) */
 void delmax(MultiHeap *H) {
     if (H->n <= 0) {
         fprintf(stderr, "Heap is empty\n");
         exit(1);
     }
     Node *root = &H->nodes[0];
     /* Find index of maximum key in root node */
     int max_index = 0;
     int max_val = root->keys[0];
     for (int i = 1; i < root->count; i++) {
         if (root->keys[i] > max_val) {
             max_val = root->keys[i];
             max_index = i;
         }
     }
     /* Remove the maximum key by replacing it with the last key in the root node */
     root->keys[max_index] = root->keys[root->count - 1];
     root->count--;
     H->n--;
     
     /* If the heap becomes empty, we are done */
     if (H->n == 0)
         return;
     
     /* If the root is not the only node, take a key from the last node and move it to the root */
     if (H->N > 1) {
         int last = H->N - 1;
         Node *lastNode = &H->nodes[last];
         int tempKey = lastNode->keys[lastNode->count - 1];
         lastNode->count--;
         if (lastNode->count == 0)
             H->N--;  // remove the last node if it becomes empty
         /* Place the key from the last node into the root */
         root->keys[root->count] = tempKey;
         root->count++;
         /* Restore the heap property starting from the root */
         heapify(H, 0);
     }
 }
 
 /* prnheap: Print the multi-heap node by node */
 void prnheap(MultiHeap *H) {
     for (int i = 0; i < H->N; i++) {
         printf("[ ");
         for (int j = 0; j < H->nodes[i].count; j++)
             printf("%d ", H->nodes[i].keys[j]);
         printf("]\n");
     }
 }
 
 /* ---------------- Main Function ---------------- */
 
 int main() {
     int p, n;
     printf("Enter node capacity (p): ");
     if (scanf("%d", &p) != 1 || p <= 0) {
         fprintf(stderr, "Invalid node capacity\n");
         return 1;
     }
     printf("Enter total number of keys to be inserted: ");
     if (scanf("%d", &n) != 1 || n < 0) {
         fprintf(stderr, "Invalid number of keys\n");
         return 1;
     }
     
     /* Read the keys into an array A */
     int *A = (int *) malloc(n * sizeof(int));
     if (!A) {
         fprintf(stderr, "Memory allocation error\n");
         return 1;
     }
     printf("Enter %d keys (space separated):\n", n);
     for (int i = 0; i < n; i++) {
         if (scanf("%d", &A[i]) != 1) {
             fprintf(stderr, "Invalid key input\n");
             free(A);
             return 1;
         }
     }
     
     /* Initialize the multi-heap with maximum capacity n */
     MultiHeap *H = initheap(p, n);
     
     /* Insert the keys one by one */
     for (int i = 0; i < n; i++) {
         insert(H, A[i]);
     }
     printf("\n+++ %d insertions made\n\n", n);
     /* Print the multi-heap after all insertions */
     prnheap(H);
     
     /* Delete keys one by one using delmax, storing them in array A in reverse order */
     int *sorted = (int *) malloc(n * sizeof(int));
     if (!sorted) {
         fprintf(stderr, "Memory allocation error\n");
         return 1;
     }
     int index = n - 1;
     while (H->n > 0) {
         sorted[index--] = findmax(H);
         delmax(H);
     }
     printf("\n+++ %d deletions made\n", n);
     printf("\n+++ Input array sorted\n\n");
     /* Print the sorted array (ascending order) */
     for (int i = 0; i < n; i++) {
         printf("%d ", sorted[i]);
     }
     printf("\n");
     
     /* Free allocated memory */
     free(sorted);
     free(A);
     /* Free each node's key array and the nodes array */
     int maxNodes = (H->nmax + p - 1) / p;
     for (int i = 0; i < maxNodes; i++) {
         free(H->nodes[i].keys);
     }
     free(H->nodes);
     free(H);
     
     return 0;
 } 