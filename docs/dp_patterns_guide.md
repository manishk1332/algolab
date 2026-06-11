# Dynamic Programming Master Guide: Fast Preparation for Exams & Interviews

This comprehensive guide covers the **11 essential Dynamic Programming (DP) patterns** repeatedly asked in Samsung, Online Assessments (OA), and technical interviews. All code implementations are written in standard, optimized **C++14** suitable for competitive programming and interview environments.

---

## Table of Contents
1. [The 4-Step DP Thinking Process](#the-4-step-dp-thinking-process)
2. [Pattern 1: 1D DP](#pattern-1-1d-dp)
3. [Pattern 2: Knapsack Family](#pattern-2-knapsack-family)
4. [Pattern 3: LCS (Longest Common Subsequence) Family](#pattern-3-lcs-family)
5. [Pattern 4: LIS (Longest Increasing Subsequence) Family](#pattern-4-lis-family)
6. [Pattern 5: Grid DP](#pattern-5-grid-dp)
7. [Pattern 6: Interval DP](#pattern-6-interval-dp)
8. [Pattern 7: Stock / State Machine DP](#pattern-7-stock--state-machine-dp)
9. [Pattern 8: Tree DP](#pattern-8-tree-dp)
10. [Pattern 9: Bitmask DP](#pattern-9-bitmask-dp)
11. [Pattern 10: Game DP](#pattern-10-game-dp)
12. [Pattern 11: Digit DP](#pattern-11-digit-dp)
13. [Exam Cheat Sheet & Identification Keywords](#exam-cheat-sheet--identification-keywords)

---

## The 4-Step DP Thinking Process

Before writing a single line of code, walk through these four stages:

```
┌──────────────────────────────────────────┐
│          1. DEFINE THE STATE             │
│ "What information uniquely describes the │
│        current subproblem configuration?"│
└────────────────────┬─────────────────────┘
                     ▼
┌──────────────────────────────────────────┐
│         2. FIND THE TRANSITION           │
│ "How does the current state relate to    │
│        smaller, previously solved states?"│
└────────────────────┬─────────────────────┘
                     ▼
┌──────────────────────────────────────────┐
│          3. DEFINE BASE CASES            │
│ "What are the simplest subproblems whose  │
│        answers are known trivially?"     │
└────────────────────┬─────────────────────┘
                     ▼
┌──────────────────────────────────────────┐
│        4. DETERMINE ITERATION ORDER      │
│ "How do we loop to guarantee dependencies│
│        are always solved first?"         │
└──────────────────────────────────────────┘
```

1. **Define State**: Define what `dp[i]` or `dp[i][j]` means in plain English.
2. **Find Transition**: Break down the decision at the current state. Usually, this is a choice: *take vs. skip*, *match vs. mismatch*, or *try all split points*.
3. **Base Case**: Set up values for empty sets, length 0, index 0, or negative bounds.
4. **Iteration Order**: Bottom-up loops must follow the direction of state dependency. If `dp[i]` depends on `dp[i-1]`, loop $i$ from $0 \to N$. If you are optimizing space, be mindful of direction (e.g., looping backwards in Knapsack).

---

## Pattern 1: 1D DP

Used when the decision at index $i$ depends only on a few preceding elements (e.g., $i-1$, $i-2$).

### 1. House Robber
* **Problem**: Maximize money stolen from $N$ houses. You cannot rob adjacent houses.
* **State**: `dp[i]` = Maximum money obtainable using the prefix array `nums[0...i]`.
* **Transition**:
  $$\text{dp}[i] = \max(\underbrace{\text{dp}[i-1]}_{\text{Skip house } i}, \quad \underbrace{\text{nums}[i] + \text{dp}[i-2]}_{\text{Rob house } i})$$
* **Space Optimization**: Since we only look back two steps, we only need two variables (`prev2` and `prev1`) to compute the next state.

```cpp
#include <vector>
#include <algorithm>
#include <iostream>

// Time Complexity: O(N)
// Space Complexity: O(1)
int rob(const std::vector<int>& nums) {
    int n = nums.size();
    if (n == 0) return 0;
    if (n == 1) return nums[0];
    
    int prev2 = nums[0];
    int prev1 = std::max(nums[0], nums[1]);
    
    for (int i = 2; i < n; ++i) {
        int curr = std::max(prev1, nums[i] + prev2);
        prev2 = prev1;
        prev1 = curr;
    }
    return prev1;
}
```

### 2. Min Cost Climbing Stairs
* **Problem**: Start at index 0 or 1. Pay `cost[i]` to climb 1 or 2 stairs. Find min cost to reach the top.
* **C++14 Implementation**:
```cpp
// Time Complexity: O(N), Space Complexity: O(1)
int minCostClimbingStairs(const std::vector<int>& cost) {
    int n = cost.size();
    int prev2 = 0; // Cost to reach step 0 (before pay)
    int prev1 = 0; // Cost to reach step 1 (before pay)
    
    for (int i = 2; i <= n; ++i) {
        int curr = std::min(prev1 + cost[i - 1], prev2 + cost[i - 2]);
        prev2 = prev1;
        prev1 = curr;
    }
    return prev1;
}
```

---

## Pattern 2: Knapsack Family

Determines how to select items under a weight limit $W$. There are three common types:
1. **0/1 Knapsack**: Each item can be taken at most **once**. Loop capacity **backwards**.
2. **Unbounded Knapsack**: Items can be reused **infinitely**. Loop capacity **forwards**.
3. **Subset Sum**: Choose subset matching target sum. (Special case of 0/1).

```
╔═══════════════════════════════════════════════════════════════════════════╗
║                      KNAPSACK LOOP DIRECTION RULES                        ║
╠═══════════════════════════════════════════════════════════════════════════╣
║ 1. 0/1 Knapsack (Finite Items)  => Outer loop: Items, Inner loop: W -> 0  ║
║ 2. Unbounded (Infinite Items)   => Outer loop: Items, Inner loop: 0 -> W  ║
║ 3. Permutations (Ordered Coins) => Outer loop: W,     Inner loop: Items  ║
╚═══════════════════════════════════════════════════════════════════════════╝
```

### 1. 0/1 Knapsack (Space Optimized)
* **State**: `dp[w]` = Maximum value achievable with capacity $w$.
* **Transition**:
  $$\text{dp}[w] = \max(\text{dp}[w], \, \text{value}[i] + \text{dp}[w - \text{weight}[i]])$$
* **Note**: Loop $w$ **downwards** from $W \to \text{weight}[i]$ to prevent the same item from updating subsequent states in the same round (which would model unbounded reuse).

```cpp
#include <vector>
#include <algorithm>

// Time Complexity: O(N * W)
// Space Complexity: O(W)
int knapsack01(int W, const std::vector<int>& weights, const std::vector<int>& values) {
    int n = weights.size();
    std::vector<int> dp(W + 1, 0);
    
    for (int i = 0; i < n; ++i) {
        for (int w = W; w >= weights[i]; --w) {
            dp[w] = std::max(dp[w], dp[w - weights[i]] + values[i]);
        }
    }
    return dp[W];
}
```

### 2. Subset Sum (Can We Form Target $S$?)
* **C++14 Implementation**:
```cpp
// Time Complexity: O(N * S), Space Complexity: O(S)
bool canPartition(const std::vector<int>& nums, int target) {
    std::vector<bool> dp(target + 1, false);
    dp[0] = true;
    
    for (int num : nums) {
        for (int s = target; s >= num; --s) {
            if (dp[s - num]) {
                dp[s] = true;
            }
        }
    }
    return dp[target];
}
```

### 3. Coin Change II (Total Combination Ways - Unbounded)
* **Problem**: Given coins, find the number of combinations that make up the target sum.
```cpp
// Time Complexity: O(N * Target), Space Complexity: O(Target)
int change(int amount, const std::vector<int>& coins) {
    std::vector<int> dp(amount + 1, 0);
    dp[0] = 1; // Base Case: 1 way to make sum 0
    
    for (int coin : coins) {
        // Forward iteration allows reuse of the same coin
        for (int s = coin; s <= amount; ++s) {
            dp[s] += dp[s - coin];
        }
    }
    return dp[amount];
}
```

---

## Pattern 3: LCS (Longest Common Subsequence) Family

Deals with operations on two sequences/strings, comparing prefixes of size $i$ and $j$.

### 1. Longest Common Subsequence with Path Reconstruction
* **State**: `dp[i][j]` = LCS length between `text1[0...i-1]` and `text2[0...j-1]`.
* **Transition**:
  - If match: $\text{dp}[i][j] = 1 + \text{dp}[i-1][j-1]$
  - Else: $\text{dp}[i][j] = \max(\text{dp}[i-1][j], \text{dp}[i][j-1])$

```cpp
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

// Time Complexity: O(N * M)
// Space Complexity: O(N * M) (Required to reconstruct path)
std::string getLCS(const std::string& text1, const std::string& text2) {
    int n = text1.size();
    int m = text2.size();
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));
    
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            if (text1[i - 1] == text2[j - 1]) {
                dp[i][j] = 1 + dp[i - 1][j - 1];
            } else {
                dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
            }
        }
    }
    
    // Backtracking path reconstruction
    std::string lcs_str = "";
    int i = n, j = m;
    while (i > 0 && j > 0) {
        if (text1[i - 1] == text2[j - 1]) {
            lcs_str.push_back(text1[i - 1]);
            --i; --j;
        } else if (dp[i - 1][j] >= dp[i][j - 1]) {
            --i;
        } else {
            --j;
        }
    }
    std::reverse(lcs_str.begin(), lcs_str.end());
    return lcs_str;
}
```

### 2. Edit Distance
* **Problem**: Convert `word1` to `word2` using insert, delete, or replace operations.
```cpp
// Time Complexity: O(N * M), Space Complexity: O(N * M)
int minDistance(const std::string& word1, const std::string& word2) {
    int n = word1.size();
    int m = word2.size();
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));
    
    for (int i = 0; i <= n; ++i) dp[i][0] = i;
    for (int j = 0; j <= m; ++j) dp[0][j] = j;
    
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            if (word1[i - 1] == word2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                dp[i][j] = 1 + std::min({
                    dp[i - 1][j],     // Delete
                    dp[i][j - 1],     // Insert
                    dp[i - 1][j - 1]  // Replace
                });
            }
        }
    }
    return dp[n][m];
}
```

---

## Pattern 4: LIS (Longest Increasing Subsequence) Family

Finds the length of the longest subsequence in which elements strictly increase.

### 1. Standard LIS ($O(N^2)$)
* **Use Case**: Simple to write; safe if $N \le 2000$.
* **State**: `dp[i]` = Length of LIS ending at index $i$.
```cpp
// Time Complexity: O(N^2), Space Complexity: O(N)
int lisQuadratic(const std::vector<int>& nums) {
    int n = nums.size();
    if (n == 0) return 0;
    std::vector<int> dp(n, 1);
    int max_len = 1;
    
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < i; ++j) {
            if (nums[j] < nums[i]) {
                dp[i] = std::max(dp[i], dp[j] + 1);
            }
        }
        max_len = std::max(max_len, dp[i]);
    }
    return max_len;
}
```

### 2. Optimized Binary Search LIS ($O(N \log N)$)
* **Concept**: Maintain a active collection `tails` where `tails[i]` stores the smallest tail value among all increasing subsequences of length `i+1` found so far.
* **C++14 Tip**:
  - For **Strictly Increasing**: Use `std::lower_bound` (replaces values $\ge$ key).
  - For **Non-Decreasing (Equal values allowed)**: Use `std::upper_bound` (replaces values $>$ key).

```cpp
#include <vector>
#include <algorithm>

// Time Complexity: O(N log N)
// Space Complexity: O(N)
int lisBinarySearch(const std::vector<int>& nums) {
    std::vector<int> tails;
    for (int num : nums) {
        auto it = std::lower_bound(tails.begin(), tails.end(), num);
        if (it == tails.end()) {
            tails.push_back(num); // Grows LIS
        } else {
            *it = num; // Lowers boundary value for length 'it - tails.begin() + 1'
        }
    }
    return tails.size();
}
```

---

## Pattern 5: Grid DP

Moves along a 2D matrix (usually only **Right** and **Down** direction).

### 1. Unique Paths II (Grid with Obstacles)
* **Problem**: Count paths from $(0, 0)$ to $(R-1, C-1)$. Obstacles are represented by `1`.
* **State**: `dp[j]` = Ways to reach current column $j$. We optimize space to $O(C)$ by running a rolling row.
* **Transition**:
  - If obstacle: `dp[j] = 0`
  - Else: `dp[j] = dp[j] (from top) + dp[j-1] (from left)`

```cpp
#include <vector>

// Time Complexity: O(R * C)
// Space Complexity: O(C)
int uniquePathsWithObstacles(const std::vector<std::vector<int>>& obstacleGrid) {
    int r = obstacleGrid.size();
    if (r == 0) return 0;
    int c = obstacleGrid[0].size();
    
    std::vector<long long> dp(c, 0);
    dp[0] = (obstacleGrid[0][0] == 0) ? 1 : 0;
    
    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
            if (obstacleGrid[i][j] == 1) {
                dp[j] = 0; // Blocked path
            } else if (j > 0) {
                dp[j] += dp[j - 1]; // Left contribution
            }
        }
    }
    return dp[c - 1];
}
```

### 2. Minimum Path Sum
```cpp
// Time Complexity: O(R * C), Space Complexity: O(C)
int minPathSum(const std::vector<std::vector<int>>& grid) {
    int r = grid.size();
    int c = grid[0].size();
    std::vector<int> dp(c, 2e9); // Infinity initialization
    dp[0] = 0;
    
    for (int i = 0; i < r; ++i) {
        dp[0] += grid[i][0]; // Accumulate down first column
        for (int j = 1; j < c; ++j) {
            if (i == 0) {
                dp[j] = dp[j - 1] + grid[i][j]; // First row base case
            } else {
                dp[j] = grid[i][j] + std::min(dp[j], dp[j - 1]);
            }
        }
    }
    return dp[c - 1];
}
```

---

## Pattern 6: Interval DP

Computes states based on range queries over segments $[l, r]$. Outer loop represents the segment length.

### 1. Matrix Chain Multiplication (MCM)
* **Problem**: Determine the cheapest order to multiply $N$ matrices.
* **State**: `dp[i][j]` = Min multiplications to compute matrices $i$ through $j$.
* **Transition**:
  $$\text{dp}[i][j] = \min_{i \le k < j} \left( \text{dp}[i][k] + \text{dp}[k+1][j] + \text{dims}[i] \times \text{dims}[k+1] \times \text{dims}[j+1] \right)$$

```cpp
#include <vector>
#include <algorithm>

// Time Complexity: O(N^3)
// Space Complexity: O(N^2)
int mcm(const std::vector<int>& dims) {
    int n = dims.size() - 1; // Number of matrices
    std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));
    
    for (int len = 2; len <= n; ++len) { // Segment length
        for (int i = 0; i <= n - len; ++i) {
            int j = i + len - 1;
            dp[i][j] = 1e9;
            for (int k = i; k < j; ++k) {
                int cost = dp[i][k] + dp[k + 1][j] + dims[i] * dims[k + 1] * dims[j + 1];
                dp[i][j] = std::min(dp[i][j], cost);
            }
        }
    }
    return dp[0][n - 1];
}
```

### 2. Burst Balloons
* **Exam Trap**: Do not think "which balloon to burst first" (as it breaks boundary conditions). Instead, decide: **"which balloon is burst last"** in range $[l, r]$.
```cpp
// Time Complexity: O(N^3), Space Complexity: O(N^2)
int maxCoins(const std::vector<int>& nums) {
    int n = nums.size();
    std::vector<int> val(n + 2, 1);
    for (int i = 0; i < n; ++i) val[i + 1] = nums[i];
    
    std::vector<std::vector<int>> dp(n + 2, std::vector<int>(n + 2, 0));
    
    for (int len = 1; len <= n; ++len) {
        for (int l = 1; l <= n - len + 1; ++l) {
            int r = l + len - 1;
            for (int k = l; k <= r; ++k) {
                dp[l][r] = std::max(dp[l][r], 
                    dp[l][k - 1] + dp[k + 1][r] + val[l - 1] * val[k] * val[r + 1]);
            }
        }
    }
    return dp[1][n];
}
```

---

## Pattern 7: Stock / State Machine DP

Models transitions as states in a state machine. Typical options: Holding (1) vs. Free (0).

### 1. Buy & Sell Stock with 1-Day Cooldown
* **Transitions**:
  - `buy[i]`: holding a stock. Transited from holding from previous day (`buy[i-1]`) OR buying today after a 1-day cooldown (`sell[i-2] - prices[i]`).
  - `sell[i]`: not holding a stock. Transited from nothing from previous day (`sell[i-1]`) OR selling stock today (`buy[i-1] + prices[i]`).

```cpp
#include <vector>
#include <algorithm>

// Time Complexity: O(N)
// Space Complexity: O(1) (State variables optimized)
int maxProfitCooldown(const std::vector<int>& prices) {
    int n = prices.size();
    if (n <= 1) return 0;
    
    int buy = -prices[0];
    int sell = 0;
    int prev_sell = 0; // Represents sell[i-2]
    
    for (int i = 1; i < n; ++i) {
        int next_buy = std::max(buy, prev_sell - prices[i]);
        int next_sell = std::max(sell, buy + prices[i]);
        prev_sell = sell;
        buy = next_buy;
        sell = next_sell;
    }
    return sell;
}
```

### 2. Buy & Sell Stock with Transaction Fee
* **C++14 Implementation**:
```cpp
// Time Complexity: O(N), Space Complexity: O(1)
int maxProfitFee(const std::vector<int>& prices, int fee) {
    int n = prices.size();
    if (n == 0) return 0;
    
    int buy = -prices[0];
    int sell = 0;
    
    for (int i = 1; i < n; ++i) {
        buy = std::max(buy, sell - prices[i]);
        sell = std::max(sell, buy + prices[i] - fee);
    }
    return sell;
}
```

---

## Pattern 8: Tree DP

Computes values on a hierarchical structure. Data is aggregated from bottom to top (Leaves $\to$ Root) using Depth First Search (DFS).

### 1. House Robber III (Robbing Node in Binary Tree)
* **Problem**: Rob nodes in a tree, cannot rob parent and child directly.
* **Return**: `pair<int, int>` $\to$ `{Robbed_Root_Value, Skipped_Root_Value}`.

```cpp
#include <algorithm>
#include <utility>

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

// Time Complexity: O(N) (single pass)
// Space Complexity: O(H) recursion stack
std::pair<int, int> robSub(TreeNode* root) {
    if (!root) return {0, 0};
    
    std::pair<int, int> left = robSub(root->left);
    std::pair<int, int> right = robSub(root->right);
    
    // Rob: cannot take child nodes
    int rob_curr = root->val + left.second + right.second;
    // Skip: take max profit option of children
    int skip_curr = std::max(left.first, left.second) + std::max(right.first, right.second);
    
    return {rob_curr, skip_curr};
}

int robTree(TreeNode* root) {
    std::pair<int, int> result = robSub(root);
    return std::max(result.first, result.second);
}
```

### 2. Tree Diameter
```cpp
#include <vector>
#include <algorithm>

class TreeDiameter {
    int max_diameter;
    
    int dfs(int u, int p, const std::vector<std::vector<int>>& adj) {
        int max1 = 0, max2 = 0;
        for (int v : adj[u]) {
            if (v == p) continue;
            int height = dfs(v, u, adj);
            if (height > max1) {
                max2 = max1;
                max1 = height;
            } else if (height > max2) {
                max2 = height;
            }
        }
        max_diameter = std::max(max_diameter, max1 + max2);
        return 1 + max1;
    }
public:
    int getDiameter(int n, const std::vector<std::vector<int>>& adj) {
        max_diameter = 0;
        dfs(0, -1, adj);
        return max_diameter;
    }
};
```

---

## Pattern 9: Bitmask DP

Used to track subsets and visitations when $N \le 20$. Uses integer bits as a boolean array representation.

### 1. Traveling Salesperson Problem (TSP) Bottom-Up
* **State**: `dp[mask][u]` = Min cost to visit cities set in `mask` ending at node `u`.
* **C++14 Bit Operators**:
  - Check if visited: `(mask & (1 << v))`
  - Set visited: `mask | (1 << v)`

```cpp
#include <vector>
#include <algorithm>

const int INF = 1e9;

// Time Complexity: O(2^N * N^2)
// Space Complexity: O(2^N * N)
int tsp(int n, const std::vector<std::vector<int>>& dist) {
    std::vector<std::vector<int>> dp(1 << n, std::vector<int>(n, INF));
    
    // Base Case: start at node 0
    dp[1][0] = 0; 
    
    for (int mask = 1; mask < (1 << n); ++mask) {
        for (int u = 0; u < n; ++u) {
            if (dp[mask][u] == INF) continue;
            
            // Try all possible next destination steps
            for (int v = 0; v < n; ++v) {
                if (!(mask & (1 << v))) { // v has not been visited
                    int next_mask = mask | (1 << v);
                    dp[next_mask][v] = std::min(dp[next_mask][v], dp[mask][u] + dist[u][v]);
                }
            }
        }
    }
    
    // Cost to return back to start (node 0)
    int ans = INF;
    for (int u = 1; u < n; ++u) {
        if (dp[(1 << n) - 1][u] != INF) {
            ans = std::min(ans, dp[(1 << n) - 1][u] + dist[u][0]);
        }
    }
    return ans;
}
```

---

## Pattern 10: Game DP

Analyzes state values (Winning vs. Losing) for two players making optimal alternate moves.

### 1. Stone Game (Subtraction Game)
* **Rule**: Remove $1, 3,$ or $4$ stones. Player taking the last stone wins.
* **Property**: Current position is winning if **any** reachable next move forces the opponent into a losing state.

```cpp
#include <vector>

// Time Complexity: O(N * |Moves|)
// Space Complexity: O(N)
bool canWinStoneGame(int n) {
    std::vector<bool> dp(n + 1, false);
    std::vector<int> moves = {1, 3, 4};
    
    for (int i = 1; i <= n; ++i) {
        for (int move : moves) {
            if (i >= move && !dp[i - move]) { // Opponent loses
                dp[i] = true;
                break;
            }
        }
    }
    return dp[n];
}
```

### 2. Stone Game II (Picking from ends - Minimax Relative Score)
```cpp
// Time Complexity: O(N^2), Space Complexity: O(N^2)
int stoneGameRelative(const std::vector<int>& piles) {
    int n = piles.size();
    std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));
    
    for (int i = 0; i < n; ++i) dp[i][i] = piles[i];
    
    for (int len = 2; len <= n; ++len) {
        for (int i = 0; i <= n - len; ++i) {
            int j = i + len - 1;
            dp[i][j] = std::max(piles[i] - dp[i + 1][j], piles[j] - dp[i][j - 1]);
        }
    }
    return dp[0][n - 1] > 0;
}
```

---

## Pattern 11: Digit DP

Counts numbers in range $[1, N]$ matching a digit condition. Processed digit-by-digit from left to right.

### 1. Count Numbers $\le N$ with Digit Sum = $K$
* **State**: `dp[pos][sum][tight]`
  - `pos`: Current index being chosen.
  - `sum`: Total sum of digits selected so far.
  - `tight`: Boolean. If `true`, current prefix matches $N$'s prefix. We are constrained: the next digit cannot exceed $N$'s current digit. If `false`, we can choose any digit $[0, 9]$.

```cpp
#include <iostream>
#include <string>
#include <vector>
#include <cstring>

class DigitDP {
    std::string N;
    int target_sum;
    int memo[20][200][2]; // Maximum 20 digits, max sum ~180, tight (0 or 1)
    
    int solve(int pos, int sum, bool tight) {
        if (pos == N.length()) {
            return (sum == target_sum) ? 1 : 0;
        }
        if (sum > target_sum) return 0;
        
        int t = tight ? 1 : 0;
        if (memo[pos][sum][t] != -1) return memo[pos][sum][t];
        
        int limit = tight ? (N[pos] - '0') : 9;
        int ans = 0;
        
        for (int d = 0; d <= limit; ++d) {
            ans += solve(pos + 1, sum + d, tight && (d == limit));
        }
        return memo[pos][sum][t] = ans;
    }
public:
    int count(const std::string& limit_str, int k) {
        N = limit_str;
        target_sum = k;
        std::memset(memo, -1, sizeof(memo));
        return solve(0, 0, true);
    }
};
```

> [!TIP]
> **Range Queries Interview Hack**: To find matching numbers in $[L, R]$:
> 1. Calculate `solve(R)`.
> 2. Calculate `solve(L-1)`.
> 3. Result is `solve(R) - solve(L-1)`.
> 
> *If subtracting 1 from a string $L$ is hard (e.g. $L = 1000 \to 999$)*: Calculate `solve(R) - solve(L)` and manually add $1$ if $L$ itself fits the condition.

---

## Exam Cheat Sheet & Identification Keywords

| Pattern | Time Complexity | Space Complexity | Common Keywords / Indicators | Space Optimization Trick |
| :--- | :--- | :--- | :--- | :--- |
| **1D DP** | $O(N)$ | $O(N)$ or $O(1)$ | Rob, climb stairs, consecutive choice restrictions | Save only last few states (`prev1`, `prev2`) |
| **Knapsack** | $O(N \times W)$ | $O(W)$ | Weight limit, sum targets, subset partitioned, infinite supply | Loop capacity **backwards** for 0/1; **forwards** for unbounded |
| **LCS** | $O(N \times M)$ | $O(N \times M)$ or $O(M)$ | Two strings, transform $A \to B$, common subsequence | Rolling rows (1D arrays) if reconstruction isn't needed |
| **LIS** | $O(N \log N)$ | $O(N)$ | Increasing order subsequence, sorting constraints | `tails` array + `std::lower_bound` |
| **Grid DP**| $O(R \times C)$ | $O(C)$ | Move Right/Down, Grid pathways, blockages | Maintain only the previous row/column states |
| **Interval**| $O(N^3)$ | $O(N^2)$ | Choose partition splits, chain matrices, balloon burst | Outer loop controls range length: $len = 2 \dots N$ |
| **Stock** | $O(N)$ | $O(1)$ | Hold/Sell states, cooldown restriction, transaction fees | Maintain `buy` and `sell` state variables |
| **Tree DP** | $O(N)$ | $O(H)$ | Binary Tree robbery, tree diameter, subtree weights | Recursion returning `std::pair` or `struct` variables |
| **Bitmask** | $O(2^N \times N^2)$ | $O(2^N \times N)$ | Visit $N \le 20$ elements, permutation, Hamiltonian | Bit operations: `1 << i`, `&`, `\|` |
| **Game DP** | $O(State \times Moves)$| $O(State)$ | Play optimally, stones removal, score difference | Standard winning check: `!opponent_state` |
| **Digit** | $O(\text{digits} \times \text{sum} \times 2)$| $O(\text{digits} \times \text{sum})$| Count numbers in range $[L, R]$, digit criteria | `tight` flag indicates if bounds constraint applies |
