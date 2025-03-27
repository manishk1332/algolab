#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_SIZE 1000003

typedef struct {
    int a[3];
} State;

typedef struct HashEntry {
    State state;
    State parent;
    int from;
    int to;
    int amount;
    struct HashEntry *next;
} HashEntry;

typedef struct QueueNode {
    State state;
    struct QueueNode *next;
} QueueNode;

typedef struct {
    QueueNode *front;
    QueueNode *rear;
} Queue;

void initQueue(Queue *q) {
    q->front = q->rear = NULL;
}

void enqueue(Queue *q, State s) {
    QueueNode *newNode = (QueueNode*)malloc(sizeof(QueueNode));
    newNode->state = s;
    newNode->next = NULL;
    if (q->rear == NULL) {
        q->front = q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
}

State dequeue(Queue *q) {
    if (q->front == NULL) {
        fprintf(stderr, "Dequeue from empty queue\n");
        exit(1);
    }
    QueueNode *temp = q->front;
    State s = temp->state;
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    free(temp);
    return s;
}

int isEmpty(Queue *q) {
    return q->front == NULL;
}

unsigned long hash(State s) {
    unsigned long h = 5381;
    for (int i = 0; i < 3; i++) {
        h = ((h << 5) + h) + s.a[i]; // h * 33 + s.a[i]
    }
    return h % HASH_SIZE;
}

HashEntry* findEntry(HashEntry **hashTable, State s) {
    unsigned long h = hash(s);
    HashEntry *entry = hashTable[h];
    while (entry != NULL) {
        if (entry->state.a[0] == s.a[0] &&
            entry->state.a[1] == s.a[1] &&
            entry->state.a[2] == s.a[2]) {
            return entry;
        }
        entry = entry->next;
    }
    return NULL;
}

void addEntry(HashEntry **hashTable, State s, State parent, int from, int to, int amount) {
    unsigned long h = hash(s);
    HashEntry *entry = (HashEntry*)malloc(sizeof(HashEntry));
    entry->state = s;
    entry->parent = parent;
    entry->from = from;
    entry->to = to;
    entry->amount = amount;
    entry->next = hashTable[h];
    hashTable[h] = entry;
}

int isEqual(State a, State b) {
    return a.a[0] == b.a[0] && a.a[1] == b.a[1] && a.a[2] == b.a[2];
}

void generateNewState(State current, int *capacities, State *new_state, int *from, int *to, int *amount, int pour_dir) {
    int dirs[6][2] = {{0,1}, {0,2}, {1,0}, {1,2}, {2,0}, {2,1}};
    *from = dirs[pour_dir][0];
    *to = dirs[pour_dir][1];
    int from_idx = *from;
    int to_idx = *to;

    int available = capacities[to_idx] - current.a[to_idx];
    *amount = (current.a[from_idx] < available) ? current.a[from_idx] : available;
    if (*amount <= 0) {
        *amount = 0;
        return;
    }

    *new_state = current;
    new_state->a[from_idx] -= *amount;
    new_state->a[to_idx] += *amount;
}

void printMoves(HashEntry **hashTable, State target, State initial) {
    State current = target;
    int from_list[1000], to_list[1000], amount_list[1000];
    int count = 0;

    while (1) {
        HashEntry *entry = findEntry(hashTable, current);
        if (entry == NULL || isEqual(entry->parent, current)) {
            break;
        }
        from_list[count] = entry->from;
        to_list[count] = entry->to;
        amount_list[count] = entry->amount;
        count++;
        current = entry->parent;
        if (count >= 1000) {
            fprintf(stderr, "Path too long\n");
            exit(1);
        }
    }

    for (int i = 0; i < count / 2; i++) {
        int temp = from_list[i];
        from_list[i] = from_list[count - 1 - i];
        from_list[count - 1 - i] = temp;

        temp = to_list[i];
        to_list[i] = to_list[count - 1 - i];
        to_list[count - 1 - i] = temp;

        temp = amount_list[i];
        amount_list[i] = amount_list[count - 1 - i];
        amount_list[count - 1 - i] = temp;
    }

    current = initial;
    printf("+++ Make the following moves\n");
    for (int i = 0; i < count; i++) {
        printf("(%d, %d, %d) ==> Transfer %d ml from Glass %d to Glass %d ==>\n",
               current.a[0], current.a[1], current.a[2],
               amount_list[i], from_list[i]+1, to_list[i]+1);
        current.a[from_list[i]] -= amount_list[i];
        current.a[to_list[i]] += amount_list[i];
    }
    printf("(%d, %d, %d)\n", current.a[0], current.a[1], current.a[2]);
}

int main() {
    int capacities[3], initial[3], target[3];
    scanf("%d %d %d", &capacities[0], &capacities[1], &capacities[2]);
    scanf("%d %d %d", &initial[0], &initial[1], &initial[2]);
    scanf("%d %d %d", &target[0], &target[1], &target[2]);

    int sum_initial = initial[0] + initial[1] + initial[2];
    int sum_target = target[0] + target[1] + target[2];
    if (sum_initial != sum_target) {
        printf("Impossible\n");
        return 0;
    }

    for (int i = 0; i < 3; i++) {
        if (initial[i] > capacities[i] || target[i] > capacities[i]) {
            printf("Impossible\n");
            return 0;
        }
    }

    int target_valid = 0;
    for (int i = 0; i < 3; i++) {
        if (target[i] == 0 || target[i] == capacities[i]) {
            target_valid = 1;
            break;
        }
    }
    if (!target_valid) {
        printf("Impossible\n");
        return 0;
    }

    State initial_state = {{initial[0], initial[1], initial[2]}};
    State target_state = {{target[0], target[1], target[2]}};

    if (isEqual(initial_state, target_state)) {
        printf("Already in target state\n");
        return 0;
    }

    Queue q;
    initQueue(&q);
    enqueue(&q, initial_state);

    HashEntry **hashTable = (HashEntry**)calloc(HASH_SIZE, sizeof(HashEntry*));
    addEntry(hashTable, initial_state, initial_state, -1, -1, -1);

    int found = 0;

    while (!isEmpty(&q)) {
        State current = dequeue(&q);

        for (int pour_dir = 0; pour_dir < 6; pour_dir++) {
            int from, to, amount;
            State new_state;
            generateNewState(current, capacities, &new_state, &from, &to, &amount, pour_dir);

            if (amount == 0) continue;

            if (isEqual(new_state, target_state)) {
                addEntry(hashTable, new_state, current, from, to, amount);
                printMoves(hashTable, new_state, initial_state);
                found = 1;
                goto end;
            }

            if (findEntry(hashTable, new_state) == NULL) {
                addEntry(hashTable, new_state, current, from, to, amount);
                enqueue(&q, new_state);
            }
        }
    }
    
end:
    if (!found) {
        printf("Impossible\n");
    }

    for (int i = 0; i < HASH_SIZE; i++) {
        HashEntry *entry = hashTable[i];
        while (entry != NULL) {
            HashEntry *temp = entry;
            entry = entry->next;
            free(temp);
        }
    }
    free(hashTable);

    return 0;
}