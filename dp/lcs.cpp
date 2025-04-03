class Solution {
    private:
        int solve(int i, int j, string& t1, string& t2, vector<vector<int>>& dp) {
            if(i==t1.length() || j==t2.length()) return 0;
            if(dp[i][j]!=-1) return dp[i][j];
            int result;
            if(t1[i]==t2[j]) result=1+solve(i+1, j+1, t1, t2, dp);
            else result = max(solve(i+1, j, t1, t2, dp), solve(i, j+1, t1, t2, dp));
            dp[i][j] = result;
            return result;
        }
    public:
        int longestCommonSubsequence(string text1, string text2) {
            vector<vector<int>> dp(text1.length(), vector<int> (text2.length(), -1));
            return solve(0, 0, text1, text2, dp);
        }
    };