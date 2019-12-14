/*
This is a very simple implementation of Conway's game of life.

By default, running this program will show 4 generations of the
game with a random initial configuration. 4 presets can be
specified by passing their names as command line arguments:

    - blinker
    - toad
    - beacon
    - glider

The command line flag '-f' will enable "forever" mode, which will
simulate the game until iterrupted.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// Game constants
enum { N = 8 };
const char DEAD = '.';
const char ALIVE = 'X';
const int PERIOD_MS = 200;

typedef char board_t[N][N];

typedef enum {
    _random,
    blinker,
    toad,
    beacon,
    glider
} initial_state;

void string_to_lower(char* s);
int mod(int x, int m);
board_t* create_board(initial_state state);
board_t* clone_board(const board_t* board);
void delete_board(board_t* board);
void randomize_board(board_t* board);
char* board_get(board_t* board, int i, int j);
void advance_board(board_t* board);
void print_board(board_t* board);
void sleep_ms(int milliseconds);

int main(int argc, char** argv) {
    // Parse cmd line args
    int i;
    int forever = 0;
    initial_state state = _random;
    for(i = 1; i < argc; i++) {
        char* arg = argv[i];
        string_to_lower(arg);
        if(!strcmp(arg, "-f")) forever = 1;
        else if(!strcmp(arg, "blinker")) state = blinker;
        else if(!strcmp(arg, "toad")) state = toad;
        else if(!strcmp(arg, "beacon")) state = beacon;
        else if(!strcmp(arg, "glider")) state = glider;
    }
    // Create board
    board_t* board = create_board(state);
    // Run
    if(forever) {
        while(1) {
            print_board(board);
            sleep_ms(PERIOD_MS);
            advance_board(board);
        }
    } else {
        int i;
        for(i = 0; i < 4; i++) {
            print_board(board);
            advance_board(board);
        }
    }
    // Delete board.
    delete_board(board);
    return 0;
}

// Convert string to lowercase
void string_to_lower(char* s) {
    for(; *s; s++) *s = tolower(*s);
}

// A correct modulus function
int mod(int x, int m) {
    if(m < 0) return -mod(-x, -m);;
    int ret = x % m;
    if(ret < 0) ret += m;
    return ret;
}

// Create a new board
board_t* create_board(initial_state state) {
    board_t* board = malloc(sizeof(board_t));
    int i, j;
    for(i = 0; i < N; i++) {
        for(j = 0; j < N; j++) {
            (*board)[i][j] = DEAD;
        }
    }

    switch (state) {
        case blinker: {
            printf("set blinker state\n");
            int j;
            for(j = 3; j <= 5; j++) *board_get(board, 4, j) = ALIVE;
            break;
        }
        case toad: {
            int i;
            for(i = 3; i <= 5; i++) *board_get(board, i, 3) = ALIVE;
            for(i = 2; i <= 4; i++) *board_get(board, i, 4) = ALIVE;
            break;
        }
        case beacon: {
            *board_get(board, 5, 1) = ALIVE;
            *board_get(board, 6, 1) = ALIVE;
            *board_get(board, 5, 2) = ALIVE;
            *board_get(board, 6, 2) = ALIVE;
            *board_get(board, 3, 3) = ALIVE;
            *board_get(board, 4, 3) = ALIVE;
            *board_get(board, 3, 4) = ALIVE;
            *board_get(board, 4, 4) = ALIVE;
            break;
        }
        case glider: {
            *board_get(board, 1, 0) = ALIVE;
            *board_get(board, 2, 1) = ALIVE;
            *board_get(board, 0, 2) = ALIVE;
            *board_get(board, 1, 2) = ALIVE;
            *board_get(board, 2, 2) = ALIVE;
            break;
        }
        default: randomize_board(board); break;
    }

    return board;
}

// Clone a board
board_t* clone_board(const board_t* board) {
    board_t* clone = malloc(sizeof(board_t));
    int i, j;
    for(i = 0; i < N; i++) {
        for(j = 0; j < N; j++) {
            (*clone)[i][j] = (*board)[i][j];
        }
    }
    return clone;
}

// Destroy a board
void delete_board(board_t* board) {
    free(board);
}

// Randomize a board
void randomize_board(board_t* board) {
    srand(time(0));
    int i, j;
    for(i = 0; i < N; i++) {
        for(j = 0; j < N; j++) {
            if(rand() % 2) *board_get(board, i, j) = DEAD;
            else *board_get(board, i, j) = ALIVE;
        }
    }
}

// Safely access a board cell. Understands the board is toroidal
char* board_get(board_t* board, int i, int j) {
    return &(*board)[mod(j, N)][mod(i, N)];
}

// Advance the board
void advance_board(board_t* board) {
    // Clone the board to have a copy of the old state to reference
    board_t* old_board = clone_board(board);
    // For each cell...
    int i, j;
    for(i = 0; i < N; i++) {
        for(j = 0; j < N; j++) {
            // Count live neighbors
            int live_neighbors = 0;
            int p, q;
            for(p = -1; p <= 1; p++) {
                for(q = -1; q <= 1; q++) {
                    if(p || q) {
                        live_neighbors += *board_get(old_board, i + p, j + q) == ALIVE;
                    }
                }
            }
            // Execute GoL rules
            char* old_cell = board_get(old_board, i, j);
            char* new_cell = board_get(board, i, j);
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
    delete_board(old_board);
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

// Sleep function
void sleep_ms(int milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}
