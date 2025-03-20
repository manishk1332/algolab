/*
 * Name         : Your Name
 * Roll Number  : YourRollNumber
 * Assignment   : 7
 *
 * This source file contains solutions to two problems using heap-based approaches.
 *
 * Question 1:
 *   Scheduling tasks on a single processor.
 *   Given n tasks where tasks[i] = [startTime, processTime], the processor always chooses
 *   the available task with the smallest processing time (and smallest index on ties).
 *   The output is the order in which the tasks are processed.
 *
 * Question 2:
 *   Scheduling tasks to machines.
 *   Given two arrays:
 *     - machines[]: each machine requires a given amount of power.
 *     - tasks[]: each task takes a certain amount of processing time.
 *   At second j, task j is added to a FIFO queue. As long as there are free machines,
 *   the task at the front of the queue is assigned to the free machine with the lowest required
 *   power (ties broken by lowest index). If no machine is free, wait until one becomes free.
 *   The output is an assignment array, where assign[j] is the index of the machine that processes task j.
 *
 * Both solutions use heap data structures with O(n log n) or O((n+m) log n) complexity.
 *
 * Compile with:
 *   gcc -o solution solution.c -lm
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <limits.h>
 #include <math.h>
 
 /* ============================================= */
 /*               QUESTION 1                    */
 /* Single Processor Task Scheduling            */
 /* ============================================= */
 
 /* Structure to represent a task */
 typedef struct {
     int start;    // when task becomes available
     int process;  // processing time required
     int index;    // original index of the task
 } Task;
 
 /* Heap for available tasks (min-heap by processing time, then by index) */
 typedef struct {
     Task *arr;
     int size;
     int capacity;
 } TaskHeap;
 
 /* Swap two Task elements */
 void swapTask(Task *a, Task *b) {
     Task temp = *a;
     *a = *b;
     *b = temp;
 }
 
 /* Heapify-up: restore heap property from index i upward */
 void taskHeapifyUp(TaskHeap *heap, int i) {
     while (i > 0) {
         int parent = (i - 1) / 2;
         /* Compare by process time; if equal, by index */
         if ((heap->arr[parent].process > heap->arr[i].process) ||
             (heap->arr[parent].process == heap->arr[i].process && heap->arr[parent].index > heap->arr[i].index)) {
             swapTask(&heap->arr[parent], &heap->arr[i]);
             i = parent;
         } else {
             break;
         }
     }
 }
 
 /* Heapify-down: restore heap property from index i downward */
 void taskHeapifyDown(TaskHeap *heap, int i) {
     int left, right, smallest;
     while ((left = 2 * i + 1) < heap->size) {
         smallest = i;
         right = 2 * i + 2;
         if (left < heap->size && 
             ((heap->arr[left].process < heap->arr[smallest].process) ||
              (heap->arr[left].process == heap->arr[smallest].process && heap->arr[left].index < heap->arr[smallest].index)))
             smallest = left;
         if (right < heap->size && 
             ((heap->arr[right].process < heap->arr[smallest].process) ||
              (heap->arr[right].process == heap->arr[smallest].process && heap->arr[right].index < heap->arr[smallest].index)))
             smallest = right;
         if (smallest != i) {
             swapTask(&heap->arr[i], &heap->arr[smallest]);
             i = smallest;
         } else {
             break;
         }
     }
 }
 
 /* Insert a task into the heap */
 void insertTask(TaskHeap *heap, Task t) {
     if (heap->size >= heap->capacity) {
         fprintf(stderr, "Task Heap overflow\n");
         exit(1);
     }
     heap->arr[heap->size] = t;
     heap->size++;
     taskHeapifyUp(heap, heap->size - 1);
 }
 
 /* Remove and return the task with minimum processing time (and index tie-breaker) */
 Task removeMinTask(TaskHeap *heap) {
     if (heap->size <= 0) {
         fprintf(stderr, "Task Heap underflow\n");
         exit(1);
     }
     Task minTask = heap->arr[0];
     heap->arr[0] = heap->arr[heap->size - 1];
     heap->size--;
     taskHeapifyDown(heap, 0);
     return minTask;
 }
 
 /* Comparator for qsort: sort by start time, then by index */
 int compareTasks(const void *a, const void *b) {
     Task *t1 = (Task*) a;
     Task *t2 = (Task*) b;
     if (t1->start != t2->start)
         return t1->start - t2->start;
     return t1->index - t2->index;
 }
 
 /* Solve Question 1 */
 void solveQuestion1() {
     int n;
     printf("Enter the number of tasks: ");
     if(scanf("%d", &n) != 1 || n <= 0) {
         fprintf(stderr, "Invalid number of tasks\n");
         return;
     }
     
     Task *tasks = (Task *) malloc(sizeof(Task) * n);
     if (!tasks) {
         fprintf(stderr, "Memory allocation error\n");
         exit(1);
     }
     
     printf("Enter each task as: start_time process_time\n");
     for (int i = 0; i < n; i++) {
         if(scanf("%d %d", &tasks[i].start, &tasks[i].process) != 2) {
             fprintf(stderr, "Invalid task input\n");
             free(tasks);
             return;
         }
         tasks[i].index = i;
     }
     
     /* Sort tasks by start time (and index) */
     qsort(tasks, n, sizeof(Task), compareTasks);
     
     /* Initialize the task heap.
        In the worst-case, we may insert all tasks into the heap.
     */
     TaskHeap *heap = (TaskHeap *) malloc(sizeof(TaskHeap));
     heap->capacity = n;
     heap->size = 0;
     heap->arr = (Task *) malloc(sizeof(Task) * heap->capacity);
     if (!heap->arr) {
         fprintf(stderr, "Memory allocation error\n");
         exit(1);
     }
     
     int *order = (int *) malloc(sizeof(int) * n);  // result order of task indices
     int orderCount = 0;
     
     long long currentTime = 0;
     int i = 0;  // pointer to tasks array
     
     /* Process tasks until all are handled */
     while (orderCount < n) {
         /* Add all tasks that are now available */
         while (i < n && tasks[i].start <= currentTime) {
             insertTask(heap, tasks[i]);
             i++;
         }
         /* If no task is available, jump to the start time of the next task */
         if (heap->size == 0) {
             currentTime = tasks[i].start;
             continue;
         }
         
         Task curr = removeMinTask(heap);
         order[orderCount++] = curr.index;
         currentTime += curr.process;
     }
     
     /* Print the order in which tasks are processed */
     printf("The order in which tasks are processed is:\n");
     for (int j = 0; j < n; j++) {
         printf("%d", order[j]);
         if (j < n - 1)
             printf(", ");
     }
     printf("\n");
     
     free(tasks);
     free(heap->arr);
     free(heap);
     free(order);
 }
 
 /* ============================================= */
 /*               QUESTION 2                    */
 /* Machine Task Scheduling                     */
 /* ============================================= */
 
 /* Structure to represent a machine */
 typedef struct {
     int index;      // machine index
     int power;      // power required by machine
     long long freeTime; // time when machine becomes free
 } Machine;
 
 /* Generic heap for Machine items, with a comparator function */
 typedef struct {
     Machine *arr;
     int size;
     int capacity;
     int (*cmp)(Machine, Machine);
 } MachineHeap;
 
 /* Swap two Machine elements */
 void swapMachine(Machine *a, Machine *b) {
     Machine temp = *a;
     *a = *b;
     *b = temp;
 }
 
 /* Heapify-up for MachineHeap using the comparator cmp */
 void machineHeapifyUp(MachineHeap *heap, int i) {
     while (i > 0) {
         int parent = (i - 1) / 2;
         if (heap->cmp(heap->arr[i], heap->arr[parent])) {
             swapMachine(&heap->arr[i], &heap->arr[parent]);
             i = parent;
         } else {
             break;
         }
     }
 }
 
 /* Heapify-down for MachineHeap using the comparator cmp */
 void machineHeapifyDown(MachineHeap *heap, int i) {
     int left, right, best;
     while ((left = 2 * i + 1) < heap->size) {
         best = i;
         right = 2 * i + 2;
         if (left < heap->size && heap->cmp(heap->arr[left], heap->arr[best]))
             best = left;
         if (right < heap->size && heap->cmp(heap->arr[right], heap->arr[best]))
             best = right;
         if (best != i) {
             swapMachine(&heap->arr[i], &heap->arr[best]);
             i = best;
         } else {
             break;
         }
     }
 }
 
 /* Insert a Machine into the MachineHeap */
 void insertMachine(MachineHeap *heap, Machine m) {
     if (heap->size >= heap->capacity) {
         fprintf(stderr, "Machine Heap overflow\n");
         exit(1);
     }
     heap->arr[heap->size] = m;
     heap->size++;
     machineHeapifyUp(heap, heap->size - 1);
 }
 
 /* Remove and return the minimum Machine from the MachineHeap */
 Machine removeMinMachine(MachineHeap *heap) {
     if (heap->size <= 0) {
         fprintf(stderr, "Machine Heap underflow\n");
         exit(1);
     }
     Machine minMachine = heap->arr[0];
     heap->arr[0] = heap->arr[heap->size - 1];
     heap->size--;
     machineHeapifyDown(heap, 0);
     return minMachine;
 }
 
 /* Build a MachineHeap with given capacity and comparator function */
 MachineHeap* buildMachineHeap(int capacity, int (*cmp)(Machine, Machine)) {
     MachineHeap *heap = (MachineHeap *) malloc(sizeof(MachineHeap));
     heap->capacity = capacity;
     heap->size = 0;
     heap->cmp = cmp;
     heap->arr = (Machine *) malloc(sizeof(Machine) * capacity);
     if (!heap->arr) {
         fprintf(stderr, "Memory allocation error\n");
         exit(1);
     }
     return heap;
 }
 
 /* Comparator for free machines:
    Order by power; if tie, order by index.
    Returns nonzero if m1 should come before m2.
 */
 int compareFree(Machine m1, Machine m2) {
     if(m1.power != m2.power)
         return m1.power < m2.power;
     return m1.index < m2.index;
 }
 
 /* Comparator for busy machines:
    Order primarily by freeTime; if tie, then by power then index.
 */
 int compareBusy(Machine m1, Machine m2) {
     if(m1.freeTime != m2.freeTime)
         return m1.freeTime < m2.freeTime;
     if(m1.power != m2.power)
         return m1.power < m2.power;
     return m1.index < m2.index;
 }
 
 /* Solve Question 2 */
 void solveQuestion2() {
     int n, m;
     int i;
     
     printf("Enter number of machines: ");
     if(scanf("%d", &n) != 1 || n <= 0) {
         fprintf(stderr, "Invalid number of machines\n");
         return;
     }
     
     int *machinePowers = (int *) malloc(sizeof(int) * n);
     if (!machinePowers) {
         fprintf(stderr, "Memory allocation error\n");
         exit(1);
     }
     
     printf("Enter power required for each machine:\n");
     for(i = 0; i < n; i++) {
         if(scanf("%d", &machinePowers[i]) != 1) {
             fprintf(stderr, "Invalid machine power input\n");
             free(machinePowers);
             return;
         }
     }
     
     printf("Enter number of tasks: ");
     if(scanf("%d", &m) != 1 || m < 0) {
         fprintf(stderr, "Invalid number of tasks\n");
         free(machinePowers);
         return;
     }
     
     int *tasks = (int *) malloc(sizeof(int) * m);
     if (!tasks) {
         fprintf(stderr, "Memory allocation error\n");
         exit(1);
     }
     
     printf("Enter processing time for each task (in seconds):\n");
     for(i = 0; i < m; i++) {
         if(scanf("%d", &tasks[i]) != 1) {
             fprintf(stderr, "Invalid task input\n");
             free(machinePowers);
             free(tasks);
             return;
         }
     }
     
     // Build free machines heap with all machines initially free at time 0.
     MachineHeap *freeHeap = buildMachineHeap(n, compareFree);
     for (i = 0; i < n; i++) {
         Machine mach;
         mach.index = i;
         mach.power = machinePowers[i];
         mach.freeTime = 0;
         insertMachine(freeHeap, mach);
     }
     
     // Build busy machines heap (initially empty).
     MachineHeap *busyHeap = buildMachineHeap(n, compareBusy);
     
     int *assign = (int *) malloc(sizeof(int) * m);
     if (!assign) {
         fprintf(stderr, "Memory allocation error\n");
         exit(1);
     }
     
     long long currentTime = 0;
     
     /*
      * Process tasks in order of arrival.
      * The j-th task arrives at second j.
      */
     for (int j = 0; j < m; j++) {
         // A task arrives at time j.
         if (currentTime < j)
             currentTime = j;
         
         // Move machines from busyHeap to freeHeap if they are free by currentTime.
         while (busyHeap->size > 0 && busyHeap->arr[0].freeTime <= currentTime) {
             Machine freeMachine = removeMinMachine(busyHeap);
             insertMachine(freeHeap, freeMachine);
         }
         
         // If no free machine, advance currentTime to the earliest free time.
         if (freeHeap->size == 0) {
             currentTime = busyHeap->arr[0].freeTime;
             while (busyHeap->size > 0 && busyHeap->arr[0].freeTime <= currentTime) {
                 Machine freeMachine = removeMinMachine(busyHeap);
                 insertMachine(freeHeap, freeMachine);
             }
         }
         
         // Now assign the task to the free machine with the lowest power (tie: smallest index)
         Machine machineAssigned = removeMinMachine(freeHeap);
         assign[j] = machineAssigned.index;
         machineAssigned.freeTime = currentTime + tasks[j];
         insertMachine(busyHeap, machineAssigned);
     }
     
     // Print the assignment array.
     printf("The assignment of tasks to machines is:\n");
     for (i = 0; i < m; i++) {
         printf("%d", assign[i]);
         if(i < m - 1)
             printf(", ");
     }
     printf("\n");
     
     free(machinePowers);
     free(tasks);
     free(assign);
     free(freeHeap->arr);
     free(freeHeap);
     free(busyHeap->arr);
     free(busyHeap);
 }
 
 /* ============================================= */
 /*                      MAIN                     */
 /* ============================================= */
 
 int main() {
     int choice;
     printf("Choose the problem to run:\n");
     printf("1: Single Processor Task Scheduling (Question 1)\n");
     printf("2: Machine Task Scheduling (Question 2)\n");
     printf("Enter your choice (1 or 2): ");
     if(scanf("%d", &choice) != 1) {
         fprintf(stderr, "Invalid input\n");
         return 1;
     }
     
     if(choice == 1) {
         solveQuestion1();
     } else if(choice == 2) {
         solveQuestion2();
     } else {
         printf("Invalid choice. Exiting.\n");
     }
     
     return 0;
 }
 