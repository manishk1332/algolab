# Binary Search & Prefix Sums Master Guide: Fast Preparation for Exams & Interviews

This guide contains clean, highly optimized **C++14** templates and explanations for Binary Search patterns, including Binary Search on Solution Space (Answer), prefix sum combinations, rotated array searches, and floating-point boundary searches.

---

## Table of Contents
1. [Core Binary Search Framework](#1-core-binary-search-framework)
   - [Standard Template (Iterative & Recursive)](#standard-template-iterative--recursive)
   - [Lower Bound vs. Upper Bound (Writing from Scratch)](#lower-bound-vs-upper-bound-writing-from-scratch)
2. [Binary Search on Solution Space (Answer)](#2-binary-search-on-solution-space-answer)
   - [Pattern A: Minimizing the Maximum (e.g., Split Array Largest Sum)](#pattern-a-minimizing-the-maximum-eg-split-array-largest-sum)
   - [Pattern B: Maximizing the Minimum (e.g., Aggressive Cows)](#pattern-b-maximizing-the-minimum-eg-aggressive-cows)
3. [Binary Search with Prefix Sums](#3-binary-search-with-prefix-sums)
   - [Static Range Queries & Prefix Bounds](#static-range-queries--prefix-bounds)
   - [Subarray Queries (Sum $\ge K$ on Non-Negative Arrays)](#subarray-queries-sum-ge-k-on-non-negative-arrays)
4. [Binary Search on Rotated Sorted Arrays](#4-binary-search-on-rotated-sorted-arrays)
   - [Searching for Target in Rotated Array](#searching-for-target-in-rotated-array)
   - [Handling Duplicates (Degraded $O(N)$ case)](#handling-duplicates-degraded-on-case)
5. [Binary Search on Floating Point Numbers (Real Numbers)](#5-binary-search-on-floating-point-numbers-real-numbers)
   - [Template & Epsilon vs. Loop Count Precision](#template--epsilon-vs-loop-count-precision)
6. [Advanced Partitioning Tricks](#6-advanced-partitioning-tricks)
   - [Median of Two Sorted Arrays](#median-of-two-sorted-arrays)
   - [Peak Finding (1D & 2D)](#peak-finding-1d--2d)
7. [Exam Identification Cheat Sheet](#exam-identification-cheat-sheet)

---

## 1. Core Binary Search Framework

Binary Search runs in $O(\log N)$ time on sorted arrays by repeatedly halving the search space.

### Standard Template (Iterative & Recursive)
* **Overflow Warning**: Always compute the midpoint as `mid = l + (r - l) / 2` instead of `(l + r) / 2` to prevent integer overflow.

```cpp
#include <vector>

// Time Complexity: O(log N), Space Complexity: O(1)
int binarySearchIterative(const std::vector<int>& nums, int target) {
    int l = 0, r = nums.size() - 1;
    while (l <= r) {
        int mid = l + (r - l) / 2;
        if (nums[mid] == target) return mid;
        else if (nums[mid] < target) l = mid + 1;
        else r = mid - 1;
    }
    return -1;
}

// Space Complexity: O(log N) due to recursive call stack
int binarySearchRecursive(const std::vector<int>& nums, int l, int r, int target) {
    if (l > r) return -1;
    int mid = l + (r - l) / 2;
    if (nums[mid] == target) return mid;
    if (nums[mid] < target) return binarySearchRecursive(nums, mid + 1, r, target);
    return binarySearchRecursive(nums, l, mid - 1, target);
}
```

### Lower Bound vs. Upper Bound (Writing from Scratch)
* **Lower Bound**: Finds the index of the first element $\ge \text{target}$.
* **Upper Bound**: Finds the index of the first element $> \text{target}$.

```cpp
#include <vector>

// First element >= target
int customLowerBound(const std::vector<int>& nums, int target) {
    int l = 0, r = nums.size(); // Search space [0, N]
    while (l < r) {
        int mid = l + (r - l) / 2;
        if (nums[mid] >= target) {
            r = mid; // Candidate found, look left
        } else {
            l = mid + 1; // Look right
        }
    }
    return l; // First index satisfying condition
}

// First element > target
int customUpperBound(const std::vector<int>& nums, int target) {
    int l = 0, r = nums.size();
    while (l < r) {
        int mid = l + (r - l) / 2;
        if (nums[mid] > target) {
            r = mid; // Candidate found, look left
        } else {
            l = mid + 1; // Look right
        }
    }
    return l;
}
```

---

## 2. Binary Search on Solution Space (Answer)

Used when the search domain is not an index, but a value range (e.g., $[1, 10^9]$ representing speed, distance, or size). We test midpoints using a validation helper function `isValid(mid)`.

```
┌──────────────────────────────────────────────────────────┐
│                 MONOTONICITY REQUIREMENT                 │
│ Binary search on answer only works if the answer space   │
│ is monotonic:                                            │
│   e.g., If cost X is valid, then cost X+1 is also valid.  │
│   e.g., If speed S is valid, then speed S-1 is also valid.│
└──────────────────────────────────────────────────────────┘
```

### Pattern A: Minimizing the Maximum
* **Concept**: We want to find the **smallest** value $X$ that satisfies a condition.
* **Update rules**:
  - If `isValid(mid) == true`: `mid` is a valid answer. Try to find an even smaller valid answer by moving left: `r = mid`.
  - If `isValid(mid) == false`: `mid` is too small. Move right: `l = mid + 1`.

#### Split Array Largest Sum (Book Allocation)
* **Problem**: Split array `nums` into $K$ contiguous subarrays such that the maximum sum of any subarray is minimized.
```cpp
#include <vector>
#include <numeric>
#include <algorithm>

// Check if we can split array into <= K subarrays with max sum <= max_sum
bool isValidSplit(const std::vector<int>& nums, int K, long long max_sum) {
    long long current_sum = 0;
    int subarrays_count = 1;
    
    for (int num : nums) {
        if (num > max_sum) return false; // Single element exceeds target limit
        if (current_sum + num > max_sum) {
            subarrays_count++;
            current_sum = num;
            if (subarrays_count > K) return false;
        } else {
            current_sum += num;
        }
    }
    return true;
}

// Time Complexity: O(N * log(Sum - Max))
int splitArray(const std::vector<int>& nums, int k) {
    long long l = *std::max_element(nums.begin(), nums.end());
    long long r = std::accumulate(nums.begin(), nums.end(), 0LL);
    long long ans = r;
    
    while (l <= r) {
        long long mid = l + (r - l) / 2;
        if (isValidSplit(nums, k, mid)) {
            ans = mid;    // Record candidate
            r = mid - 1;  // Try to minimize
        } else {
            l = mid + 1;  // Needs larger limit
        }
    }
    return ans;
}
```

### Pattern B: Maximizing the Minimum
* **Concept**: We want to find the **largest** value $X$ that satisfies a condition.
* **Update rules**:
  - If `isValid(mid) == true`: `mid` is valid. Try to find a larger valid answer: `l = mid + 1`.
  - If `isValid(mid) == false`: `mid` is too large. Move left: `r = mid - 1`.

#### Aggressive Cows
* **Problem**: Place $C$ cows in $N$ stalls such that the minimum distance between any two cows is maximized.
```cpp
#include <vector>
#include <algorithm>

// Check if we can place C cows with min spacing >= min_dist
bool isValidPlacement(const std::vector<int>& stalls, int C, int min_dist) {
    int count = 1;
    int last_placed = stalls[0];
    
    for (size_t i = 1; i < stalls.size(); ++i) {
        if (stalls[i] - last_placed >= min_dist) {
            count++;
            last_placed = stalls[i];
            if (count == C) return true;
        }
    }
    return false;
}

// Time Complexity: O(N log(Stalls_Range))
int maxMinDistance(std::vector<int>& stalls, int C) {
    std::sort(stalls.begin(), stalls.end());
    
    int l = 1;
    int r = stalls.back() - stalls.front();
    int ans = 0;
    
    while (l <= r) {
        int mid = l + (r - l) / 2;
        if (isValidPlacement(stalls, C, mid)) {
            ans = mid;   // Record candidate
            l = mid + 1; // Try to maximize spacing
        } else {
            r = mid - 1; // Spacing is too large
        }
    }
    return ans;
}
```

---

## 3. Binary Search with Prefix Sums

Used for static range sum queries and locating subsegment boundaries.

### Static Range Queries & Prefix Bounds
* **Prefix Sum Formula**:
  $$\text{Pref}[i] = \sum_{k=0}^{i-1} \text{nums}[k], \quad \text{Sum}(L, R) = \text{Pref}[R+1] - \text{Pref}[L]$$

```cpp
#include <vector>
#include <algorithm>

class RangeQuery {
    std::vector<long long> pref;
public:
    RangeQuery(const std::vector<int>& nums) {
        int n = nums.size();
        pref.assign(n + 1, 0);
        for (int i = 0; i < n; ++i) {
            pref[i + 1] = pref[i] + nums[i];
        }
    }
    
    // Sum in range [L, R] inclusive
    long long getSum(int L, int R) {
        return pref[R + 1] - pref[L];
    }
};
```

### Subarray Queries (Sum $\ge K$ on Non-Negative Arrays)
* **Problem**: Find the minimum length of a contiguous subarray with sum $\ge K$.
* **Algorithm**: Since values are non-negative, the prefix sum array `pref` is **monotonically increasing**. For each index $i$, we can binary search the first index $j > i$ where `pref[j] - pref[i] >= K`.

```cpp
// Time Complexity: O(N log N)
int minSubarrayLen(int target, const std::vector<int>& nums) {
    int n = nums.size();
    std::vector<long long> pref(n + 1, 0);
    for (int i = 0; i < n; ++i) pref[i + 1] = pref[i] + nums[i];
    
    int min_len = 1e9;
    for (int i = 0; i < n; ++i) {
        long long search_val = pref[i] + target;
        // Search first index where pref[j] >= pref[i] + target
        auto it = std::lower_bound(pref.begin() + i + 1, pref.end(), search_val);
        if (it != pref.end()) {
            int j = it - pref.begin();
            min_len = std::min(min_len, j - i);
        }
    }
    return (min_len == 1e9) ? 0 : min_len;
}
```

---

## 4. Binary Search on Rotated Sorted Arrays

A sorted array is rotated at some pivot index. The key property: **at least one half of any split is always fully sorted**.

### Searching for Target in Rotated Array
* **Rule**:
  1. Determine which half of the array `[l, mid]` or `[mid, r]` is sorted.
  2. Check if the `target` falls inside the sorted half boundary.
  3. Adjust `l` and `r` accordingly.

```cpp
#include <vector>

// Time Complexity: O(log N), Space Complexity: O(1)
int searchRotated(const std::vector<int>& nums, int target) {
    int l = 0, r = nums.size() - 1;
    
    while (l <= r) {
        int mid = l + (r - l) / 2;
        if (nums[mid] == target) return mid;
        
        // Case 1: Left half is sorted
        if (nums[l] <= nums[mid]) {
            if (target >= nums[l] && target < nums[mid]) {
                r = mid - 1; // target lies in left sorted half
            } else {
                l = mid + 1; // search right half
            }
        }
        // Case 2: Right half is sorted
        else {
            if (target > nums[mid] && target <= nums[r]) {
                l = mid + 1; // target lies in right sorted half
            } else {
                r = mid - 1; // search left half
            }
        }
    }
    return -1;
}
```

### Handling Duplicates (Degraded $O(N)$ case)
* **Problem**: Rotated array can contain duplicates (e.g., `[1, 0, 1, 1, 1]`).
* **Trick**: If `nums[l] == nums[mid] && nums[mid] == nums[r]`, we cannot tell which half is sorted. We must increment `l` and decrement `r` to shrink boundaries.

```cpp
// Time Complexity: O(log N) average, O(N) worst case
bool searchRotatedWithDuplicates(const std::vector<int>& nums, int target) {
    int l = 0, r = nums.size() - 1;
    
    while (l <= r) {
        int mid = l + (r - l) / 2;
        if (nums[mid] == target) return true;
        
        // Cannot determine sorted side due to duplicates
        if (nums[l] == nums[mid] && nums[mid] == nums[r]) {
            l++;
            r--;
            continue;
        }
        
        if (nums[l] <= nums[mid]) {
            if (target >= nums[l] && target < nums[mid]) r = mid - 1;
            else l = mid + 1;
        } else {
            if (target > nums[mid] && target <= nums[r]) l = mid + 1;
            else r = mid - 1;
        }
    }
    return false;
}
```

---

## 5. Binary Search on Floating Point Numbers (Real Numbers)

Used when maximizing/minimizing continuous values. There are no indices.

### Template & Epsilon vs. Loop Count Precision
1. **Epsilon Comparison**: Stop when `r - l < 1e-9`. (Vulnerable to floating-point precision loss on large numbers).
2. **Fixed Iterations (CP Trick)**: Run the loop **exactly 100 times**. This achieves maximum precision ($\approx 10^{-30}$ error margin) and prevents infinite loop locks.

```cpp
#include <iostream>
#include <iomanip>

// Example: Compute Square Root using binary search
double mySqrt(double x) {
    if (x < 0) return -1;
    double l = 0, r = std::max(1.0, x);
    
    // Running loop exactly 100 times guarantees precision
    for (int iter = 0; iter < 100; ++iter) {
        double mid = l + (r - l) / 2;
        if (mid * mid > x) {
            r = mid;
        } else {
            l = mid;
        }
    }
    return l;
}
```

---

## 6. Advanced Partitioning Tricks

### Median of Two Sorted Arrays
* **Concept**: Partition two sorted arrays $A$ and $B$ such that the left partition has the same size as the right partition, and all elements in the left partition are $\le$ elements in the right partition.
* **Formula**:
  - `leftA = A[partitionA - 1]`, `rightA = A[partitionA]`
  - `leftB = B[partitionB - 1]`, `rightB = B[partitionB]`
  - Validate: `leftA <= rightB` and `leftB <= rightA`.

```cpp
#include <vector>
#include <algorithm>
#include <iostream>

// Time Complexity: O(log(min(N, M)))
// Space Complexity: O(1)
double findMedianSortedArrays(const std::vector<int>& A, const std::vector<int>& B) {
    if (A.size() > B.size()) return findMedianSortedArrays(B, A); // Ensure A is smaller
    
    int n = A.size();
    int m = B.size();
    int l = 0, r = n;
    
    while (l <= r) {
        int partitionA = l + (r - l) / 2;
        int partitionB = (n + m + 1) / 2 - partitionA;
        
        int maxLeftA = (partitionA == 0) ? -2e9 : A[partitionA - 1];
        int minRightA = (partitionA == n) ? 2e9 : A[partitionA];
        
        int maxLeftB = (partitionB == 0) ? -2e9 : B[partitionB - 1];
        int minRightB = (partitionB == m) ? 2e9 : B[partitionB];
        
        if (maxLeftA <= minRightB && maxLeftB <= minRightA) {
            if ((n + m) % 2 == 0) {
                return (std::max(maxLeftA, maxLeftB) + std::min(minRightA, minRightB)) / 2.0;
            } else {
                return std::max(maxLeftA, maxLeftB);
            }
        } else if (maxLeftA > minRightB) {
            r = partitionA - 1; // Move left in A
        } else {
            l = partitionA + 1; // Move right in A
        }
    }
    return 0.0;
}
```

### Peak Finding (1D & 2D)
* **1D Peak Element**: Find any element $i$ such that `nums[i] > nums[i-1]` and `nums[i] > nums[i+1]`.
* **Idea**: If `nums[mid] < nums[mid + 1]`, there must be a peak to the right. Else, there is a peak to the left.

```cpp
// Time Complexity: O(log N)
int findPeakElement(const std::vector<int>& nums) {
    int l = 0, r = nums.size() - 1;
    while (l < r) {
        int mid = l + (r - l) / 2;
        if (nums[mid] < nums[mid + 1]) {
            l = mid + 1; // Climb uphill to the right
        } else {
            r = mid;     // Slope goes down, look left
        }
    }
    return l;
}
```

---

## Exam Identification Cheat Sheet

| Question Description Hints | Strategy | Boundary Variable Init | Loop Check |
| :--- | :--- | :--- | :--- |
| **"Minimize the maximum value of..."** | Binary Search on Answer (Minimizing Pattern) | `l = max_element`, `r = sum_elements` | `while (l <= r)` with candidate saving |
| **"Maximize the minimum value of..."** | Binary Search on Answer (Maximizing Pattern) | `l = 1`, `r = max_possible_dist` | `while (l <= r)` with candidate saving |
| **"Find first element satisfying condition..."** | Lower Bound Search | `l = 0`, `r = N` | `while (l < r)` returning `l` |
| **"Rotated sorted elements search..."** | Pivot-based Binary Search | `l = 0`, `r = N-1` | Compare `nums[l]` with `nums[mid]` to find sorted side |
| **"Continuous variable search (precision limits)"** | Float-based Binary Search | `l = min_val`, `r = max_val` | Loop exactly `100` times for maximum precision |
| **"Subarray sum queries (all positive values)"** | Monotonic Prefix Sum + Binary Search | `pref` array constructed first | Find index using `std::lower_bound` |
