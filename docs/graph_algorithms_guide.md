# Graph Algorithms Master Guide: Fast Preparation for Exams & Interviews

This guide contains clean, highly optimized **C++14** templates and explanations for the most frequently tested graph algorithms, covering both basic foundations and advanced competitive programming (CP) tricks in a single comprehensive resource.

---

## Table of Contents
1. [DFS (Depth-First Search) & Variations](#1-dfs-depth-first-search--variations)
   - [Core DFS Template](#core-dfs-template)
   - [Cycle Detection (Directed & Undirected)](#cycle-detection-directed--undirected)
   - [DFS-based Topo Sort with 3-State Cycle Detection](#dfs-based-topo-sort-with-3-state-cycle-detection)
   - [Bipartiteness Check (2-Coloring)](#bipartiteness-check-2-coloring)
   - [Finding Bridges / Cut Edges (Tarjan's Algorithm)](#finding-bridges--cut-edges-tarjans-algorithm)
2. [BFS (Breadth-First Search) & Variations](#2-bfs-breadth-first-search--variations)
   - [Core BFS Template](#core-bfs-template)
   - [CP Best Practice: Eliminating the Visited Array](#cp-best-practice-eliminating-the-visited-array)
   - [Multi-Source BFS](#multi-source-bfs)
   - [0-1 BFS (Deque-based)](#0-1-bfs-deque-based)
3. [Topological Sort & Variations](#3-topological-sort--variations)
   - [Kahn's Algorithm (BFS-based)](#kahns-algorithm-bfs-based)
   - [Lexicographically Smallest Topological Sort](#lexicographically-smallest-topological-sort)
4. [DSU (Disjoint Set Union) Tricks & Tips](#4-dsu-disjoint-set-union-tricks--tips)
   - [Standard DSU (Path Compression & Union by Size)](#standard-dsu-path-compression--union-by-size)
   - [DSU Rollback Trick (No Path Compression)](#dsu-rollback-trick-no-path-compression)
5. [Dijkstra's Algorithm & Variations](#5-dijkstras-algorithm--variations)
   - [Core Dijkstra Template](#core-dijkstra-template)
   - [Path Reconstruction](#path-reconstruction)
   - [Grid Dijkstra](#grid-dijkstra)
   - [State-Based (2D) Dijkstra](#state-based-2d-dijkstra)
   - [Bottleneck Shortest Path (Minimax)](#bottleneck-shortest-path-minimax)
6. [Bellman-Ford & SPFA](#6-bellman-ford--spfa)
   - [Standard Bellman-Ford](#standard-bellman-ford)
   - [SPFA (Shortest Path Faster Algorithm)](#spfa-shortest-path-faster-algorithm)
7. [Floyd-Warshall & Variations](#7-floyd-warshall--variations)
8. [Strongly Connected Components (Kosaraju)](#8-strongly-connected-components-kosaraju)
   - [Core Kosaraju Template](#core-kosaraju-template)
   - [Building the Condensation Graph (DAG)](#building-the-condensation-graph-dag)
9. [Graph Algorithms Complexity Cheat Sheet](#graph-algorithms-complexity-cheat-sheet)

---

## 1. DFS (Depth-First Search) & Variations

DFS explores as deep as possible along each branch before backtracking. It is implemented recursively (using the system call stack).

### Core DFS Template
```cpp
#include <vector>

void dfs(int u, const std::vector<std::vector<int>>& adj, std::vector<bool>& visited) {
    visited[u] = true;
    for (int v : adj[u]) {
        if (!visited[v]) {
            dfs(v, adj, visited);
        }
    }
}
```

### Cycle Detection (Directed & Undirected)
* **Undirected Graph**: A cycle exists if we visit an already visited node that is **not** the immediate parent of the current node.
* **Directed Graph**: A cycle exists if we visit a node currently in the active recursion stack (Back Edge).

#### Undirected Cycle Detection Code
```cpp
// Time Complexity: O(V + E), Space Complexity: O(V)
bool checkCycleUndirected(int u, int parent, const std::vector<std::vector<int>>& adj, 
                           std::vector<bool>& visited) {
    visited[u] = true;
    for (int v : adj[u]) {
        if (!visited[v]) {
            if (checkCycleUndirected(v, u, adj, visited)) return true;
        } else if (v != parent) {
            return true; // Visited node is not the parent -> cycle found
        }
    }
    return false;
}
```

#### Directed Cycle Detection Code
```cpp
// Time Complexity: O(V + E), Space Complexity: O(V)
bool checkCycleDirected(int u, const std::vector<std::vector<int>>& adj, 
                         std::vector<bool>& visited, std::vector<bool>& in_stack) {
    visited[u] = true;
    in_stack[u] = true;
    
    for (int v : adj[u]) {
        if (!visited[v]) {
            if (checkCycleDirected(v, adj, visited, in_stack)) return true;
        } else if (in_stack[v]) {
            return true; // Found a back-edge (ancestor in active stack)
        }
    }
    
    in_stack[u] = false; // Backtrack
    return false;
}
```

### DFS-based Topo Sort with 3-State Cycle Detection
* **Concept**: Instead of a simple boolean `visited` array, we use a 3-state `state` array to perform topological sorting and cycle detection in a single DFS pass.
  - **State 0 (Unvisited)**: Node has not been touched.
  - **State 1 (Visiting / Processing)**: Node is in the active recursion stack. Reaching this state again indicates a **Back Edge** (directed cycle).
  - **State 2 (Visited / Fully Processed)**: Node and all its descendants have been explored. Safe to add to topological sort.

```cpp
#include <vector>
#include <algorithm>

// Time Complexity: O(V + E), Space Complexity: O(V)
bool dfsTopo(int u, const std::vector<std::vector<int>>& adj, 
             std::vector<int>& state, std::vector<int>& topo) {
    state[u] = 1; // Mark as visiting (in active recursion stack)
    
    for (int v : adj[u]) {
        if (state[v] == 1) {
            return false; // Found back-edge (cycle detected)
        }
        if (state[v] == 0) {
            if (!dfsTopo(v, adj, state, topo)) return false;
        }
    }
    
    state[u] = 2; // Mark as fully processed
    topo.push_back(u); // Add to post-order stack
    return true;
}

std::vector<int> getDFSOrder(int n, const std::vector<std::vector<int>>& adj) {
    std::vector<int> state(n, 0); // 0 = unvisited, 1 = visiting, 2 = processed
    std::vector<int> topo;
    
    for (int i = 0; i < n; ++i) {
        if (state[i] == 0) {
            if (!dfsTopo(i, adj, state, topo)) {
                return {}; // Cycle detected, no valid topological order exists
            }
        }
    }
    
    std::reverse(topo.begin(), topo.end()); // Post-order reversed is topological order
    return topo;
}
```

### Bipartiteness Check (2-Coloring)
* **Rule**: A graph is bipartite if it can be colored using 2 colors such that no two adjacent vertices have the same color. A graph is bipartite **if and only if** it contains no odd-length cycles.
```cpp
// Time Complexity: O(V + E), Space Complexity: O(V)
bool isBipartiteDFS(int u, int curr_color, const std::vector<std::vector<int>>& adj, 
                    std::vector<int>& color) {
    color[u] = curr_color;
    for (int v : adj[u]) {
        if (color[v] == -1) {
            if (!isBipartiteDFS(v, 1 - curr_color, adj, color)) return false;
        } else if (color[v] == color[u]) {
            return false; // Adjacent nodes have the same color
        }
    }
    return true;
}
```

### Finding Bridges / Cut Edges (Tarjan's Algorithm)
* **Concept**: A bridge is an edge whose removal increases the number of connected components. We track:
  - `tin[u]`: Time of entry (discovery time) of node $u$.
  - `low[u]`: Lowest `tin` reachable from $u$ using at most one back-edge.
* **Bridge Condition**: For edge $u \to v$, if `low[v] > tin[u]`, then $u \to v$ is a bridge. (There is no alternative path back to $u$ or any ancestor from $v$).

```cpp
#include <vector>
#include <algorithm>

class BridgeFinder {
    int timer = 0;
    std::vector<int> tin, low;
    std::vector<std::vector<int>> bridges;
    
    void dfs(int u, int parent, const std::vector<std::vector<int>>& adj) {
        tin[u] = low[u] = timer++;
        for (int v : adj[u]) {
            if (v == parent) continue;
            if (tin[v] != -1) {
                // Back-edge: update low using discovery time of v
                low[u] = std::min(low[u], tin[v]);
            } else {
                // Tree-edge
                dfs(v, u, adj);
                low[u] = std::min(low[u], low[v]);
                if (low[v] > tin[u]) {
                    bridges.push_back({u, v}); // Bridge found
                }
            }
        }
    }
public:
    std::vector<std::vector<int>> findBridges(int n, const std::vector<std::vector<int>>& adj) {
        tin.assign(n, -1);
        low.assign(n, -1);
        bridges.clear();
        timer = 0;
        for (int i = 0; i < n; ++i) {
            if (tin[i] == -1) dfs(i, -1, adj);
        }
        return bridges;
    }
};
```

---

## 2. BFS (Breadth-First Search) & Variations

BFS explores level-by-level using a queue. It guarantees finding the **shortest path in unweighted graphs**.

### Core BFS Template
```cpp
#include <vector>
#include <queue>

// Time Complexity: O(V + E), Space Complexity: O(V)
void bfs(int start, const std::vector<std::vector<int>>& adj, std::vector<bool>& visited) {
    std::queue<int> q;
    q.push(start);
    visited[start] = true;
    
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        
        for (int v : adj[u]) {
            if (!visited[v]) {
                visited[v] = true;
                q.push(v);
            }
        }
    }
}
```

### CP Best Practice: Eliminating the Visited Array
* **Trick**: Do not allocate both a `visited` and a `dist` array. Initialize `dist` to `-1` (or `INF`/`1e9`). A node is considered visited if `dist[v] != -1` (or `dist[v] != INF`).
* **Benefit**: Saves memory overhead and limits cache misses in performance-critical environments.

```cpp
#include <vector>
#include <queue>

// Shortest path from start using only the dist array
std::vector<int> bfsShortestPath(int start, int n, const std::vector<std::vector<int>>& adj) {
    std::vector<int> dist(n, -1); // -1 acts as "unvisited"
    std::queue<int> q;
    
    dist[start] = 0;
    q.push(start);
    
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        
        for (int v : adj[u]) {
            if (dist[v] == -1) { // Node is unvisited
                dist[v] = dist[u] + 1;
                q.push(v);
            }
        }
    }
    return dist;
}
```

### Multi-Source BFS
* **Concept**: Instead of pushing one node, push **multiple source nodes** into the queue initially with `dist = 0`. BFS will find the shortest distance from *any* source to all other nodes simultaneously.
* **Typical Problem**: Rotting Oranges, Distance to nearest 0 in a binary matrix.
```cpp
#include <vector>
#include <queue>
#include <utility>

// Time Complexity: O(R * C), Space Complexity: O(R * C)
std::vector<std::vector<int>> updateMatrix(const std::vector<std::vector<int>>& mat) {
    int r = mat.size(), c = mat[0].size();
    std::vector<std::vector<int>> dist(r, std::vector<int>(c, -1));
    std::queue<std::pair<int, int>> q;
    
    // Step 1: Push all sources (all 0s) to queue
    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
            if (mat[i][j] == 0) {
                dist[i][j] = 0;
                q.push({i, j});
            }
        }
    }
    
    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};
    
    // Step 2: Standard level-order traversal
    while (!q.empty()) {
        std::pair<int, int> curr = q.front();
        q.pop();
        int x = curr.first;
        int y = curr.second;
        
        for (int i = 0; i < 4; ++i) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            if (nx >= 0 && nx < r && ny >= 0 && ny < c && dist[nx][ny] == -1) {
                dist[nx][ny] = dist[x][y] + 1;
                q.push({nx, ny});
            }
        }
    }
    return dist;
}
```

### 0-1 BFS (Deque-based)
* **Concept**: Used when edge weights are only $0$ or $1$. Use a double-ended queue (`std::deque`).
  - Push to **front** if cost increment is $0$.
  - Push to **back** if cost increment is $1$.

```cpp
#include <vector>
#include <deque>

struct Edge {
    int to, weight;
};

// 0-1 BFS using dist array to check visitation
std::vector<int> zeroOneBFS(int start, int n, const std::vector<std::vector<Edge>>& adj) {
    std::vector<int> dist(n, 1e9); // 1e9 represents infinity / unvisited
    std::deque<int> dq;
    
    dist[start] = 0;
    dq.push_back(start);
    
    while (!dq.empty()) {
        int u = dq.front();
        dq.pop_front();
        
        for (const auto& edge : adj[u]) {
            int v = edge.to;
            int w = edge.weight;
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                if (w == 0) {
                    dq.push_front(v); // Front push (0-cost expansion)
                } else {
                    dq.push_back(v);  // Back push (1-cost expansion)
                }
            }
        }
    }
    return dist;
}
```

---

## 3. Topological Sort & Variations

A linear ordering of vertices in a Directed Acyclic Graph (DAG) such that for every directed edge $u \to v$, vertex $u$ comes before $v$ in the ordering.

### Kahn's Algorithm (BFS-based)
* **Idea**: Continually remove nodes with indegree $0$ and add them to the ordering.
* **Cycle Detection**: If the resulting topological sort contains fewer than $V$ nodes, the graph contains a cycle.

```cpp
#include <vector>
#include <queue>

// Time Complexity: O(V + E), Space Complexity: O(V)
std::vector<int> kahnTopoSort(int n, const std::vector<std::vector<int>>& adj) {
    std::vector<int> indegree(n, 0);
    for (int u = 0; u < n; ++u) {
        for (int v : adj[u]) {
            indegree[v]++;
        }
    }
    
    std::queue<int> q;
    for (int i = 0; i < n; ++i) {
        if (indegree[i] == 0) q.push(i);
    }
    
    std::vector<int> topo;
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        topo.push_back(u);
        
        for (int v : adj[u]) {
            indegree[v]--;
            if (indegree[v] == 0) {
                q.push(v);
            }
        }
    }
    
    if (topo.size() < n) return {}; // Cycle detected, Topo sort impossible!
    return topo;
}
```

### Lexicographically Smallest Topological Sort
* **Trick**: To get the lexicographically smallest topological sort, replace `std::queue` in Kahn's algorithm with a **min-priority queue** (`std::priority_queue<int, std::vector<int>, std::greater<int>>`).
* **C++14 Code**:
```cpp
#include <queue>
#include <vector>

std::vector<int> lexicographicalTopo(int n, const std::vector<std::vector<int>>& adj) {
    std::vector<int> indegree(n, 0);
    for (int u = 0; u < n; ++u) {
        for (int v : adj[u]) indegree[v]++;
    }
    
    std::priority_queue<int, std::vector<int>, std::greater<int>> pq;
    for (int i = 0; i < n; ++i) {
        if (indegree[i] == 0) pq.push(i);
    }
    
    std::vector<int> topo;
    while (!pq.empty()) {
        int u = pq.top();
        pq.pop();
        topo.push_back(u);
        
        for (int v : adj[u]) {
            indegree[v]--;
            if (indegree[v] == 0) pq.push(v);
        }
    }
    return (topo.size() == n) ? topo : std::vector<int>(); // Empty if cycle exists
}
```

---

## 4. DSU (Disjoint Set Union) Tricks & Tips

DSU tracks connected components in a partition. Optimization via **Path Compression** and **Union by Size** yields near-constant time operations.

### Standard DSU (Path Compression & Union by Size)
* **Component Tracking**:
  - `num_components`: Decremented on every successful union.
  - `size[find(u)]`: Tracks size of $u$'s component.
* **Cycle Detection (Undirected Graph)**: Loop through every edge $(u, v)$. If `find(u) == find(v)`, adding this edge creates a cycle. Otherwise, `unite(u, v)`.

```cpp
#include <vector>
#include <numeric>

class DSU {
    std::vector<int> parent;
    std::vector<int> size;
    int num_components;
public:
    DSU(int n) {
        parent.resize(n);
        std::iota(parent.begin(), parent.end(), 0);
        size.assign(n, 1);
        num_components = n;
    }
    
    int find(int i) {
        if (parent[i] == i) return i;
        return parent[i] = find(parent[i]); // Path compression
    }
    
    bool unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);
        
        if (root_i != root_j) {
            if (size[root_i] < size[root_j]) {
                std::swap(root_i, root_j);
            }
            parent[root_j] = root_i;
            size[root_i] += size[root_j];
            num_components--;
            return true;
        }
        return false; // Already in same component (cycle detected)
    }
    
    int get_num_components() const { return num_components; }
    int get_component_size(int i) { return size[find(i)]; }
};
```

### DSU Rollback Trick (No Path Compression)
* **Use Case**: Used in backtrackable algorithms (e.g. Offline Dynamic Connectivity, Divide & Conquer DP on queries).
* **Rule**: Path compression **must be disabled** because it changes grandparent pointers permanently. We use only **Union by Size** to maintain $O(\log N)$ tree depth, storing parent-update history in a stack for rollbacks.

```cpp
#include <vector>
#include <utility>

class RollbackDSU {
    std::vector<int> parent;
    std::vector<int> size;
    
    // Stack stores history: {child, original_parent}
    // We also track original size changes if necessary
    std::vector<std::pair<int, int>> history;
public:
    RollbackDSU(int n) {
        parent.resize(n);
        for (int i = 0; i < n; ++i) parent[i] = i;
        size.assign(n, 1);
    }
    
    // Path compression disabled: find runs in O(log N)
    int find(int i) {
        while (i != parent[i]) i = parent[i];
        return i;
    }
    
    bool unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);
        if (root_i == root_j) return false;
        
        if (size[root_i] < size[root_j]) std::swap(root_i, root_j);
        
        // Record state before modifications
        history.push_back({root_j, parent[root_j]});
        history.push_back({root_i, size[root_i]});
        
        parent[root_j] = root_i;
        size[root_i] += size[root_j];
        return true;
    }
    
    // Rollback last union operation
    void rollback() {
        if (history.empty()) return;
        
        // Pop size update
        int root_i = history.back().first;
        int orig_size = history.back().second;
        history.pop_back();
        size[root_i] = orig_size;
        
        // Pop parent update
        int root_j = history.back().first;
        int orig_parent = history.back().second;
        history.pop_back();
        parent[root_j] = orig_parent;
    }
};
```

---

## 5. Dijkstra's Algorithm & Variations

Dijkstra finds the shortest path on graphs with non-negative edge weights.

### Core Dijkstra Template
* **Pruning Condition**: `if (d > dist[u]) continue;` is critical. If a vertex is processed multiple times via different relaxation steps, this check prevents $O(E \log V)$ degradation.

```cpp
#include <vector>
#include <queue>
#include <utility>

struct Edge {
    int to;
    int weight;
};

// Time Complexity: O((V + E) log V)
// Space Complexity: O(V + E)
std::vector<int> dijkstra(int start, int n, const std::vector<std::vector<Edge>>& adj) {
    std::vector<int> dist(n, 1e9);
    // Min-heap of pair<dist, node>
    std::priority_queue<std::pair<int, int>, 
                        std::vector<std::pair<int, int>>, 
                        std::greater<std::pair<int, int>>> pq;
                        
    dist[start] = 0;
    pq.push({0, start});
    
    while (!pq.empty()) {
        std::pair<int, int> curr = pq.top();
        pq.pop();
        int d = curr.first;
        int u = curr.second;
        
        if (d > dist[u]) continue; // Stale path optimization
        
        for (const auto& edge : adj[u]) {
            int v = edge.to;
            int w = edge.weight;
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pq.push({dist[v], v});
            }
        }
    }
    return dist;
}
```

### Path Reconstruction
To print the actual shortest path, maintain a `parent` array.
```cpp
// Update step in Dijkstra:
if (dist[u] + w < dist[v]) {
    dist[v] = dist[u] + w;
    parent[v] = u;
    pq.push({dist[v], v});
}

// Reconstructing path:
std::vector<int> getPath(int target, const std::vector<int>& parent) {
    std::vector<int> path;
    for (int curr = target; curr != -1; curr = parent[curr]) {
        path.push_back(curr);
    }
    std::reverse(path.begin(), path.end());
    return path;
}
```

### Grid Dijkstra
* **Grid Traversal**: When traversing cell-by-cell, map $(r, c)$ coordinates to a 1D index `r * cols + c` or use coordinate structs.

```cpp
#include <vector>
#include <queue>
#include <utility>

struct Cell {
    int r, c, d;
    bool operator>(const Cell& other) const { return d > other.d; }
};

// Time Complexity: O(R * C log(R * C)), Space Complexity: O(R * C)
int dijkstraGrid(const std::vector<std::vector<int>>& grid) {
    int R = grid.size();
    int C = grid[0].size();
    
    std::vector<std::vector<int>> dist(R, std::vector<int>(C, 1e9));
    std::priority_queue<Cell, std::vector<Cell>, std::greater<Cell>> pq;
    
    dist[0][0] = grid[0][0];
    pq.push({0, 0, grid[0][0]});
    
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};
    
    while (!pq.empty()) {
        Cell curr = pq.top();
        pq.pop();
        
        if (curr.d > dist[curr.r][curr.c]) continue; // Stale path check
        if (curr.r == R - 1 && curr.c == C - 1) return curr.d;
        
        for (int i = 0; i < 4; ++i) {
            int nr = curr.r + dr[i];
            int nc = curr.c + dc[i];
            if (nr >= 0 && nr < R && nc >= 0 && nc < C) {
                int cost = curr.d + grid[nr][nc];
                if (cost < dist[nr][nc]) {
                    dist[nr][nc] = cost;
                    pq.push({nr, nc, cost});
                }
            }
        }
    }
    return -1;
}
```

### State-Based (2D) Dijkstra
* **State Expansion**: Define state as `(u, state)`. For example, `dist[u][k]` is the shortest path to $u$ using exactly $k$ coupon skips.
* **Typical Problem**: Cheapest Flights Within K Stops.

```cpp
#include <vector>
#include <queue>
#include <algorithm>

// Find shortest path from start to target with at most K free edges
int dijkstraWithKFreeEdges(int start, int target, int n, int K, 
                           const std::vector<std::vector<Edge>>& adj) {
    // dist[u][k] stores shortest path to u using exactly k free edges
    std::vector<std::vector<int>> dist(n, std::vector<int>(K + 1, 1e9));
    
    struct State {
        int d, u, k;
        bool operator>(const State& other) const { return d > other.d; }
    };
    
    std::priority_queue<State, std::vector<State>, std::greater<State>> pq;
    dist[start][0] = 0;
    pq.push({0, start, 0});
    
    while (!pq.empty()) {
        State curr = pq.top();
        pq.pop();
        int d = curr.d;
        int u = curr.u;
        int k = curr.k;
        
        if (d > dist[u][k]) continue;
        if (u == target) return d;
        
        for (const auto& edge : adj[u]) {
            int v = edge.to;
            int w = edge.weight;
            
            // Choice 1: Do not use a free edge
            if (dist[u][k] + w < dist[v][k]) {
                dist[v][k] = dist[u][k] + w;
                pq.push({dist[v][k], v, k});
            }
            
            // Choice 2: Use a free edge (if we have uses remaining)
            if (k < K && dist[u][k] < dist[v][k + 1]) {
                dist[v][k + 1] = dist[u][k]; // cost is 0
                pq.push({dist[v][k + 1], v, k + 1});
            }
        }
    }
    
    int ans = 1e9;
    for (int k = 0; k <= K; ++k) ans = std::min(ans, dist[target][k]);
    return (ans == 1e9) ? -1 : ans;
}
```

### Bottleneck Shortest Path (Minimax)
* **Goal**: Find a path from start to end that minimizes the **maximum edge weight** along the path.
* **Transition Modification**:
  $$\text{dist}[v] = \min(\text{dist}[v], \, \max(\text{dist}[u], \, \text{weight}(u, v)))$$

---

## 6. Bellman-Ford & SPFA

Used when graphs contain negative edge weights. Can identify negative-weight cycles.

### Standard Bellman-Ford
* **Idea**: Relax all $E$ edges $V-1$ times.
* **Cycle Detection**: On the $V$-th pass, if any distance decreases, a negative cycle exists.

```cpp
#include <vector>

struct EdgeInfo {
    int from, to, weight;
};

// Time Complexity: O(V * E)
// Space Complexity: O(V)
bool bellmanFord(int start, int n, const std::vector<EdgeInfo>& edges, std::vector<int>& dist) {
    dist.assign(n, 1e9);
    dist[start] = 0;
    
    for (int i = 0; i < n - 1; ++i) {
        for (const auto& edge : edges) {
            if (dist[edge.from] != 1e9 && dist[edge.from] + edge.weight < dist[edge.to]) {
                dist[edge.to] = dist[edge.from] + edge.weight;
            }
        }
    }
    
    // Check for negative cycle
    for (const auto& edge : edges) {
        if (dist[edge.from] != 1e9 && dist[edge.from] + edge.weight < dist[edge.to]) {
            return true; // Negative cycle detected!
        }
    }
    return false;
}
```

### SPFA (Shortest Path Faster Algorithm)
* **Concept**: An optimization of Bellman-Ford that only relaxes edges from nodes whose distances were updated in the previous step. It uses a queue to track active nodes.

```cpp
#include <vector>
#include <queue>

// Time Complexity: O(E) average, O(V * E) worst case
bool spfa(int start, int n, const std::vector<std::vector<Edge>>& adj, std::vector<int>& dist) {
    dist.assign(n, 1e9);
    std::vector<bool> in_queue(n, false);
    std::vector<int> relax_count(n, 0);
    
    std::queue<int> q;
    dist[start] = 0;
    q.push(start);
    in_queue[start] = true;
    
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        in_queue[u] = false;
        
        for (const auto& edge : adj[u]) {
            int v = edge.to;
            int w = edge.weight;
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                if (!in_queue[v]) {
                    q.push(v);
                    in_queue[v] = true;
                    relax_count[v]++;
                    if (relax_count[v] >= n) {
                        return true; // Negative cycle detected!
                    }
                }
            }
        }
    }
    return false;
}
```

---

## 7. Floyd-Warshall & Variations

Computes shortest paths between **all pairs of vertices** in $O(V^3)$ time. Keep intermediate vertex $k$ in the **outermost loop**.

```cpp
#include <vector>
#include <algorithm>

const int INF = 1e9;

void floydWarshall(int n, std::vector<std::vector<int>>& dist) {
    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (dist[i][k] < INF && dist[k][j] < INF) {
                    dist[i][j] = std::min(dist[i][j], dist[i][k] + dist[k][j]);
                }
            }
        }
    }
}
```

### Variations
* **Transitive Closure**: Determine if path exists from $i \to j$.
  `dist[i][j] = dist[i][j] || (dist[i][k] && dist[k][j])`
* **Minimax Path**: Minimize the maximum edge weight between $i \to j$.
  `dist[i][j] = min(dist[i][j], max(dist[i][k], dist[k][j]))`
* **Negative Cycle Detection**: A negative cycle exists if `dist[i][i] < 0` for any vertex $i$ after processing.

---

## 8. Strongly Connected Components (Kosaraju)

A Strongly Connected Component (SCC) is a maximal subgraph in a directed graph where every vertex is reachable from every other vertex.

### Core Kosaraju Template
```cpp
#include <vector>
#include <stack>
#include <algorithm>

void dfsFillOrder(int u, const std::vector<std::vector<int>>& adj, 
                  std::vector<bool>& visited, std::stack<int>& st) {
    visited[u] = true;
    for (int v : adj[u]) {
        if (!visited[v]) dfsFillOrder(v, adj, visited, st);
    }
    st.push(u);
}

void dfsCollectSCC(int u, const std::vector<std::vector<int>>& adj_T, 
                   std::vector<bool>& visited, std::vector<int>& current_scc) {
    visited[u] = true;
    current_scc.push_back(u);
    for (int v : adj_T[u]) {
        if (!visited[v]) dfsCollectSCC(v, adj_T, visited, current_scc);
    }
}

// Time Complexity: O(V + E)
// Space Complexity: O(V + E)
std::vector<std::vector<int>> getSCCs(int n, const std::vector<std::vector<int>>& adj) {
    std::vector<bool> visited(n, false);
    std::stack<int> st;
    
    // Pass 1: Topological finish ordering
    for (int i = 0; i < n; ++i) {
        if (!visited[i]) dfsFillOrder(i, adj, visited, st);
    }
    
    // Reverse graph
    std::vector<std::vector<int>> adj_T(n);
    for (int u = 0; u < n; ++u) {
        for (int v : adj[u]) {
            adj_T[v].push_back(u);
        }
    }
    
    // Pass 2: Extract SCC components
    visited.assign(n, false);
    std::vector<std::vector<int>> sccs;
    
    while (!st.empty()) {
        int u = st.top();
        st.pop();
        if (!visited[u]) {
            std::vector<int> current_scc;
            dfsCollectSCC(u, adj_T, visited, current_scc);
            sccs.push_back(current_scc);
        }
    }
    return sccs;
}
```

### Building the Condensation Graph (DAG)
* **Concept**: Collapsing each SCC into a single node transforms any general directed graph into a DAG.
* **Kosaraju topological property**: The order of components returned by the second pass is already a **topological sort** of the Condensation DAG (specifically, from sink components to source components).

```cpp
#include <vector>
#include <stack>
#include <set>

class CondensationGraph {
    std::vector<bool> visited;
    std::stack<int> st;
    
    void dfs1(int u, const std::vector<std::vector<int>>& adj) {
        visited[u] = true;
        for (int v : adj[u]) {
            if (!visited[v]) dfs1(v, adj);
        }
        st.push(u);
    }
    
    void dfs2(int u, const std::vector<std::vector<int>>& adj_T, 
              int scc_id, std::vector<int>& scc_map) {
        visited[u] = true;
        scc_map[u] = scc_id;
        for (int v : adj_T[u]) {
            if (!visited[v]) dfs2(v, adj_T, scc_id, scc_map);
        }
    }
public:
    // Returns condensation adjacency list, and node component mapping
    std::pair<std::vector<std::vector<int>>, std::vector<int>> 
    buildCondensation(int n, const std::vector<std::vector<int>>& adj) {
        visited.assign(n, false);
        for (int i = 0; i < n; ++i) {
            if (!visited[i]) dfs1(i, adj);
        }
        
        std::vector<std::vector<int>> adj_T(n);
        for (int u = 0; u < n; ++u) {
            for (int v : adj[u]) adj_T[v].push_back(u);
        }
        
        visited.assign(n, false);
        std::vector<int> scc_map(n, -1);
        int scc_counter = 0;
        
        while (!st.empty()) {
            int u = st.top();
            st.pop();
            if (!visited[u]) {
                dfs2(u, adj_T, scc_counter, scc_map);
                scc_counter++;
            }
        }
        
        // Build the condensed DAG (avoid duplicates using set)
        std::vector<std::set<int>> dag_sets(scc_counter);
        for (int u = 0; u < n; ++u) {
            for (int v : adj[u]) {
                if (scc_map[u] != scc_map[v]) {
                    dag_sets[scc_map[u]].insert(scc_map[v]);
                }
            }
        }
        
        std::vector<std::vector<int>> condensed_adj(scc_counter);
        for (int i = 0; i < scc_counter; ++i) {
            condensed_adj[i].assign(dag_sets[i].begin(), dag_sets[i].end());
        }
        
        return {condensed_adj, scc_map};
    }
};
```

---

## Graph Algorithms Complexity Cheat Sheet

| Algorithm | Time Complexity | Space Complexity | Best Used For |
| :--- | :--- | :--- | :--- |
| **DFS** | $O(V + E)$ | $O(V)$ | Connectivity, cycle detection, path existence |
| **BFS** | $O(V + E)$ | $O(V)$ | Shortest path in unweighted graphs |
| **0-1 BFS** | $O(V + E)$ | $O(V)$ | Shortest path with edge weights 0 or 1 |
| **Kahn's (Topo)**| $O(V + E)$ | $O(V)$ | Resolving scheduling dependencies on DAGs |
| **DSU** | $O(\alpha(V))$ | $O(V)$ | Dynamic connectivity, cycle checks, component merging |
| **Dijkstra** | $O((V + E) \log V)$ | $O(V + E)$ | Single-source shortest path (positive weights only) |
| **Bellman-Ford**| $O(V \times E)$ | $O(V)$ | Single-source shortest path with negative weights |
| **Floyd-Warshall**| $O(V^3)$ | $O(V^2)$ | All-pairs shortest paths |
| **Kosaraju (SCC)**| $O(V + E)$ | $O(V + E)$ | Finding strongly connected subgraphs in directed graphs |
