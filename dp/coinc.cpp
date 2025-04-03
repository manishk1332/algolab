class Solution {
    public:
        int coinChange(vector<int>& coins, int amount) {
            sort(coins.begin(), coins.end());
            vector<int> dp(amount+1, 0);
            int minn, diff, temp;
            for(int i=1;i<=amount;i++) {
                minn = INT_MAX;
                for(int coin:coins) {
                    diff = i - coin;
                    if(diff<0) break;
                    else {
                        temp = dp[diff];
                        if(temp!=INT_MAX) minn = min(minn, temp+1);
                    }
                }
                dp[i] = minn;
            }
    
    
            if(dp[amount]<INT_MAX) return dp[amount];
            else return -1;
        }
    };