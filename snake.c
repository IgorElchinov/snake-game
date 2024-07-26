#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <ncurses.h>

enum {
    FIELD_CELL = 0,
    SNAKE_HEAD = 1, 
    SNAKE_TAIL = 2,
    APPLE = 3,
    WALL = 4,
    MAXH = 14,
    MAXW = 57,
    END_DELAY = 100,
    MIN_MOVE_DELAY = 3 
};

const int WALL_CHAR = '#';
const int FIELD_CELL_CHAR = '.';
const int APPLE_CHAR = '@';
const int SNAKE_HEAD_CHAR = 'o';
const int SNAKE_TAIL_CHAR = '+';
const int ERROR_CHAR = '!';

int field[MAXH][MAXW];
int snake_xs[MAXH * MAXW];
int snake_ys[MAXH * MAXW];

int dir = -1;
int cur_row, cur_col;
int apple_row, apple_col;
int snake_lenght = 1;
int move_delay = 7;

void draw_field();
void draw_char(char c, int x, int y);
int legal(char cur_move);
int get_rand(int min, int max);
void gen_apple();
int move_up();
int move_down();
int move_left();
int move_right();
void update_snake();
void update_screen();
void update_screen_colored();
void update_screen_uncolored();
void update_move_delay();
void debug();

int
main() {
    srand(time(NULL));
    initscr();

    noecho();
    printw("Use W, S, A and D keys to move the snake\nPress any key to start\n"); 
    refresh();
    getch(); 
    clear();

    draw_field();
    cur_row = MAXH / 2;
    cur_col = MAXW / 2;
    snake_xs[0] = cur_col;
    snake_ys[0] = cur_row;
    field[cur_row][cur_col] = SNAKE_HEAD;
    gen_apple();
    update_screen();
    
    halfdelay(move_delay);
    char cur_move = getch();
    int step = 0;
    int exit = 0;
    time_t last_time = 0;
    time_t cur_time = time(NULL);
    while (step < INT_MAX) {
        ++step;
        time(&cur_time);
        int prev_dir = dir;
        switch(cur_move) {
            case 'w':
                dir = 0;
                break;
            case 's':
                dir = 1;
                break;
            case 'a':
                dir = 2;
                break;
            case 'd':
                dir = 3;
                break;
            case ERR:
                if (dir == -1) {
                    dir = 0;
                }
                break;
            default:
                dir = -1;
                break;
        }
        //if (dir != prev_dir || (cur_time - last_time) >= 1) {
        switch (dir) {
            case 0:
                exit = !move_up();
                break;
            case 1:
                exit = !move_down();
                break;
            case 2:
                exit = !move_left();
                break;
            case 3:
                exit = !move_right();
                break;
            default:
                exit = 1;
                break;
        }
        //}
        update_screen();
        if (exit) {
            break;
        }
        last_time = cur_time;
        cur_move = getch();
    }
    halfdelay(END_DELAY);

    move(MAXH, 0);
    printw("Game over\nYour score is %d\nPress any key to exit\n", snake_lenght - 1);
    getch();
    echo();
    endwin();
    return 0;
}

void draw_field() {
    for (int i = 0; i < MAXW; ++i) {
        field[0][i] = WALL;
    }
    for (int i = 1; i < MAXH - 1; ++i) {
        field[i][0] = WALL;
        for (int j = 1; j < MAXW - 1; ++j) {
            field[i][j] = FIELD_CELL;
        }
        field[i][MAXW - 1] = WALL;
    }
    for (int i = 0; i < MAXW; ++i) {
        field[MAXH - 1][i] = WALL;
    }
    refresh();
}

void draw_char(char c, int x, int y) {
    move(x, y);
    printw("%c", c);
    move(MAXH + 2, 0);
    printw("");
}

int legal(char cur_move) {
    return cur_move == 'w' || cur_move == 's' || cur_move == 'a' || cur_move == 'd' || cur_move == ERR;
}

int get_rand(int min, int max) {
    return min + rand() / ((RAND_MAX + 1u) / (max - min));
}

void gen_apple() {
    do {
        apple_row = get_rand(1, MAXH);
        apple_col = get_rand(1, MAXW);
    } while (field[apple_row][apple_col] != FIELD_CELL);
    field[apple_row][apple_col] = APPLE;
};

int move_up() {
    int next_row = cur_row - 1;
    if (field[next_row][cur_col] != FIELD_CELL && field[next_row][cur_col] != APPLE) {
        return 0;
    }
    if (field[next_row][cur_col] == APPLE) {
        snake_lenght++;
        gen_apple();
        update_move_delay();
    } else {
        field[snake_ys[snake_lenght - 1]][snake_xs[snake_lenght - 1]] = FIELD_CELL;
    }
    update_snake();
    field[next_row][cur_col] = SNAKE_HEAD;
    if (snake_lenght != 1) {
        field[cur_row][cur_col] = SNAKE_TAIL;
    }
    snake_xs[0] = cur_col;
    snake_ys[0] = next_row;
    cur_row = next_row;
    return 1;
}

int move_down() {
    int next_row = cur_row + 1;
    if (field[next_row][cur_col] != FIELD_CELL && field[next_row][cur_col] != APPLE) {
        return 0;
    }
    if (field[next_row][cur_col] == APPLE) {
        snake_lenght++;
        gen_apple();
        update_move_delay();
    } else {
        field[snake_ys[snake_lenght - 1]][snake_xs[snake_lenght - 1]] = FIELD_CELL;
    }
    update_snake();
    field[next_row][cur_col] = SNAKE_HEAD;
    if (snake_lenght != 1) {
        field[cur_row][cur_col] = SNAKE_TAIL;
    }
    snake_xs[0] = cur_col;
    snake_ys[0] = next_row;
    cur_row = next_row;
    return 1;
}

int move_left() {
    int next_col = cur_col - 1;
    if (field[cur_row][next_col] != FIELD_CELL && field[cur_row][next_col] != APPLE) {
        return 0;
    }
    if (field[cur_row][next_col] == APPLE) {
        snake_lenght++;
        gen_apple();
        update_move_delay();
    } else {
        field[snake_ys[snake_lenght - 1]][snake_xs[snake_lenght - 1]] = FIELD_CELL;
    }
    update_snake();
    field[cur_row][next_col] = SNAKE_HEAD;
    if (snake_lenght != 1) {
        field[cur_row][cur_col] = SNAKE_TAIL;
    }
    snake_xs[0] = next_col;
    snake_ys[0] = cur_row;
    cur_col = next_col;
    return 1;
}

int move_right() {
    int next_col = cur_col + 1;
    if (field[cur_row][next_col] != FIELD_CELL && field[cur_row][next_col] != APPLE) {
        return 0;
    }
    if (field[cur_row][next_col] == APPLE) {
        snake_lenght++;
        gen_apple();
        update_move_delay();
    } else {
        field[snake_ys[snake_lenght - 1]][snake_xs[snake_lenght - 1]] = FIELD_CELL;
    }
    update_snake();
    field[cur_row][next_col] = SNAKE_HEAD;
    if (snake_lenght != 1) {
        field[cur_row][cur_col] = SNAKE_TAIL;
    }
    snake_xs[0] = next_col;
    snake_ys[0] = cur_row;
    cur_col = next_col;
    return 1;
}

void update_snake() {
    for (int i = snake_lenght - 1; i > 0; --i) {
        snake_xs[i] = snake_xs[i - 1];
        snake_ys[i] = snake_ys[i - 1];
    }
}

void update_screen() {
    update_screen_uncolored();
    // if (!has_colors()) {
    //     update_screen_colored();
    // } else {
    //     update_screen_uncolored();
    // }
}

void update_screen_colored() {
    for (int i = 0; i < MAXH; ++i) {
        for (int j = 0; j < MAXW; ++j) {
            move(i, j);
            switch(field[i][j]) {
                case FIELD_CELL:
                    addch(FIELD_CELL_CHAR);
                    break;
                case SNAKE_HEAD:
                    addch(SNAKE_HEAD_CHAR | COLOR_GREEN);
                    break;
                case SNAKE_TAIL:
                    addch(SNAKE_TAIL_CHAR | COLOR_GREEN);
                    break;
                case APPLE:
                    addch(APPLE_CHAR | COLOR_RED);
                    break;
                case WALL:
                    addch(WALL_CHAR | COLOR_MAGENTA);
                    break;
                default:
                    addch(ERROR_CHAR | COLOR_BLUE);
            }
        }
        addch('\n');
    }
    move(MAXH, 0);
    printw("Score: %d", snake_lenght - 1);
    move(MAXH + 1, 0);
}

void update_screen_uncolored() {
    for (int i = 0; i < MAXH; ++i) {
        for (int j = 0; j < MAXW; ++j) {
            move(i, j);
            switch(field[i][j]) {
                case FIELD_CELL:
                    printw("%c", FIELD_CELL_CHAR);
                    break;
                case SNAKE_HEAD:
                    printw("%c", SNAKE_HEAD_CHAR);
                    break;
                case SNAKE_TAIL:
                    printw("%c", SNAKE_TAIL_CHAR);
                    break;
                case APPLE:
                    printw("%c", APPLE_CHAR);
                    break;
                case WALL:
                    printw("%c", WALL_CHAR);
                    break;
                default:
                    printw("%c", ERROR_CHAR);
            }
        }
        printw("\n");
    }
    move(MAXH, 0);
    printw("Score: %d", snake_lenght - 1);
    move(MAXH + 1, 0);
}

void update_move_delay() {
    if ((snake_lenght - 1) % 3 == 0 && move_delay > MIN_MOVE_DELAY) {
        move_delay--;
        halfdelay(move_delay);
    }
}

void debug() {
    for (int i = 2 + MAXH; i < 2 * MAXH + 2; ++i) {
        for (int j = 0; j < MAXW; ++j) {
            move(i, j);
            switch(field[i - 2 - MAXH][j]) {
                case FIELD_CELL:
                    printw("%c", FIELD_CELL_CHAR);
                    break;
                case SNAKE_HEAD:
                    printw("%c", SNAKE_HEAD_CHAR);
                    break;
                case SNAKE_TAIL:
                    printw("%c", SNAKE_TAIL_CHAR);
                    break;
                case APPLE:
                    printw("%c", APPLE_CHAR);
                    break;
                case WALL:
                    printw("%c", WALL_CHAR);
                    break;
                default:
                    printw("%c", ERROR_CHAR);
            }
        }
        printw("\n");
    }
    for (int i = 0; i < snake_lenght + 5; ++i) {
        printw("%d %d\n", snake_xs[i], snake_ys[i]);
    }
}
