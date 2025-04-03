#include <iostream>
#include <string>

using namespace std;

typedef unsigned long long ull;

// Constants for the polynomial hash function.
const ull MOD = 1000000007ULL;  // Large prime modulus to reduce collisions.
const ull p = 31;              // Base value for hashing.

// Define the size of the hash table (should ideally be a prime number).
const int TABLE_SIZE = 101;

// Computes a polynomial hash for a given string.
// The hash is calculated as: hash(s) = Σ (s[i]-'a'+1) * p^i mod MOD.
ull polynomialHash(const string &s) {
    ull hash = 0;
    ull p_pow = 1; // Represents p^0 initially.
    for (size_t i = 0; i < s.size(); i++) {
        int char_value = s[i] - 'a' + 1;  // Map 'a' -> 1, 'b' -> 2, etc.
        hash = (hash + char_value * p_pow) % MOD;
        p_pow = (p_pow * p) % MOD;
    }
    return hash;
}

// Hash table structure using open addressing (linear probing).
struct HashTable {
    // Array of pointers to dynamically allocated std::string objects.
    std::string **table;
    int size;
};

// Create and initialize a hash table with a given number of buckets.
HashTable* createHashTable(int size) {
    HashTable* ht = new HashTable;
    ht->size = size;
    ht->table = new std::string*[size];
    for (int i = 0; i < size; i++) {
        ht->table[i] = nullptr;
    }
    return ht;
}

// Insert a key into the hash table using linear probing.
// Returns true if insertion is successful, false if the key already exists or table is full.
bool insert(HashTable* ht, const std::string &key) {
    ull hash_val = polynomialHash(key);
    int index = hash_val % ht->size;
    int start = index;
    
    // Loop until an empty slot is found.
    while (ht->table[index] != nullptr) {
        // If the key already exists, do not insert it again.
        if (*(ht->table[index]) == key) {
            return false;
        }
        index = (index + 1) % ht->size;
        // If we have looped back to the starting index, the table is full.
        if (index == start) {
            std::cout << "Hash table is full!" << std::endl;
            return false;
        }
    }
    
    // Insert a new key.
    ht->table[index] = new std::string(key);
    return true;
}

// Search for a key in the hash table using linear probing.
// Returns true if the key is found; otherwise, false.
bool search(HashTable* ht, const std::string &key) {
    ull hash_val = polynomialHash(key);
    int index = hash_val % ht->size;
    int start = index;
    
    while (ht->table[index] != nullptr) {
        if (*(ht->table[index]) == key) {
            return true;
        }
        index = (index + 1) % ht->size;
        if (index == start) {  // We've searched the entire table.
            break;
        }
    }
    return false;
}

// Free all dynamically allocated memory for the hash table.
void deleteHashTable(HashTable* ht) {
    for (int i = 0; i < ht->size; i++) {
        if (ht->table[i] != nullptr) {
            delete ht->table[i];
        }
    }
    delete[] ht->table;
    delete ht;
}

int main() {
    HashTable* ht = createHashTable(TABLE_SIZE);
    
    int n;
    cout << "Enter number of strings to insert: ";
    cin >> n;
    cin.ignore();  // Clear newline from the input buffer.
    
    std::string input;
    for (int i = 0; i < n; i++) {
        cout << "Enter string " << i + 1 << ": ";
        getline(cin, input);
        insert(ht, input);
    }
    
    cout << "\nEnter string to search: ";
    getline(cin, input);
    if (search(ht, input))
        cout << "String found in the hash table." << endl;
    else
        cout << "String NOT found in the hash table." << endl;
    
    deleteHashTable(ht);
    return 0;
}