/*
 * Name         : Your Name
 * Roll Number  : YourRollNumber
 * Assignment   : 7
 *
 * Description: This program uses a min-heap (implemented as an array of triples)
 *              to generate all sums of cubes a^3 + b^3 for a, b in [0, n] in sorted order.
 *              Only pairs with a <= b are generated, ensuring that duplicate
 *              representations such as (24,27) and (27,24) do not both appear.
 *
 *              The heap is initially populated with triples (i, i, 2*i^3) for i = 0, 1, ... n.
 *              Then, until the heap is empty, the minimum triple (a, b, a^3+b^3) is removed.
 *              If b < n, the next triple (a, b+1, a^3+(b+1)^3) is inserted.
 *              Every time a triple is removed, a check is performed to see if its sum matches
 *              the previous triple's sum, which indicates a 2-way Ramanujan (taxicab) number.
 *              When found, the number along with its two distinct pairs is printed.
 *
 * Compile with: gcc -o taxicab taxicab.c -lm
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <math.h>
 #include <limits.h>
 
 typedef struct {
     int a;
     int b;
     unsigned long long sum;  // key: a^3 + b^3
 } Triple;
 
 typedef struct {
     Triple *arr;   // array of Triple
     int size;      // current number of elements in the heap
     int capacity;  // maximum capacity of the heap
 } Heap;
 
 /* Function prototypes */
 Heap* buildHeap(int capacity);
 void insert(Heap *heap, Triple t);
 Triple removeMin(Heap *heap);
 void heapifyUp(Heap *heap, int i);
 void heapifyDown(Heap *heap, int i);
 
 /* Build a new empty heap with the given capacity */
 Heap* buildHeap(int capacity) {
     Heap *heap = (Heap*) malloc(sizeof(Heap));
     if (!heap) {
         fprintf(stderr, "Memory allocation error\n");
         exit(1);
     }
     heap->arr = (Triple*) malloc(sizeof(Triple) * capacity);
     if (!heap->arr) {
         fprintf(stderr, "Memory allocation error\n");
         exit(1);
     }
     heap->size = 0;
     heap->capacity = capacity;
     return heap;
 }
 
 /* Restore heap property upward from index i */
 void heapifyUp(Heap *heap, int i) {
     while (i > 0) {
         int parent = (i - 1) / 2;
         if (heap->arr[parent].sum > heap->arr[i].sum) {
             Triple temp = heap->arr[parent];
             heap->arr[parent] = heap->arr[i];
             heap->arr[i] = temp;
             i = parent;
         } else {
             break;
         }
     }
 }
 
 /* Restore heap property downward from index i */
 void heapifyDown(Heap *heap, int i) {
     int left, right, smallest;
     while ((left = 2*i + 1) < heap->size) {
         smallest = i;
         right = 2*i + 2;
         if (heap->arr[left].sum < heap->arr[smallest].sum)
             smallest = left;
         if (right < heap->size && heap->arr[right].sum < heap->arr[smallest].sum)
             smallest = right;
         if (smallest != i) {
             Triple temp = heap->arr[i];
             heap->arr[i] = heap->arr[smallest];
             heap->arr[smallest] = temp;
             i = smallest;
         } else {
             break;
         }
     }
 }
 
 /* Insert a triple into the heap while maintaining the min-heap property */
 void insert(Heap *heap, Triple t) {
     if (heap->size >= heap->capacity) {
         fprintf(stderr, "Heap overflow\n");
         exit(1);
     }
     heap->arr[heap->size] = t;
     heap->size++;
     heapifyUp(heap, heap->size - 1);
 }
 
 /* Remove and return the triple with the minimum sum from the heap */
 Triple removeMin(Heap *heap) {
     if (heap->size <= 0) {
         fprintf(stderr, "Heap underflow\n");
         exit(1);
     }
     Triple min = heap->arr[0];
     heap->arr[0] = heap->arr[heap->size - 1];
     heap->size--;
     heapifyDown(heap, 0);
     return min;
 }
 
 int main() {
     unsigned long long M;
     printf("Enter M: ");
     if (scanf("%llu", &M) != 1) {
         fprintf(stderr, "Invalid input\n");
         return 1;
     }
     
     /* Determine n = floor(cuberoot(M)).
        We want to generate pairs (a, b) with a, b in [0, n].
        (Some generated sums may exceed M; we stop processing when that happens.)
     */
     double d = cbrt((double) M);
     int n = (int) d;
     if (((unsigned long long)(n+1) * (n+1) * (n+1)) <= M)
         n++;
     
     /* Build a min-heap with capacity (n+1), ensuring O(n) space. */
     Heap *heap = buildHeap(n + 1);
     // Insert only pairs (i, i) to ensure a <= b.
     for (int i = 0; i <= n; i++) {
         Triple t;
         t.a = i;
         t.b = i;
         t.sum = (unsigned long long)i * i * i + (unsigned long long)i * i * i;  // 2 * i^3
         insert(heap, t);
     }
     
     // Variables to track previous removed triple for duplicate-sum check.
     Triple prev;
     int first = 1;
     unsigned long long lastPrinted = ULLONG_MAX; // sentinel: no taxicab number printed yet
     
     /* Process triples in increasing order of sum */
     while (heap->size > 0) {
         Triple t = removeMin(heap);
         
         // If current sum exceeds M, we are done.
         if (t.sum > M)
             break;
         
         // Check if this sum matches the previous triple's sum.
         // Since pairs are generated in canonical order (a <= b),
         // we avoid printing duplicate unordered pairs.
         if (!first && t.sum == prev.sum && t.sum != lastPrinted) {
             printf("%llu ( %d,%d) ( %d,%d)\n", t.sum, prev.a, prev.b, t.a, t.b);
             lastPrinted = t.sum;
         }
         
         prev = t;
         first = 0;
         
         // Generate the next triple for the same 'a', if possible.
         if (t.b < n) {
             Triple t2;
             t2.a = t.a;
             t2.b = t.b + 1;
             t2.sum = (unsigned long long)t.a * t.a * t.a + (unsigned long long)(t.b + 1) * (t.b + 1) * (t.b + 1);
             insert(heap, t2);
         }
     }
     
     // Free allocated memory
     free(heap->arr);
     free(heap);
     
     return 0;
 }
 