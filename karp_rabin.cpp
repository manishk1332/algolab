#include <iostream>
#include <string>

using namespace std;

typedef long long ll;

const ll MOD = 1000000007;  // A large prime modulus
const ll p = 31;            // Base used for the polynomial hash

int main() {
    // Read input text and pattern
    string text, pattern;
    cout << "Enter text: ";
    getline(cin, text);
    cout << "Enter pattern: ";
    getline(cin, pattern);

    int n = text.size();
    int m = pattern.size();
    
    if (m > n) {
        cout << "Pattern not found." << endl;
        return 0;
    }

    // Allocate arrays for powers of p and prefix hashes for the text.
    ll* p_pow = new ll[n + 1];
    ll* h = new ll[n + 1];
    
    // Precompute powers of p: p_pow[i] = p^i mod MOD
    p_pow[0] = 1;
    for (int i = 1; i <= n; i++) {
        p_pow[i] = (p_pow[i - 1] * p) % MOD;
    }
    
    // Compute prefix hashes for the text:
    // h[i+1] = (h[i] + (text[i]-'a'+1) * p_pow[i]) mod MOD
    h[0] = 0;
    for (int i = 0; i < n; i++) {
        h[i + 1] = (h[i] + (text[i] - 'a' + 1) * p_pow[i]) % MOD;
    }
    
    // Compute the hash of the pattern using the same method.
    ll pattern_hash = 0;
    for (int i = 0; i < m; i++) {
        pattern_hash = (pattern_hash + (pattern[i] - 'a' + 1) * p_pow[i]) % MOD;
    }
    
    // Allocate a dynamic array to store occurrence indices.
    // In worst-case, every position could be an occurrence.
    int* occ = new int[n];
    int occCount = 0;
    
    // Slide over every possible starting index in text
    for (int i = 0; i <= n - m; i++) {
        // Calculate hash for the current substring text[i...i+m-1]
        ll cur_hash = (h[i + m] + MOD - h[i]) % MOD;
        // Adjust for the power factor: cur_hash should equal pattern_hash * p_pow[i] mod MOD
        if (cur_hash == (pattern_hash * p_pow[i]) % MOD) {
            // Verify the substring to rule out false positives due to hash collisions
            if (text.substr(i, m) == pattern) {
                occ[occCount++] = i;
            }
        }
    }
    
    // Output the results
    if (occCount == 0) {
        cout << "Pattern not found." << endl;
    } else {
        cout << "Pattern found at indices: ";
        for (int i = 0; i < occCount; i++) {
            cout << occ[i] << " ";
        }
        cout << endl;
    }
    
    // Clean up dynamically allocated memory
    delete[] p_pow;
    delete[] h;
    delete[] occ;
    
    return 0;
}