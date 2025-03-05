#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define WIDTH 40
#define HEIGHT 20

int i, j, gameOver;
int x, y, fruitX, fruitY, score;
int tailX[100], tailY[100];
int nTail;
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN};
enum eDirection dir;

void Setup() {
    gameOver = 0;
    dir = STOP;
    x = WIDTH / 2;
    y = HEIGHT / 2;
    fruitX = rand() % WIDTH;
    fruitY = rand() % HEIGHT;
    score = 0;
}

void Draw() {
    clear(); // Clear the screen

    // Draw the top wall
    for (i = 0; i < WIDTH + 2; i++)
        mvaddch(0, i, '#');
    mvaddch(0, WIDTH + 1, '\n');

    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            if (j == 0 || j == WIDTH - 1)  // Draw side walls
                mvaddch(i + 1, j, '#');
            else if (i == y && j == x)  // Draw snake head
                mvaddch(i + 1, j, 'O');
            else if (i == fruitY && j == fruitX)  // Draw fruit
                mvaddch(i + 1, j, '*');
            else {
                int tailDrawn = 0;
                for (int k = 0; k < nTail; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        mvaddch(i + 1, j, 'o');
                        tailDrawn = 1;
                        break;
                    }
                }
                if (!tailDrawn)
                    mvaddch(i + 1, j, ' ');
            }
        }
        mvaddch(i + 1, WIDTH, '\n');
    }

    // Draw the bottom wall
    for (i = 0; i < WIDTH + 2; i++)
        mvaddch(HEIGHT + 1, i, '#');
    mvaddch(HEIGHT + 1, WIDTH + 1, '\n');

    mvprintw(HEIGHT + 2, 0, "Score: %d", score);
    refresh();
}

void Input() {
    int c = getch();
    switch(c) {
        case 'a':
            if (dir != RIGHT) dir = LEFT;
            break;
        case 'd':
            if (dir != LEFT) dir = RIGHT;
            break;
        case 'w':
            if (dir != DOWN) dir = UP;
            break;
        case 's':
            if (dir != UP) dir = DOWN;
            break;
        case 'x':
            gameOver = 1;
            break;
    }
}

void Logic() {
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;

    for (i = 1; i < nTail; i++) {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    switch(dir) {
        case LEFT:
            x--;
            break;
        case RIGHT:
            x++;
            break;
        case UP:
            y--;
            break;
        case DOWN:
            y++;
            break;
        default:
            break;
    }

    // Check for wall collision
    if (x >= WIDTH - 1 || x < 0 || y >= HEIGHT - 1 || y < 0)
        gameOver = 1;

    // Check for self-collision
    for (i = 0; i < nTail; i++)
        if (tailX[i] == x && tailY[i] == y)
            gameOver = 1;

    // Check if fruit was eaten
    if (x == fruitX && y == fruitY) {
        score += 10;
        fruitX = rand() % WIDTH;
        fruitY = rand() % HEIGHT;
        nTail++;
    }
}

int main() {
    initscr();              // Initialize ncurses
    cbreak();               // Disable buffering of stdin
    noecho();               // Don't echo keyboard input
    keypad(stdscr, TRUE);   // Enable special keys (F1, arrow keys etc.)
    nodelay(stdscr, TRUE);  // Make getch() non-blocking
    curs_set(0);            // Hide cursor

    srand(time(NULL));      // Seed the random number generator

    Setup();
    while (!gameOver) {
        Draw();
        Input();
        Logic();
        usleep(100000);     // Slow down the game loop, adjust for speed
    }

    endwin();               // Clean up after ncurses
    printf("Game Over! Your score: %d\n", score);
    return 0;
}
