#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum { N = 8 };

typedef struct {
    char grid[N][N];
} board_t;

static char DEAD = '.';
static char ALIVE = 'X';
static int PERIOD_MS = 200;

int mod(int x, int m);
board_t* create_board();
board_t* clone_board(const board_t* board);
void delete_board(board_t* board);
char* board_get(board_t* board, int i, int j);
void advance_board(board_t* board);
void print_board(board_t* board);
void sleep_ms(int milliseconds);

int main(int argc, char **argv) {
    board_t* board = create_board();
    while(1) {
        print_board(board);
        sleep_ms(PERIOD_MS);
        advance_board(board);
    }
    delete_board(board);
    return 0;
}

// A correct modulus function
int mod(int x, int m) {
    if(m < 0) return -mod(-x, -m);;
    int ret = x % m;
    if(ret < 0) ret += m;
    return ret;
}

// Create a new board
board_t* create_board() {
    board_t* board = malloc(sizeof(board_t));
    int i, j;
    for(i = 0; i < N; i++) {
        for(j = 0; j < N; j++) {
            board->grid[i][j] = DEAD;
        }
    }

    *board_get(board, 5, 4) = ALIVE;
    *board_get(board, 5, 5) = ALIVE;
    *board_get(board, 5, 6) = ALIVE;

    return board;
}

// Clone a board
board_t* clone_board(const board_t* board) {
    board_t* clone = malloc(sizeof(board_t));
    int i, j;
    for(i = 0; i < N; i++) {
        for(j = 0; j < N; j++) {
            clone->grid[i][j] = board->grid[i][j];
        }
    }
    return clone;
}

// Destroy a board
void delete_board(board_t* board) {
    free(board);
}

// Access a board tile
char* board_get(board_t* board, int i, int j) {
    return &board->grid[mod(j, N)][mod(i, N)];
}

// Advance the board
void advance_board(board_t* board) {
    board_t* advanced_board = clone_board(board);
    int i, j;
    for(i = 0; i < N; i++) {
        for(j = 0; j < N; j++) {
            int live_neighbors = 0;
            int p, q;
            for(p = -1; p <= 1; p++) {
                for(q = -1; q <= 1; q++) {
                    if(p || q) {
                        live_neighbors += *board_get(board, i + p, j + q) == ALIVE;
                    }
                }
            }
            char* old_cell = board_get(board, i, j);
            char* new_cell = board_get(advanced_board, i, j);
            if(*old_cell == ALIVE) {
                if(live_neighbors < 2 || live_neighbors > 3) {
                    *new_cell = DEAD;
                }
            } else if(*old_cell == DEAD) {
                if(live_neighbors == 3) {
                    *new_cell = ALIVE;
                }
            }
        }
    }
    // board_t* temp = board;
    *board = *advanced_board;
    // delete_board(temp);
}

// Display the board
void print_board(board_t* board) {
    int i, j;
    for(i = 0; i < N; i++) {
        for(j = 0; j < N; j++) {
            printf("%c ", *board_get(board, j, i));
        }
        printf("\n");
    }
    printf("\n");
}

// Cross-platform sleep function
void sleep_ms(int milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}
