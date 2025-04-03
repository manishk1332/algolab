#include <iostream>
using namespace std;

const int MAX = 50;

// Helper functions
int min2(int a, int b) { return (a < b) ? a : b; }
int max2(int a, int b) { return (a > b) ? a : b; }

// Check if the subarray of F starting at (si,sj) of size (r x c) has the chessboard pattern.
// The expected color at offset (i,j) is determined by (i+j) even/odd relative to the top–left cell.
bool isChessboard(int F[MAX][MAX], int m, int n, int si, int sj, int r, int c) {
    int base = F[si][sj];
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            int expected = ((i + j) % 2 == 0) ? base : 1 - base;
            if(F[si + i][sj + j] != expected)
                return false;
        }
    }
    return true;
}

// (1) Exhaustive search for largest square with chessboard pattern.
void essquare(int F[MAX][MAX], int m, int n, int &best_i, int &best_j, int &best_size) {
    int bestArea = 0;
    best_size = 0;
    best_i = best_j = 0;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            int maxLen = min2(m - i, n - j);
            for (int len = maxLen; len >= 1; len--) {
                if(len * len <= bestArea)
                    break;
                if(isChessboard(F, m, n, i, j, len, len)) {
                    bestArea = len * len;
                    best_i = i;
                    best_j = j;
                    best_size = len;
                    break;
                }
            }
        }
    }
}

// (2) Exhaustive search for largest rectangle with chessboard pattern.
void esrectangle(int F[MAX][MAX], int m, int n, int &best_i, int &best_j, int &best_h, int &best_w) {
    int bestArea = 0;
    best_i = best_j = best_h = best_w = 0;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            int maxH = m - i;
            int maxW = n - j;
            for (int h = 1; h <= maxH; h++) {
                for (int w = 1; w <= maxW; w++) {
                    if(h * w <= bestArea)
                        continue;
                    if(isChessboard(F, m, n, i, j, h, w)) {
                        bestArea = h * w;
                        best_i = i;
                        best_j = j;
                        best_h = h;
                        best_w = w;
                    }
                }
            }
        }
    }
}

// (3) Dynamic Programming for the largest square.
// T[i][j] will store the size of the largest chessboard square starting at (i,j).
void dpsquare(int F[MAX][MAX], int m, int n, int &best_i, int &best_j, int &best_size) {
    int T[MAX][MAX];
    int bestArea = 0;
    best_size = 0;
    best_i = best_j = 0;
    
    for (int i = m - 1; i >= 0; i--) {
        for (int j = n - 1; j >= 0; j--) {
            if(i == m - 1 || j == n - 1) {
                T[i][j] = 1;
            } else {
                if(F[i][j] == F[i+1][j] || F[i][j] == F[i][j+1])
                    T[i][j] = 1;
                else {
                    int l1 = T[i+1][j];
                    int l2 = T[i][j+1];
                    if(l1 < l2)
                        T[i][j] = l1 + 1;
                    else if(l1 > l2)
                        T[i][j] = l2 + 1;
                    else { // l1 == l2
                        if(i + l1 < m && j + l1 < n && F[i+l1][j+l1] == F[i][j])
                            T[i][j] = l1 + 1;
                        else
                            T[i][j] = l1;
                    }
                }
            }
            if(T[i][j] * T[i][j] > bestArea) {
                bestArea = T[i][j] * T[i][j];
                best_i = i;
                best_j = j;
                best_size = T[i][j];
            }
        }
    }
}

// (4) Dynamic Programming for the largest rectangle with chessboard pattern.
// We precompute an auxiliary array alt[i][j][c] which gives the length of the maximal alternating
// segment in row i starting at column j if we require the cell to be of color c.
void dprectangle(int F[MAX][MAX], int m, int n, int &best_i, int &best_j, int &best_h, int &best_w) {
    int alt[MAX][MAX][2]; // third index: 0 or 1 for expected color.
    
    // Precompute alternating sequences per row.
    for (int i = 0; i < m; i++) {
        for (int j = n - 1; j >= 0; j--) {
            for (int c = 0; c < 2; c++) {
                if(F[i][j] == c) {
                    if(j == n - 1)
                        alt[i][j][c] = 1;
                    else
                        alt[i][j][c] = 1 + alt[i][j+1][1-c];
                } else {
                    alt[i][j][c] = 0;
                }
            }
        }
    }
    
    int bestArea = 0;
    best_i = best_j = best_h = best_w = 0;
    
    // For each candidate top-left corner.
    for (int i0 = 0; i0 < m; i0++) {
        for (int j0 = 0; j0 < n; j0++) {
            int base = F[i0][j0]; // starting color of the rectangle.
            int minWidth = alt[i0][j0][base]; // maximum valid width in the first row.
            if(minWidth > 0) {
                if(minWidth > bestArea) {
                    bestArea = minWidth;
                    best_i = i0;
                    best_j = j0;
                    best_h = 1;
                    best_w = minWidth;
                }
            }
            // Grow downward and update the minimum width.
            for (int h = 2; i0 + h - 1 < m; h++) {
                int i_current = i0 + h - 1;
                // For row i_current, the expected starting color is base if (h-1) is even, else opposite.
                int expected = ((h - 1) % 2 == 0) ? base : (1 - base);
                int curWidth = alt[i_current][j0][expected];
                if(curWidth < minWidth)
                    minWidth = curWidth;
                int area = h * minWidth;
                if(area > bestArea) {
                    bestArea = area;
                    best_i = i0;
                    best_j = j0;
                    best_h = h;
                    best_w = minWidth;
                }
            }
        }
    }
}

// A function to print the m x n floor. The solution subarray (starting at (top,left) with
// dimensions (height x width)) is printed with 'W' or 'B' while the rest is printed as dots.
void printSolution(int F[MAX][MAX], int m, int n, int top, int left, int height, int width) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if(i >= top && i < top + height && j >= left && j < left + width)
                cout << (F[i][j] == 0 ? "W" : "B") << " ";
            else
                cout << ". ";
        }
        cout << "\n";
    }
    cout << "Area = " << height * width << "\n";
}

int main(){
    int m, n;
    cin >> m >> n;
    int F[MAX][MAX];
    for (int i = 0; i < m; i++){
        for (int j = 0; j < n; j++){
            cin >> F[i][j];
        }
    }
    
    // (A) Exhaustive search: Square
    int esq_i, esq_j, esq_size;
    essquare(F, m, n, esq_i, esq_j, esq_size);
    
    // (B) Exhaustive search: Rectangle
    int esr_i, esr_j, esr_h, esr_w;
    esrectangle(F, m, n, esr_i, esr_j, esr_h, esr_w);
    
    // (C) Dynamic Programming: Square
    int dpsq_i, dpsq_j, dpsq_size;
    dpsquare(F, m, n, dpsq_i, dpsq_j, dpsq_size);
    
    // (D) Dynamic Programming: Rectangle
    int dpr_i, dpr_j, dpr_h, dpr_w;
    dprectangle(F, m, n, dpr_i, dpr_j, dpr_h, dpr_w);
    
    // Print results in the required format.
    cout << "+++ Exhaustive search: Square" << "\n";
    printSolution(F, m, n, esq_i, esq_j, esq_size, esq_size);
    cout << "\n";
    
    cout << "+++ Exhaustive search: Rectangle" << "\n";
    printSolution(F, m, n, esr_i, esr_j, esr_h, esr_w);
    cout << "\n";
    
    cout << "+++ Dynamic Programming: Square" << "\n";
    printSolution(F, m, n, dpsq_i, dpsq_j, dpsq_size, dpsq_size);
    cout << "\n";
    
    cout << "+++ Dynamic Programming: Rectangle" << "\n";
    printSolution(F, m, n, dpr_i, dpr_j, dpr_h, dpr_w);
    
    return 0;
}
