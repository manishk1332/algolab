#include <iostream>
#include <string>

#define MOD 1000000007

// Compute a polynomial hash for the string s.
// Uses the formula: hash(s) = Σ (s[i]-'a'+1) * p^i mod MOD
unsigned long long polynomialHash(const std::string &s) {
    unsigned long long hash = 0;
    unsigned long long p = 31;        // Base value
    unsigned long long p_pow = 1;     // p^0 initially

    for (size_t i = 0; i < s.size(); i++) {
        int char_value = s[i] - 'a' + 1;
        hash = (hash + char_value * p_pow) % MOD;
        p_pow = (p_pow * p) % MOD;
    }
    return hash;
}

// Node structure for linked list chaining in each bucket.
struct Node {
    std::string key;
    Node* next;
};

// Hash table structure with an array of pointers to Node (one per bucket).
struct HashTable {
    Node** buckets;
    int size;
};

// Create a hash table with a specified number of buckets.
HashTable* createHashTable(int size) {
    HashTable* ht = new HashTable;
    ht->size = size;
    ht->buckets = new Node*[size];
    for (int i = 0; i < size; i++) {
        ht->buckets[i] = nullptr;
    }
    return ht;
}

// Insert a key (std::string) into the hash table using chaining.
void insert(HashTable* ht, const std::string &key) {
    unsigned long long hash_val = polynomialHash(key);
    int index = hash_val % ht->size;

    // Create a new node with the key and insert it at the beginning of the chain.
    Node* newNode = new Node;
    newNode->key = key;
    newNode->next = ht->buckets[index];
    ht->buckets[index] = newNode;
}

// Search for a key in the hash table; returns true if found, false otherwise.
bool search(HashTable* ht, const std::string &key) {
    unsigned long long hash_val = polynomialHash(key);
    int index = hash_val % ht->size;
    Node* current = ht->buckets[index];

    // Traverse the chain in the bucket to find the key.
    while (current != nullptr) {
        if (current->key == key) {
            return true;
        }
        current = current->next;
    }
    return false;
}

// Delete the hash table and free all allocated memory.
void deleteHashTable(HashTable* ht) {
    for (int i = 0; i < ht->size; i++) {
        Node* current = ht->buckets[i];
        while (current != nullptr) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
    }
    delete[] ht->buckets;
    delete ht;
}

int main() {
    const int TABLE_SIZE = 101; // A prime number as the number of buckets.
    HashTable* ht = createHashTable(TABLE_SIZE);

    int n;
    std::cout << "Enter number of strings to insert: ";
    std::cin >> n;
    std::cin.ignore(); // Discard newline after reading the integer.

    std::string input;
    for (int i = 0; i < n; i++) {
        std::cout << "Enter string " << i + 1 << " (lowercase letters only): ";
        std::getline(std::cin, input);
        insert(ht, input);
    }

    std::cout << "\nEnter string to search: ";
    std::getline(std::cin, input);
    if (search(ht, input))
        std::cout << "String found in the hash table." << std::endl;
    else
        std::cout << "String NOT found in the hash table." << std::endl;

    deleteHashTable(ht);
    return 0;
}