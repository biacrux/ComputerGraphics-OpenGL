#define _CRT_SECURE_NO_WARNINGS

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

/* ================================================
   CONSTANTES
   ================================================ */

#define MAX_BULLETS     100
#define MAX_TARGETS     100
#define PLAYER_WIDTH    50.0f
#define PLAYER_HEIGHT   20.0f
#define BULLET_W        5.0f
#define BULLET_H        10.0f
#define TARGET_W        40.0f
#define TARGET_H        20.0f
#define PLAYER_SPEED    4.0f
#define BULLET_SPEED    7.0f
#define TARGET_SPEED    2.0f
#define TIMER_MS        16
#define SPAWN_CHANCE    50

   /* ================================================
      TIPOS
      ================================================ */

typedef enum { MENU, PLAYING, GAME_OVER } GameState;

typedef struct {
    float x, y;
    int   active;
} Bullet;

typedef struct {
    float x, y;
    int   active;
} Target;

/* ================================================
   ESTADO GLOBAL
   ================================================ */

static int       WindowWidth = 800;
static int       WindowHeight = 600;
static GameState gameState = MENU;

static float playerX;
static float playerY;
static int   playerScore;
static int   lives;

static int keysHeld[256];
static int specialKeysHeld[8];

#define KEY_UP    0
#define KEY_DOWN  1
#define KEY_LEFT  2
#define KEY_RIGHT 3

static Bullet bullets[MAX_BULLETS];
static int    bulletCount;
static Target targets[MAX_TARGETS];
static int    targetCount;

/* ================================================
   FUNCOES AUXILIARES DE DESENHO
   ================================================ */

static void drawRectangle(float x, float y, float w, float h)
{
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

static void drawText(const char* text, float x, float y)
{
    const char* c;
    glRasterPos2f(x, y);
    for (c = text; *c != '\0'; c++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
}

static void drawTextCentered(const char* text, float cy)
{
    float x = WindowWidth / 2.0f - (strlen(text) * 5.0f);
    drawText(text, x, cy);
}

/* ================================================
   LOGICA DO JOGO
   ================================================ */

static void resetGame(void)
{
    int i;
    playerScore = 0;
    lives = 3;
    playerX = (WindowWidth - PLAYER_WIDTH) / 2.0f;
    playerY = 10.0f;

    for (i = 0; i < MAX_BULLETS; i++) bullets[i].active = 0;
    bulletCount = 0;

    for (i = 0; i < MAX_TARGETS; i++) targets[i].active = 0;
    targetCount = 0;

    memset(keysHeld, 0, sizeof(keysHeld));
    memset(specialKeysHeld, 0, sizeof(specialKeysHeld));
}

static void spawnBullet(void)
{
    int i;
    for (i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].x = playerX + PLAYER_WIDTH / 2.0f - BULLET_W / 2.0f;
            bullets[i].y = playerY + PLAYER_HEIGHT;
            bullets[i].active = 1;
            if (i >= bulletCount) bulletCount = i + 1;
            break;
        }
    }
}

static void spawnTarget(void)
{
    int i;
    for (i = 0; i < MAX_TARGETS; i++) {
        if (!targets[i].active) {
            targets[i].x = (float)(rand() % (WindowWidth - (int)TARGET_W));
            targets[i].y = (float)WindowHeight;
            targets[i].active = 1;
            if (i >= targetCount) targetCount = i + 1;
            break;
        }
    }
}

static void loseLife(void)
{
    lives--;
    if (lives <= 0)
        gameState = GAME_OVER;
}

static void updatePlayer(void)
{
    if (keysHeld['a'] || keysHeld['A'] || specialKeysHeld[KEY_LEFT]) {
        playerX -= PLAYER_SPEED;
        if (playerX < 0) playerX = 0;
    }
    if (keysHeld['d'] || keysHeld['D'] || specialKeysHeld[KEY_RIGHT]) {
        playerX += PLAYER_SPEED;
        if (playerX + PLAYER_WIDTH > WindowWidth)
            playerX = WindowWidth - PLAYER_WIDTH;
    }
    if (keysHeld['w'] || keysHeld['W'] || specialKeysHeld[KEY_UP]) {
        playerY += PLAYER_SPEED;
        if (playerY + PLAYER_HEIGHT > WindowHeight)
            playerY = WindowHeight - PLAYER_HEIGHT;
    }
    if (keysHeld['s'] || keysHeld['S'] || specialKeysHeld[KEY_DOWN]) {
        playerY -= PLAYER_SPEED;
        if (playerY < 10) playerY = 10;
    }
}

static void updateBullets(void)
{
    int i;
    for (i = 0; i < bulletCount; i++) {
        if (!bullets[i].active) continue;
        bullets[i].y += BULLET_SPEED;
        if (bullets[i].y > WindowHeight)
            bullets[i].active = 0;
    }
}

static void updateTargets(void)
{
    int i;
    for (i = 0; i < targetCount; i++) {
        if (!targets[i].active) continue;
        targets[i].y -= TARGET_SPEED;
        if (targets[i].y + TARGET_H < 0) {
            targets[i].active = 0;
            loseLife();
        }
    }
}

static void checkCollisions(void)
{
    int i, j;
    for (i = 0; i < bulletCount; i++) {
        if (!bullets[i].active) continue;
        for (j = 0; j < targetCount; j++) {
            if (!targets[j].active) continue;
            if (bullets[i].x            < targets[j].x + TARGET_W &&
                bullets[i].x + BULLET_W > targets[j].x &&
                bullets[i].y            < targets[j].y + TARGET_H &&
                bullets[i].y + BULLET_H > targets[j].y)
            {
                bullets[i].active = 0;
                targets[j].active = 0;
                playerScore++;
            }
        }
    }
}

/* ================================================
   CALLBACKS GLUT
   ================================================ */

void display(void)
{
    int  i;
    char buf[64];

    glClear(GL_COLOR_BUFFER_BIT);

    switch (gameState) {

    case MENU:
        glColor3f(1.0f, 1.0f, 1.0f);
        drawTextCentered("=== 2D Shooting Game ===", WindowHeight / 2.0f + 60);
        drawTextCentered("WASD ou Setas = Mover  |  Espaco = Disparar", WindowHeight / 2.0f + 20);

        glColor3f(0.2f, 0.7f, 0.2f);
        drawRectangle(WindowWidth / 2.0f - 80, WindowHeight / 2.0f - 60, 160, 35);
        glColor3f(1.0f, 1.0f, 1.0f);
        drawTextCentered("Start Game", WindowHeight / 2.0f - 50);

        glColor3f(0.7f, 0.2f, 0.2f);
        drawRectangle(WindowWidth / 2.0f - 80, WindowHeight / 2.0f - 110, 160, 35);
        glColor3f(1.0f, 1.0f, 1.0f);
        drawTextCentered("Exit", WindowHeight / 2.0f - 100);
        break;

    case PLAYING:
        /* Jogador */
        glColor3f(0.0f, 1.0f, 0.0f);
        drawRectangle(playerX, playerY, PLAYER_WIDTH, PLAYER_HEIGHT);

        /* Balas */
        glColor3f(1.0f, 1.0f, 0.0f);
        for (i = 0; i < bulletCount; i++)
            if (bullets[i].active)
                drawRectangle(bullets[i].x, bullets[i].y, BULLET_W, BULLET_H);

        /* Alvos */
        glColor3f(0.9f, 0.2f, 0.2f);
        for (i = 0; i < targetCount; i++)
            if (targets[i].active)
                drawRectangle(targets[i].x, targets[i].y, TARGET_W, TARGET_H);

        /* HUD */
        glColor3f(1.0f, 1.0f, 1.0f);
        sprintf(buf, "Score: %d", playerScore);
        drawText(buf, 10, WindowHeight - 25);
        sprintf(buf, "Vidas: %d", lives);
        drawText(buf, 10, WindowHeight - 50);
        break;

    case GAME_OVER:
        glColor3f(1.0f, 0.3f, 0.3f);
        drawTextCentered("GAME OVER", WindowHeight / 2.0f + 40);
        glColor3f(1.0f, 1.0f, 1.0f);
        sprintf(buf, "Pontuacao final: %d", playerScore);
        drawTextCentered(buf, WindowHeight / 2.0f);
        drawTextCentered("R = Recomecar   |   ESC = Sair", WindowHeight / 2.0f - 40);
        break;

    default:
        break;
    }

    glutSwapBuffers();
}

void reshape(int width, int height)
{
    if (height == 0) height = 1;
    WindowWidth = width;
    WindowHeight = height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
    (void)x; (void)y;

    switch (gameState) {
    case PLAYING:
        keysHeld[(int)key] = 1;
        switch (key) {
        case ' ':  spawnBullet();    break;
        case 27:   gameState = MENU; break;
        default:                     break;
        }
        break;

    case MENU:
        switch (key) {
        case 27: exit(0); break;
        default:          break;
        }
        break;

    case GAME_OVER:
        switch (key) {
        case 'r': case 'R': resetGame(); gameState = PLAYING; break;
        case 27:            exit(0);                           break;
        default:                                               break;
        }
        break;

    default:
        break;
    }
    glutPostRedisplay();
}

void keyboardUp(unsigned char key, int x, int y)
{
    (void)x; (void)y;
    keysHeld[(int)key] = 0;
}

void specialKeys(int key, int x, int y)
{
    (void)x; (void)y;
    if (gameState != PLAYING) return;
    switch (key) {
    case GLUT_KEY_UP:    specialKeysHeld[KEY_UP] = 1; break;
    case GLUT_KEY_DOWN:  specialKeysHeld[KEY_DOWN] = 1; break;
    case GLUT_KEY_LEFT:  specialKeysHeld[KEY_LEFT] = 1; break;
    case GLUT_KEY_RIGHT: specialKeysHeld[KEY_RIGHT] = 1; break;
    default:                                              break;
    }
}

void specialKeysUp(int key, int x, int y)
{
    (void)x; (void)y;
    switch (key) {
    case GLUT_KEY_UP:    specialKeysHeld[KEY_UP] = 0; break;
    case GLUT_KEY_DOWN:  specialKeysHeld[KEY_DOWN] = 0; break;
    case GLUT_KEY_LEFT:  specialKeysHeld[KEY_LEFT] = 0; break;
    case GLUT_KEY_RIGHT: specialKeysHeld[KEY_RIGHT] = 0; break;
    default:                                              break;
    }
}

void mouse(int button, int state, int x, int y)
{
    float gy;
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;
    gy = WindowHeight - (float)y;

    switch (gameState) {
    case MENU:
        /* Botao Start */
        if (x > WindowWidth / 2.0f - 80 && x < WindowWidth / 2.0f + 80 &&
            gy > WindowHeight / 2.0f - 60 && gy < WindowHeight / 2.0f - 25) {
            resetGame();
            gameState = PLAYING;
        }
        /* Botao Exit */
        if (x > WindowWidth / 2.0f - 80 && x < WindowWidth / 2.0f + 80 &&
            gy > WindowHeight / 2.0f - 110 && gy < WindowHeight / 2.0f - 75) {
            exit(0);
        }
        break;
    default:
        break;
    }
}

void timer(int value)
{
    (void)value;
    switch (gameState) {
    case PLAYING:
        updatePlayer();
        updateBullets();
        updateTargets();
        checkCollisions();
        if (rand() % SPAWN_CHANCE == 0)
            spawnTarget();
        break;
    default:
        break;
    }
    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, timer, 0);
}

void menuCallback(int option)
{
    switch (option) {
    case 1: resetGame(); gameState = PLAYING; break;
    case 2: exit(0);                          break;
    default:                                  break;
    }
}

void createContextMenu(void)
{
    glutCreateMenu(menuCallback);
    glutAddMenuEntry("Start Game", 1);
    glutAddMenuEntry("Quit", 2);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/* ================================================
   MAIN
   ================================================ */

int main(int argc, char** argv)
{
    srand((unsigned int)time(NULL));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WindowWidth, WindowHeight);
    glutCreateWindow("2D Shooting Game");

    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WindowWidth, 0, WindowHeight);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialKeys);
    glutSpecialUpFunc(specialKeysUp);
    glutTimerFunc(TIMER_MS, timer, 0);

    glutFullScreen();
    createContextMenu();

    resetGame();
    glutMainLoop();
    return 0;
}