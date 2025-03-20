#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/* Heap structure definition */
typedef struct {
    int *arr;      // pointer to the heap array
    int capacity;  // maximum number of elements
    int size;      // current number of elements in the heap
} Heap;

/* Utility function to swap two integers */
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/* Create a new heap with given capacity */
Heap *create_heap(int capacity) {
    Heap *heap = (Heap *) malloc(sizeof(Heap));
    if (!heap) {
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }
    heap->capacity = capacity;
    heap->size = 0;
    heap->arr = (int *) malloc(capacity * sizeof(int));
    if (!heap->arr) {
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }
    return heap;
}

/* Max-heapify: ensures the subtree rooted at index i obeys the max-heap property */
void max_heapify(Heap *heap, int i) {
    int largest = i;
    int left  = 2 * i + 1;
    int right = 2 * i + 2;
    
    if (left < heap->size && heap->arr[left] > heap->arr[largest])
        largest = left;
    if (right < heap->size && heap->arr[right] > heap->arr[largest])
        largest = right;
    
    if (largest != i) {
        swap(&heap->arr[i], &heap->arr[largest]);
        max_heapify(heap, largest);
    }
}

/* Build a max-heap from an unsorted array in the heap structure */
void build_max_heap(Heap *heap) {
    for (int i = heap->size / 2 - 1; i >= 0; i--) {
        max_heapify(heap, i);
    }
}

/* Return the maximum element in the heap (located at the root) */
int maximum(Heap *heap) {
    if (heap->size < 1) {
        fprintf(stderr, "Heap underflow\n");
        exit(1);
    }
    return heap->arr[0];
}

/* Extract and return the maximum element from the heap */
int extract_max(Heap *heap) {
    if (heap->size < 1) {
        fprintf(stderr, "Heap underflow\n");
        exit(1);
    }
    int max = heap->arr[0];
    heap->arr[0] = heap->arr[heap->size - 1];
    heap->size--;
    max_heapify(heap, 0);
    return max;
}

/* Increase the key at index i to new_key, assuming new_key is greater than the current key */
void increase_key(Heap *heap, int i, int new_key) {
    if (new_key < heap->arr[i]) {
        fprintf(stderr, "New key is smaller than current key\n");
        return;
    }
    heap->arr[i] = new_key;
    while (i > 0 && heap->arr[(i - 1) / 2] < heap->arr[i]) {
        swap(&heap->arr[i], &heap->arr[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

/* Insert a new key into the heap */
void insert(Heap *heap, int key) {
    if (heap->size == heap->capacity) {
        fprintf(stderr, "Heap overflow\n");
        exit(1);
    }
    heap->size++;
    /* Start with a very small value, then increase to desired key */
    heap->arr[heap->size - 1] = INT_MIN;
    increase_key(heap, heap->size - 1, key);
}

/* Print the elements in the heap */
void print_heap(Heap *heap) {
    printf("Heap: ");
    for (int i = 0; i < heap->size; i++) {
        printf("%d ", heap->arr[i]);
    }
    printf("\n");
}

/* ---------------- In-Place Heap Sort Functions ---------------- */

/* Heapify an array (treated as a heap) for heap sort */
void heapify_array(int arr[], int n, int i) {
    int largest = i;
    int left  = 2 * i + 1;
    int right = 2 * i + 2;
    
    if (left < n && arr[left] > arr[largest])
        largest = left;
    if (right < n && arr[right] > arr[largest])
        largest = right;
    
    if (largest != i) {
        swap(&arr[i], &arr[largest]);
        heapify_array(arr, n, largest);
    }
}

/* Perform heap sort on an array */
void heap_sort(int arr[], int n) {
    // Build max heap
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify_array(arr, n, i);
    
    // One by one extract elements from heap
    for (int i = n - 1; i > 0; i--) {
        swap(&arr[0], &arr[i]);
        heapify_array(arr, i, 0);
    }
}

/* Utility function to print an array */
void print_array(int arr[], int n) {
    for (int i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

/* ---------------- Main Function ---------------- */

int main() {
    /* Demonstrate heap operations */
    int capacity = 20;
    Heap *heap = create_heap(capacity);
    
    // Insert some keys into the heap
    insert(heap, 10);
    insert(heap, 20);
    insert(heap, 15);
    insert(heap, 30);
    insert(heap, 40);
    
    printf("Heap after insertions:\n");
    print_heap(heap);
    
    printf("Maximum element in the heap: %d\n", maximum(heap));
    
    printf("Extracting maximum: %d\n", extract_max(heap));
    printf("Heap after extracting maximum:\n");
    print_heap(heap);
    
    printf("Increasing key at index 2 to 50.\n");
    increase_key(heap, 2, 50);
    print_heap(heap);
    
    /* Demonstrate heap sort */
    int arr[] = {12, 11, 13, 5, 6, 7};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    printf("\nOriginal array for heap sort:\n");
    print_array(arr, n);
    
    heap_sort(arr, n);
    
    printf("Sorted array after heap sort:\n");
    print_array(arr, n);
    
    /* Free allocated memory for the heap */
    free(heap->arr);
    free(heap);
    
    return 0;
}
