class Solution {
    public:
        int change(int amount, vector<int>& coins) {
            sort(coins.begin(), coins.end());
            int m = coins.size(), diff;
            unsigned long long tmp1, tmp2;
            unsigned long long** dp = new unsigned long long*[m];
            for(int i=0;i<m;i++) {
                dp[i] = new unsigned long long[amount+1];
            }
            for(int i=0;i<m;i++) {
                dp[i][0] = 1;
            }
            for(int col=1;col<=amount;col++) {
                for(int row=0;row<m;row++) {
                    diff = col-coins[m-(row+1)];
                    if(diff>=0) tmp1 = dp[row][diff];
                    else tmp1=0;
                    if(row!=0) tmp2 = dp[row-1][col];
                    else tmp2=0;
                    dp[row][col] = tmp1+tmp2;
                }
            }
            return dp[m-1][amount];
        }
    };