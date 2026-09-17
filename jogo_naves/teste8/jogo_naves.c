#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

int WindowWidth = 800;
int WindowHeight = 600;

typedef enum { MENU, PLAYING, GAME_OVER, EXIT_GAME } GameState;
GameState gameState = MENU;

int playerScore = 0;
int lives = 3;

float objectX = 400.0f;
float objectY = 10.0f;

const float playerWidth = 50.0f;
const float playerHeight = 20.0f;
const float bulletSpeed = 5.0f;
const float Speed = 30.0f;
const float targetSpeed = 2.0f;

/* ── Estruturas (sem bool — usa int) ────────────────────────── */
typedef struct { float x, y; int active; } Bullet;
typedef struct { float x, y; int active; } Target;

#define MAX_BULLETS 100
#define MAX_TARGETS 100

Bullet bullets[MAX_BULLETS];
Target targets[MAX_TARGETS];
int bulletCount = 0;
int targetCount = 0;

/* ── Utilitários ─────────────────────────────────────────────── */
void drawRectangle(float x, float y, float w, float h) {
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void drawText(const char* text, float x, float y) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
}

/* sprintf para int → string em C */
void drawTextInt(const char* label, int value, float x, float y) {
    char buf[64];
    sprintf_s(buf, sizeof(buf), "%s%d", label, value);
    drawText(buf, x, y);
}

/* ── Reshape ─────────────────────────────────────────────────── */
void reshape(int width, int height) {
    if (height == 0) height = 1;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    WindowWidth = width;
    WindowHeight = height;
}

/* ── Display ─────────────────────────────────────────────────── */
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (gameState == MENU) {
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText("2D Shooting Game", WindowWidth / 2.0f - 100, WindowHeight / 2.0f + 50);
        drawText("Click to Start", WindowWidth / 2.0f - 80, WindowHeight / 2.0f);
        drawText("Exit", WindowWidth / 2.0f - 30, WindowHeight / 2.0f - 50);
    }
    else if (gameState == PLAYING) {
        int i;

        /* Jogador (verde) */
        glColor3f(0.0f, 1.0f, 0.0f);
        drawRectangle(objectX, 10, playerWidth, playerHeight);

        /* Balas (vermelho) */
        glColor3f(1.0f, 0.0f, 0.0f);
        for (i = 0; i < bulletCount; i++)
            if (bullets[i].active)
                drawRectangle(bullets[i].x, bullets[i].y, 5.0f, 10.0f);

        /* Targets (azul) */
        glColor3f(0.0f, 0.0f, 1.0f);
        for (i = 0; i < targetCount; i++)
            if (targets[i].active)
                drawRectangle(targets[i].x, targets[i].y, 40.0f, 20.0f);

        /* HUD */
        glColor3f(1.0f, 1.0f, 1.0f);
        drawTextInt("Score: ", playerScore, 10, WindowHeight - 30);
        drawTextInt("Lives: ", lives, 10, WindowHeight - 50);

        if (lives <= 0) gameState = GAME_OVER;
    }
    else if (gameState == GAME_OVER) {
        glColor3f(1.0f, 0.0f, 0.0f);
        drawText("Game Over", WindowWidth / 2.0f - 50, WindowHeight / 2.0f + 20);
        drawTextInt("Final Score: ", playerScore, WindowWidth / 2.0f - 70, WindowHeight / 2.0f);
        drawText("Press R to Restart or ESC to Exit", WindowWidth / 2.0f - 150, WindowHeight / 2.0f - 50);
    }

    glutSwapBuffers();
}

/* ── Teclado ─────────────────────────────────────────────────── */
void keyboard(unsigned char key, int x, int y) {
    if (gameState == PLAYING) {
        switch (key) {
        case 'w': case 'W': objectY += Speed; break;
        case 's': case 'S': objectY -= Speed; break;
        case 'a': case 'A': objectX -= Speed; break;
        case 'd': case 'D': objectX += Speed; break;
        case ' ':
            if (bulletCount < MAX_BULLETS) {
                bullets[bulletCount].x = objectX + playerWidth / 2 - 2.5f;
                bullets[bulletCount].y = 30.0f;
                bullets[bulletCount].active = 1;
                bulletCount++;
            }
            break;
        case 27: gameState = MENU; break;
        }
        if (objectX < 0) objectX = 0;
        if (objectX + playerWidth > WindowWidth) objectX = WindowWidth - playerWidth;
    }
    else if (gameState == GAME_OVER) {
        if (key == 'r' || key == 'R') {
            playerScore = 0;
            lives = 3;
            objectX = WindowWidth / 2.0f - playerWidth / 2.0f;
            objectY = 10.0f;
            bulletCount = 0;
            targetCount = 0;
            gameState = PLAYING;
        }
        if (key == 27) exit(0);
    }
    else if (gameState == MENU) {
        if (key == 27) exit(0);
    }
    glutPostRedisplay();
}

/* ── Setas ───────────────────────────────────────────────────── */
void specialKeys(int key, int x, int y) {
    if (gameState != PLAYING) return;
    switch (key) {
    case GLUT_KEY_UP:    objectY += Speed; break;
    case GLUT_KEY_DOWN:  objectY -= Speed; break;
    case GLUT_KEY_LEFT:  objectX -= Speed; break;
    case GLUT_KEY_RIGHT: objectX += Speed; break;
    }
    glutPostRedisplay();
}

/* ── Rato ────────────────────────────────────────────────────── */
void mouse(int button, int state, int x, int y) {
    if (gameState == MENU && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        float oy = (float)(WindowHeight - y);
        if (x > WindowWidth / 2.0f - 100 && x < WindowWidth / 2.0f + 100 &&
            oy > WindowHeight / 2.0f - 10 && oy < WindowHeight / 2.0f + 20)
            gameState = PLAYING;
        if (x > WindowWidth / 2.0f - 30 && x < WindowWidth / 2.0f + 30 &&
            oy > WindowHeight / 2.0f - 60 && oy < WindowHeight / 2.0f - 30)
            exit(0);
    }
}

/* ── Timer (~60 FPS) ─────────────────────────────────────────── */
void timer(int value) {
    if (gameState == PLAYING) {
        int i, j;

        /* Move balas */
        for (i = 0; i < bulletCount; i++) {
            if (bullets[i].active) {
                bullets[i].y += bulletSpeed;
                if (bullets[i].y > WindowHeight)
                    bullets[i].active = 0;
            }
        }

        /* Move targets */
        for (i = 0; i < targetCount; i++) {
            if (targets[i].active) {
                targets[i].y -= targetSpeed;
                if (targets[i].y < 0) {
                    targets[i].active = 0;
                    lives--;
                }
            }
        }

        /* Colisão bala ↔ target */
        for (i = 0; i < bulletCount; i++) {
            if (!bullets[i].active) continue;
            for (j = 0; j < targetCount; j++) {
                if (!targets[j].active) continue;
                if (bullets[i].x < targets[j].x + 40 &&
                    bullets[i].x + 5 > targets[j].x &&
                    bullets[i].y < targets[j].y + 20 &&
                    bullets[i].y + 10 > targets[j].y) {
                    bullets[i].active = 0;
                    targets[j].active = 0;
                    playerScore += 10;
                }
            }
        }

        /* Spawn targets */
        if (rand() % 50 == 0 && targetCount < MAX_TARGETS) {
            targets[targetCount].x = (float)(rand() % (WindowWidth - 40));
            targets[targetCount].y = (float)WindowHeight;
            targets[targetCount].active = 1;
            targetCount++;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

/* ── Menu contexto ───────────────────────────────────────────── */
void menuCallback(int option) {
    switch (option) {
    case 1: gameState = PLAYING; break;
    case 2: exit(0);
    }
}

void createMenu() {
    glutCreateMenu(menuCallback);
    glutAddMenuEntry("Start Game", 1);
    glutAddMenuEntry("Quit", 2);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/* ── Main ────────────────────────────────────────────────────── */
int main(int argc, char** argv) {
    srand((unsigned int)time(NULL));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WindowWidth, WindowHeight);
    glutCreateWindow("2D Shooting Game");

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WindowWidth, 0, WindowHeight);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(16, timer, 0);
    glutFullScreen();
    createMenu();

    glutMainLoop();
    return 0;
}