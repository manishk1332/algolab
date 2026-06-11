# Priority Queues & Heaps Master Guide: Fast Preparation for Exams & Interviews

This guide contains clean, highly optimized **C++14** templates and explanations for Priority Queue (Heap) patterns, including custom comparators, K-Way merging, running medians, greedy interval scheduling, and STL heap operations.

---

## Table of Contents
1. [Core STL Priority Queue Syntax](#1-core-stl-priority-queue-syntax)
   - [Max-Heap vs. Min-Heap](#max-heap-vs-min-heap)
   - [Custom Comparators (Structs & Lambda Functions)](#custom-comparators-structs--lambda-functions)
2. [Pattern 1: K-Way Merge (Merge K Sorted Lists/Arrays)](#2-pattern-1-k-way-merge-merge-k-sorted-listsarrays)
3. [Pattern 2: Running Median (Two Heaps Trick)](#3-pattern-2-running-median-two-heaps-trick)
4. [Pattern 3: Top K Elements / Frequencies (Heap Size Limit Trick)](#4-pattern-3-top-k-elements--frequencies-heap-size-limit-trick)
5. [Pattern 4: Greedy Scheduling & Meeting Rooms](#5-pattern-4-greedy-scheduling--meeting-rooms)
6. [Pattern 5: Greedy Merging (Minimum Connect Cost)](#6-pattern-5-greedy-merging-minimum-connect-cost)
7. [Alternative STL Heap Operations (`make_heap`, `push_heap`, `pop_heap`)](#7-alternative-stl-heap-operations-make_heap-push_heap-pop_heap)
8. [Exam Identification Cheat Sheet](#exam-identification-cheat-sheet)

---

## 1. Core STL Priority Queue Syntax

In C++, `std::priority_queue` is a container adaptor that provides $O(\log N)$ insertion and extraction of the top element. By default, it is configured as a **Max-Heap**.

### Max-Heap vs. Min-Heap
```cpp
#include <queue>
#include <vector>

void basicSyntax() {
    // 1. Max-Heap (Default: Largest element at top)
    std::priority_queue<int> max_pq;
    
    // 2. Min-Heap (Smallest element at top)
    std::priority_queue<int, std::vector<int>, std::greater<int>> min_pq;
}
```

### Custom Comparators (Structs & Lambda Functions)
* **Crucial Rule**: In STL priority queue, the comparison parameter behaves **opposite** to standard sorting (`std::sort`).
  - To make it a **Min-Heap** (smaller values at top), the comparator must return `true` when the first argument is **greater than** the second argument (`>`).

#### Custom Struct Comparator
```cpp
#include <queue>
#include <vector>
#include <string>

struct Point {
    int x, y;
    std::string label;
};

// We want closest points to origin (0,0) at top -> Min-Heap by Euclidean distance
struct CompareDistance {
    bool operator()(const Point& p1, const Point& p2) {
        long long dist1 = 1LL * p1.x * p1.x + 1LL * p1.y * p1.y;
        long long dist2 = 1LL * p2.x * p2.x + 1LL * p2.y * p2.y;
        return dist1 > dist2; // > makes it a Min-Heap
    }
};

void customStructPQ() {
    std::priority_queue<Point, std::vector<Point>, CompareDistance> pq;
}
```

---

## 2. Pattern 1: K-Way Merge (Merge K Sorted Lists/Arrays)

* **Problem**: Merge $K$ sorted arrays into a single sorted array.
* **Idea**: Maintain a Min-Heap containing the first element of each of the $K$ arrays. Pop the smallest element, add it to the output, and push the next element from that same array.
* **State Tracked**: `{value, array_index, element_index}`.

```cpp
#include <vector>
#include <queue>

struct HeapNode {
    int val;
    int arr_idx;
    int elem_idx;
    
    bool operator>(const HeapNode& other) const {
        return val > other.val; // > for Min-Heap
    }
};

// Time Complexity: O(N log K) where N is total elements, K is number of arrays
// Space Complexity: O(K) for heap storage
std::vector<int> mergeKSortedArrays(const std::vector<std::vector<int>>& arrays) {
    std::priority_queue<HeapNode, std::vector<HeapNode>, std::greater<HeapNode>> min_heap;
    
    // Initialize heap with first elements
    for (size_t i = 0; i < arrays.size(); ++i) {
        if (!arrays[i].empty()) {
            min_heap.push({arrays[i][0], (int)i, 0});
        }
    }
    
    std::vector<int> result;
    while (!min_heap.empty()) {
        HeapNode curr = min_heap.top();
        min_heap.pop();
        
        result.push_back(curr.val);
        
        // Push next element from same array if it exists
        int next_elem_idx = curr.elem_idx + 1;
        if (next_elem_idx < arrays[curr.arr_idx].size()) {
            min_heap.push({arrays[curr.arr_idx][next_elem_idx], curr.arr_idx, next_elem_idx});
        }
    }
    return result;
}
```

---

## 3. Pattern 2: Running Median (Two Heaps Trick)

* **Problem**: Design a structure that receives a stream of numbers and finds the median at any point in $O(1)$ time.
* **Idea**: Divide the numbers into two halves:
  1. `max_heap` (stores the smaller half).
  2. `min_heap` (stores the larger half).
* **Invariant**: The difference in sizes between `max_heap` and `min_heap` must never exceed 1.

```cpp
#include <queue>
#include <vector>
#include <algorithm>

class MedianFinder {
    std::priority_queue<int> max_heap; // Lower half
    std::priority_queue<int, std::vector<int>, std::greater<int>> min_heap; // Upper half
public:
    // Time Complexity: O(log N)
    void addNum(int num) {
        if (max_heap.empty() || num <= max_heap.top()) {
            max_heap.push(num);
        } else {
            min_heap.push(num);
        }
        
        // Rebalance sizes
        if (max_heap.size() > min_heap.size() + 1) {
            min_heap.push(max_heap.top());
            max_heap.pop();
        } else if (min_heap.size() > max_heap.size()) {
            max_heap.push(min_heap.top());
            min_heap.pop();
        }
    }
    
    // Time Complexity: O(1)
    double findMedian() {
        if (max_heap.size() == min_heap.size()) {
            return (max_heap.top() + min_heap.top()) / 2.0;
        } else {
            return max_heap.top();
        }
    }
};
```

---

## 4. Pattern 3: Top K Elements / Frequencies (Heap Size Limit Trick)

* **Problem**: Find the top $K$ largest elements in an unsorted stream or array.
* **Trick**: Do not sort the array ($O(N \log N)$) or build a Max-Heap of size $N$ ($O(N \log N)$). Instead:
  - Maintain a **Min-Heap** of size $K$.
  - Push elements to the heap. If the size exceeds $K$, pop the top (which removes the smallest candidate).
  - At the end, the heap contains the $K$ largest elements.
* **Complexity**: $O(N \log K)$ time and $O(K)$ space.

```cpp
#include <vector>
#include <queue>
#include <unordered_map>
#include <utility>

// Find K most frequent elements
// Time Complexity: O(N log K), Space Complexity: O(N)
std::vector<int> topKFrequent(const std::vector<int>& nums, int k) {
    std::unordered_map<int, int> count;
    for (int num : nums) count[num]++;
    
    // pair: {frequency, number}
    std::priority_queue<std::pair<int, int>, 
                        std::vector<std::pair<int, int>>, 
                        std::greater<std::pair<int, int>>> min_heap;
                        
    for (const auto& pair : count) {
        min_heap.push({pair.second, pair.first});
        if (min_heap.size() > k) {
            min_heap.pop(); // Remove lowest frequency element
        }
    }
    
    std::vector<int> result;
    while (!min_heap.empty()) {
        result.push_back(min_heap.top().second);
        min_heap.pop();
    }
    return result;
}
```

---

## 5. Pattern 4: Greedy Scheduling & Meeting Rooms

* **Problem**: Given a list of meeting time intervals `[start, end]`, find the minimum number of meeting rooms required.
* **Idea**:
  1. Sort intervals by start times.
  2. Use a **Min-Heap** to store the **end times** of meetings currently in progress.
  3. For each new meeting: if its start time is $\ge$ the earliest ending meeting (`heap.top()`), we can reuse that room (pop from heap).
  4. Push the current meeting's end time to the heap. The size of the heap at the end is the rooms required.

```cpp
#include <vector>
#include <queue>
#include <algorithm>

struct Interval {
    int start, end;
};

// Time Complexity: O(N log N) (due to sorting), Space Complexity: O(N)
int minMeetingRooms(std::vector<Interval>& intervals) {
    if (intervals.empty()) return 0;
    
    // Sort intervals by start time
    std::sort(intervals.begin(), intervals.end(), [](const Interval& a, const Interval& b) {
        return a.start < b.start;
    });
    
    // Min-heap to store meeting end times
    std::priority_queue<int, std::vector<int>, std::greater<int>> min_heap;
    
    min_heap.push(intervals[0].end);
    
    for (size_t i = 1; i < intervals.size(); ++i) {
        // If room is free, remove the old meeting
        if (intervals[i].start >= min_heap.top()) {
            min_heap.pop();
        }
        // Allocate room (extend current or use new)
        min_heap.push(intervals[i].end);
    }
    
    return min_heap.size();
}
```

---

## 6. Pattern 5: Greedy Merging (Minimum Connect Cost)

* **Problem**: Connect $N$ ropes of different lengths into one rope. The cost to connect two ropes is the sum of their lengths. Find the minimum cost to connect all ropes.
* **Greedy Choice**: Always combine the two **shortest** ropes first to minimize intermediate contributions.
* **Template**:

```cpp
#include <vector>
#include <queue>

// Time Complexity: O(N log N)
// Space Complexity: O(N)
long long minConnectCost(const std::vector<int>& ropes) {
    std::priority_queue<long long, std::vector<long long>, std::greater<long long>> min_heap(
        ropes.begin(), ropes.end()
    );
    
    long long total_cost = 0;
    while (min_heap.size() > 1) {
        long long first = min_heap.top();
        min_heap.pop();
        long long second = min_heap.top();
        min_heap.pop();
        
        long long cost = first + second;
        total_cost += cost;
        min_heap.push(cost);
    }
    return total_cost;
}
```

---

## 7. Alternative STL Heap Operations (`make_heap`, `push_heap`, `pop_heap`)

* **Why use this?**
  - `std::priority_queue` does not support iteration or index-based access, and has no `.clear()` method.
  - Using heap algorithms directly on a `std::vector` gives you a contiguous array representation that can be cleared, traversed, or sorted in-place.

```cpp
#include <vector>
#include <algorithm>
#include <iostream>

void stlHeapDemo() {
    std::vector<int> v = {3, 1, 4, 1, 5, 9};
    
    // 1. Convert vector to max-heap in-place: O(N)
    std::make_heap(v.begin(), v.end());
    // v.front() is now the largest element (9)
    
    // 2. Add an element to heap: O(log N)
    v.push_back(6);
    std::push_heap(v.begin(), v.end()); // Rearranges back element into correct position
    
    // 3. Remove largest element: O(log N)
    std::pop_heap(v.begin(), v.end()); // Moves front (max) element to the very back
    int max_val = v.back();            // Retrieves the popped value (9)
    v.pop_back();                      // Removes it from vector
    
    // 4. Sort heap in-place: O(N log N)
    std::sort_heap(v.begin(), v.end());
}
```

---

## Exam Identification Cheat Sheet

| Problem Pattern / Hint | Heap Configuration | State Tracking Strategy |
| :--- | :--- | :--- |
| **"Merge K sorted arrays/lists"** | Min-Heap | `{val, arr_idx, elem_idx}` to track next element candidate |
| **"Find dynamic running median"** | Max-Heap & Min-Heap | Rebalance so sizes differ by $\le 1$; median is top(s) |
| **"Find K largest/smallest elements"**| Min-Heap (K largest) / Max-Heap (K smallest) | Maintain heap size $\le K$. If exceeded, pop top element |
| **"Greedy intervals / room scheduling"**| Min-Heap | Track active interval end-times. Pop if next start $\ge$ top |
| **"Connect parts / ropes with min cost"**| Min-Heap | Pop top 2 smallest elements, merge, push back to heap |
| **"Dijkstra's shortest path"** | Min-Heap | Pair `{dist, node}` to greedily extract closest unvisited node |
