#include <iostream>
#include <string>

using namespace std;
typedef unsigned long long ull;

// Constants for polynomial hashing
const ull MOD = 1000000007ULL;  // A large prime modulus
const ull p = 31;              // Base value for hashing

// Size of the hash table (preferably a prime number for better distribution)
const int TABLE_SIZE = 101;

// Compute a polynomial hash for the given string.
// Each character (assumed lowercase) is mapped to (s[i]-'a'+1) and contributes as:
// hash(s) = sum_{i=0}^{n-1} (s[i]-'a'+1) * p^i mod MOD.
ull polynomialHash(const string &s) {
    ull hash = 0;
    ull p_pow = 1; // Represents p^0 initially.
    for (size_t i = 0; i < s.size(); i++) {
        int char_value = s[i] - 'a' + 1;
        hash = (hash + char_value * p_pow) % MOD;
        p_pow = (p_pow * p) % MOD;
    }
    return hash;
}

// Hash table structure using open addressing with quadratic probing.
struct HashTable {
    // Array of pointers to std::string (dynamically allocated keys)
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

// Insert a key into the hash table using quadratic probing.
// Returns true if insertion is successful, or false if the key already exists or table is full.
bool insert(HashTable* ht, const std::string &key) {
    ull hash_val = polynomialHash(key);
    
    // Quadratic probing: try indices (hash + i + i^2) mod table_size.
    for (int i = 0; i < ht->size; i++) {
        int index = (hash_val + i + i * i) % ht->size;
        
        // If the slot is empty, insert the key.
        if (ht->table[index] == nullptr) {
            ht->table[index] = new std::string(key);
            return true;
        }
        // If the key is already present, do not insert it again.
        if (*(ht->table[index]) == key) {
            return false;
        }
    }
    
    // If we exit the loop, the table is full.
    std::cout << "Hash table is full!" << std::endl;
    return false;
}

// Search for a key in the hash table using quadratic probing.
// Returns true if the key is found; otherwise, false.
bool search(HashTable* ht, const std::string &key) {
    ull hash_val = polynomialHash(key);
    
    for (int i = 0; i < ht->size; i++) {
        int index = (hash_val + i + i * i) % ht->size;
        
        // If an empty slot is found, the key is not in the table.
        if (ht->table[index] == nullptr) {
            return false;
        }
        if (*(ht->table[index]) == key) {
            return true;
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
    cin.ignore(); // Clear the newline character from the input buffer.
    
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