#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

int g_blockCoords[9][9][2] = { {{0,0}, {0,1}, {0,2}, {1,0}, {1,1}, {1,2}, {2,0}, {2,1}, {2,2}},
                               {{0,3}, {0,4}, {0,5}, {1,3}, {1,4}, {1,5}, {2,3}, {2,4}, {2,5}},
                               {{0,6}, {0,7}, {0,8}, {1,6}, {1,7}, {1,8}, {2,6}, {2,7}, {2,8}},

                               {{3,0}, {3,1}, {3,2}, {4,0}, {4,1}, {4,2}, {5,0}, {5,1}, {5,2}},
                               {{3,3}, {3,4}, {3,5}, {4,3}, {4,4}, {4,5}, {5,3}, {5,4}, {5,5}},
                               {{3,6}, {3,7}, {3,8}, {4,6}, {4,7}, {4,8}, {5,6}, {5,7}, {5,8}},

                               {{6,0}, {6,1}, {6,2}, {7,0}, {7,1}, {7,2}, {8,0}, {8,1}, {8,2}},
                               {{6,3}, {6,4}, {6,5}, {7,3}, {7,4}, {7,5}, {8,3}, {8,4}, {8,5}},
                               {{6,6}, {6,7}, {6,8}, {7,6}, {7,7}, {7,8}, {8,6}, {8,7}, {8,8}}};

void PrintBoard(char board[][9], int boardSize, int* boardColSize)
{
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
}

/*
 * |0|1|2|
 * |3|4|5|
 * |6|7|8|
 * */
int GetBlockInd(int x, int y)
{
    if (x < 3) {
        if (y < 3) {
            return 0;
        } else if (y < 6) {
            return 1;
        } else {
            return 2;
        }
    } else if (x < 6) {
        if (y < 3) {
            return 3;
        } else if (y < 6) {
            return 4;
        } else {
            return 5;
        }
    } else {
        if (y < 3) {
            return 6;
        } else if (y < 6) {
            return 7;
        } else {
            return 8;
        }
    }
}


void MarkOptions(char board[][9], int options[9], int x, int y)
{
    // options are all 1s when passed in
    // if it's a determined already
    if (board[x][y] != '.') {
        for (int i = 0; i < 9; i++) {
            if (i != board[x][y] - '1') {
                options[i] = -1;
            }
        }
        return;
    } else {
        // eliminate row
        for (int i = 0; i < 9; i++) {
            if (board[x][i] != '.') {
                options[board[x][i] - '1'] = -1;
            }
        }
        // eliminate col
        for (int i = 0; i < 9; i++) {
            if (board[i][y] != '.') {
                options[board[i][y] - '1'] = -1;
            }
        }
        // eliminate block
        int blockId = GetBlockInd(x, y);
        for (int i = 0; i < 9; i++) {
            char cur = board[g_blockCoords[blockId][i][0]][g_blockCoords[blockId][i][1]];
            if (cur != '.') {
                options[cur - '1'] = -1;
            }
        }
    }
}

bool Check(char board[][9], int x, int y, char ch){
    int k;
    // search row
    for (k = 0; k < 9; k++) {
        if (board[x][k] == ch) {
            return false;
        }
    }
    // search col
    for (k = 0; k < 9; k++) {
        if (board[k][y] == ch) {
            return false;
        }
    }
    // search block
    int blockId = GetBlockInd(x, y);
    for (k = 0; k < 9; k++) {
        int newX = g_blockCoords[blockId][k][0];
        int newY = g_blockCoords[blockId][k][1];
        if (board[newX][newY] == ch) {
            return false;
        }
    }
    return true;
}

bool Backtracking(char board[][9], int options[81][9], int ind)
{
    if (ind == 81) return true;
    int x = ind / 9;
    int y = ind % 9;
    if (board[x][y] != '.') return Backtracking(board, options, ind + 1);
    for (int i = 0; i < 9; i++) {
        if (options[ind][i] == 0 && Check(board, x, y, (char)('1' + i))) {
            // forward
            board[x][y] = (char)('1' + i);
            if (Backtracking(board, options, ind + 1)) return true;
            board[x][y] = '.';
        }
    }
    return false;
}



void solveSudoku(char board[][9], int boardSize, int* boardColSize){
    // prepare options for empty cells(pruning for backtracking)
    int options[81][9] = {0};
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            MarkOptions(board, options[i * 9 + j], i, j);
        }
    }
    // backtracking: whenever conflicts happens, tracking; fill in value when it's the only option
    Backtracking(board, options, 0);
}

int main()
{
    char input[9][9] = {{'5','3','.','.','7','.','.','.','.'},
                        {'6','.','.','1','9','5','.','.','.'},
                        {'.','9','8','.','.','.','.','6','.'},
                        {'8','.','.','.','6','.','.','.','3'},
                        {'4','.','.','8','.','3','.','.','1'},
                        {'7','.','.','.','2','.','.','.','6'},
                        {'.','6','.','.','.','.','2','8','.'},
                        {'.','.','.','4','1','9','.','.','5'},
                        {'.','.','.','.','8','.','.','7','9'}};
    int boardSize[] = {9, 9, 9, 9, 9, 9, 9, 9, 9};
    solveSudoku(input, 9, boardSize);
    PrintBoard(input, 9, boardSize);
    return 0;
}
