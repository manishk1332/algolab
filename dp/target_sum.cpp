class Solution {
    public:
      bool solve(int i, int target, vector<int>& arr, int** dp) {
          if(i<0) {
              if(target==0) return true;
              return false;
          }
          if(target==0) return true;
          if(dp[i][target]!=-1) return dp[i][target];
          int result;
          if(arr[i]>target) result = solve(i-1, target, arr, dp);
          else result = solve(i-1, target, arr, dp) || solve(i-1, target-arr[i], arr, dp);
          dp[i][target] = result;
          return result;
      }
    
      bool isSubsetSum(vector<int>& arr, int target) {
          // code here
          int n = arr.size();
          int sum=0;
          for(int i:arr) {
              sum+=i;
          }
          if(sum<target) return false;
          int** dp=new int*[n];
          for(int i=0;i<n;i++) {
              dp[i] = new int[target+1];
          }
          for(int i=0;i<n;i++) {
              for(int j=0;j<=target;j++) {
                  dp[i][j] = -1;
              }
          }
          return solve(n-1, target, arr, dp);
      }
  };