// ldd: -lncurses
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 4

typedef enum { UP, DOWN, LEFT, RIGHT, EXIT } key;

int board[SIZE][SIZE];
int score = 0;

/* ================= INIT ================= */

void INIT_2048() {
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    srand(time(NULL));
}

void DONE_2048() {
    endwin();
    exit(0);
}

void init_board() {
    for(int i=0;i<SIZE;i++)
        for(int j=0;j<SIZE;j++)
            board[i][j] = 0;
}

/* ================= DISPLAY ================= */

void display_board() {
    clear();
    mvprintw(0, 0, "2048 - Score: %d\n\n", score);

    for(int i=0;i<SIZE;i++){
        for(int j=0;j<SIZE;j++){
            if(board[i][j] == 0)
                printw("|    ");
            else
                printw("|%4d", board[i][j]);
        }
        printw("|\n");
    }

    refresh();
}

/* ================= UTIL ================= */

void swap(int* a, int* b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void mirror_board() {
    for(int i=0;i<SIZE;i++)
        for(int j=0;j<SIZE/2;j++)
            swap(&board[i][j], &board[i][SIZE-1-j]);
}

void pivot_board() {
    for(int i=0;i<SIZE;i++)
        for(int j=i+1;j<SIZE;j++)
            swap(&board[i][j], &board[j][i]);
}

/* ================= GAME LOGIC ================= */

int shift_board() {
    int moved = 0;

    for(int i=0;i<SIZE;i++){
        for(int j=0;j<SIZE;j++){
            if(board[i][j] == 0){
                for(int k=j+1;k<SIZE;k++){
                    if(board[i][k] != 0){
                        board[i][j] = board[i][k];
                        board[i][k] = 0;
                        moved = 1;
                        break;
                    }
                }
            }
        }
    }

    return moved;
}

int update_board() {
    int moved = 0;

    if(shift_board()) moved = 1;

    for(int i=0;i<SIZE;i++){
        for(int j=0;j<SIZE-1;j++){
            if(board[i][j] != 0 && board[i][j] == board[i][j+1]){
                board[i][j] *= 2;
                score += board[i][j];
                board[i][j+1] = 0;
                moved = 1;
            }
        }
    }

    if(shift_board()) moved = 1;

    return moved;
}

/* ================= RANDOM ================= */

int count_empty() {
    int count = 0;
    for(int i=0;i<SIZE;i++)
        for(int j=0;j<SIZE;j++)
            if(board[i][j] == 0) count++;
    return count;
}

void add_two(int empty) {
    int target = rand() % empty;
    int count = 0;

    for(int i=0;i<SIZE;i++){
        for(int j=0;j<SIZE;j++){
            if(board[i][j] == 0){
                if(count == target){
                    board[i][j] = 2;
                    return;
                }
                count++;
            }
        }
    }
}

int game_over(int add) {
    int empty = count_empty();

    if(empty == 0){
        mvprintw(10, 0, "Game Over! Press any key...");
        refresh();
        getch();
        return 1;
    }

    if(add){
        add_two(empty);
        display_board();
    }

    return 0;
}

/* ================= INPUT ================= */

key get_key() {
    int ch;

    while(1){
        ch = getch();
        switch(ch){
            case KEY_UP: return UP;
            case KEY_DOWN: return DOWN;
            case KEY_LEFT: return LEFT;
            case KEY_RIGHT: return RIGHT;
            case KEY_BACKSPACE: return EXIT;
        }
    }
}

/* ================= PLAY ================= */

int play(key dir) {
    int moved = 0;

    if(dir == DOWN){
        pivot_board();
        mirror_board();
        moved = update_board();
        mirror_board();
        pivot_board();
    }
    else if(dir == UP){
        pivot_board();
        moved = update_board();
        pivot_board();
    }
    else if(dir == RIGHT){
        mirror_board();
        moved = update_board();
        mirror_board();
    }
    else if(dir == LEFT){
        moved = update_board();
    }

    return moved;
}

/* ================= MAIN ================= */

int main() {
    INIT_2048();

    init_board();
    add_two(count_empty());
    add_two(count_empty());

    display_board();

    key k;
    int add = 1;

    while(1){
        k = get_key();

        if(k == EXIT)
            break;

        int moved = play(k);
        add = moved;

        if(game_over(add))
            break;
    }

    DONE_2048();
    return 0;
}