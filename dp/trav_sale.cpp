#include <iostream>
using namespace std;

int main() {
    // Read the number of cities.
    int n;
    cin >> n;

    // Define a large number to represent infinity.
    const int INF = 1000000000;

    // Dynamically allocate a 2D array for the cost matrix.
    int** cost = new int*[n];
    for (int i = 0; i < n; i++) {
        cost[i] = new int[n];
    }

    // Read the cost matrix.
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cin >> cost[i][j];
        }
    }

    // Calculate the total number of subsets (masks).
    int totalMasks = 1 << n;

    // Dynamically allocate the DP table.
    // dp[mask][i] holds the minimum cost to reach city i after visiting the cities encoded in 'mask'.
    int** dp = new int*[totalMasks];
    for (int mask = 0; mask < totalMasks; mask++) {
        dp[mask] = new int[n];
        for (int i = 0; i < n; i++) {
            dp[mask][i] = INF;
        }
    }

    // Starting state: only city 0 is visited and its cost is 0.
    dp[1][0] = 0;

    // Process all masks (subsets of visited cities).
    for (int mask = 0; mask < totalMasks; mask++) {
        for (int i = 0; i < n; i++) {
            // Skip if the current state is not reachable.
            if (dp[mask][i] == INF)
                continue;
            // Try to extend the tour by visiting an unvisited city j.
            for (int j = 0; j < n; j++) {
                // Check if city j is already visited in this mask.
                if (mask & (1 << j))
                    continue;
                int nextMask = mask | (1 << j);
                int newCost = dp[mask][i] + cost[i][j];
                // Update the DP state if a lower cost path is found.
                if (newCost < dp[nextMask][j])
                    dp[nextMask][j] = newCost;
            }
        }
    }

    // Complete the tour by returning to the starting city (city 0)
    int ans = INF;
    for (int i = 0; i < n; i++) {
        int tourCost = dp[totalMasks - 1][i] + cost[i][0];
        if (tourCost < ans)
            ans = tourCost;
    }

    // Output the minimal tour cost.
    cout << ans << endl;

    // Free the dynamically allocated memory for the dp table.
    for (int mask = 0; mask < totalMasks; mask++) {
        delete[] dp[mask];
    }
    delete[] dp;

    // Free the dynamically allocated memory for the cost matrix.
    for (int i = 0; i < n; i++) {
        delete[] cost[i];
    }
    delete[] cost;

    return 0;
}
