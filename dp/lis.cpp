class Solution {
    public:
        int lengthOfLIS(vector<int>& nums) {
            vector<int> dp(nums.size(), 0);
            dp[0] = 1;
            for(int i=1;i<nums.size();i++) {
                int max=0;
                for(int j=0;j<i;j++) {
                    if(nums[j]<nums[i]) {
                        if(dp[j]>max) max=dp[j];
                    }
                }
                dp[i] = 1+max;
            }
            return *max_element(dp.begin(), dp.end());
        }
    };