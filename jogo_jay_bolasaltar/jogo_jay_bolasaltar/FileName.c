#define _CRT_SECURE_NO_WARNINGS

#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

// ---------------------------
// CONSTANTES DO "MUNDO"
// ---------------------------
#define W 200.0f
#define H 120.0f
#define PS 6.0f

// ---------------------------
// VARIÁVEIS DO JOGADOR
// ---------------------------
float px = 10, py = 20;
float pvy = 0;
int onGround = 0;
int leftK = 0, rightK = 0;
int alive = 1;
int won = 0;
int level = 1;
int vidas = 3;       // vidas do jogador
int game_over = 0;   // sem mais vidas

// ---------------------------
// PLATAFORMAS: x, y, largura, altura, deadly
// ---------------------------
float plats[3][11][5] = {
    // NÍVEL 1
    {{0,0,200,8,0},{20,20,30,4,0},{70,35,30,4,0},{130,50,30,4,0},
     {160,28,40,4,0},{55,8,15,5,1},{115,8,15,5,1}},
     // NÍVEL 2
     {{0,0,200,8,0},{15,18,25,4,0},{55,32,25,4,0},{95,46,25,4,0},
      {140,62,25,4,0},{155,82,45,4,0},{40,8,12,5,1},{80,8,12,5,1},
      {120,8,12,5,1},{80,38,18,4,1}},
      // NÍVEL 3
      {{0,0,50,8,0},{70,0,60,8,0},{150,0,50,8,0},{15,22,25,4,0},
       {70,38,25,4,0},{115,52,25,4,0},{150,68,45,4,0},{150,100,45,4,0},
       {50,0,20,8,1},{130,0,20,8,1},{92,28,18,4,1}}
};

int nPlats[3] = { 7, 10, 11 };

float goals[3][2] = { {178,36},{178,92},{178,108} };
float spawns[3][2] = { {8,20},{8,20},{8,20} };

// ---------------------------
// COLISÃO AABB
// ---------------------------
int aabb(float ax, float ay, float bx, float by, float bw, float bh) {
    return ax + PS > bx && ax < bx + bw && ay + PS > by && ay < by + bh;
}

// ---------------------------
// PERDE UMA VIDA
// ---------------------------
void perder_vida(void) {
    vidas--;
    if (vidas <= 0) {
        vidas = 0;
        game_over = 1;
    }
    else {
        // volta ao spawn do nivel atual
        px = spawns[level - 1][0];
        py = spawns[level - 1][1];
        pvy = 0;
        alive = 1;
        onGround = 0;
    }
}

// ---------------------------
// SPAWN DO JOGADOR
// ---------------------------
void spawn(void) {
    px = spawns[level - 1][0];
    py = spawns[level - 1][1];
    pvy = 0;
    alive = 1;
    onGround = 0;
}

// ---------------------------
// FISICA
// ---------------------------
void physics(void) {
    if (!alive || won || game_over) return;

    float prevX = px;
    float prevY = py;

    if (leftK)  px -= 1.4f;
    if (rightK) px += 1.4f;

    pvy -= 0.25f;
    py += pvy;

    onGround = 0;

    for (int i = 0; i < nPlats[level - 1]; i++) {
        float* p = plats[level - 1][i];

        if (aabb(px, py, p[0], p[1], p[2], p[3])) {

            // plataforma mortal - perde uma vida
            if (p[4]) {
                alive = 0;
                perder_vida();
                return;
            }

            // determina o lado da colisao:
            // 1=cima, 2=baixo, 3=lateral esq, 4=lateral dir
            int lado;
            if (prevY >= p[1] + p[3] && pvy <= 0) lado = 1;
            else if (prevY + PS <= p[1] && pvy > 0) lado = 2;
            else if (prevX + PS <= p[0])                lado = 3;
            else                                        lado = 4;

            switch (lado) {
            case 1: /* pousa por cima */
                py = p[1] + p[3];
                pvy = 0;
                onGround = 1;
                break;
            case 2: /* bate no teto */
                py = p[1] - PS;
                pvy = 0;
                break;
            case 3: /* lateral esquerda */
                px = p[0] - PS;
                break;
            case 4: /* lateral direita */
                px = p[0] + p[2];
                break;
            }
        }
    }

    if (px < 0) px = 0;
    if (px + PS > W) px = W - PS;

    // caiu do mapa - perde uma vida
    if (py + PS < 0) {
        alive = 0;
        perder_vida();
        return;
    }

    // chegou ao objetivo
    if (aabb(px, py, goals[level - 1][0] - 5, goals[level - 1][1] - 5, 10, 10)) {
        if (level < 3) { level++; spawn(); }
        else won = 1;
    }

    glutPostRedisplay();
}

// ---------------------------
// TIMER
// ---------------------------
void timer(int v) {
    physics();
    glutTimerFunc(16, timer, v + 1);
}

// ---------------------------
// TEXTO
// ---------------------------
void drawText(float x, float y, char* s) {
    glRasterPos2f(x, y);
    while (*s) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *s++);
}

// ---------------------------
// DESENHO
// ---------------------------
void display(void) {
    int i, j;
    char buf[32];

    glClear(GL_COLOR_BUFFER_BIT);

    // plataformas
    for (i = 0; i < nPlats[level - 1]; i++) {
        float* p = plats[level - 1][i];
        glColor3f(p[4] ? 0.9f : 0.1f, p[4] ? 0.1f : 0.8f, 0.1f);
        glRectf(p[0], p[1], p[0] + p[2], p[1] + p[3]);
    }

    // objetivo
    float gx = goals[level - 1][0];
    float gy = goals[level - 1][1];
    for (int r = 5; r >= 1; r--) {
        float t = r / 5.0f;
        glColor3f(t, t, t);
        glBegin(GL_LINE_LOOP);
        for (j = 0; j < 24; j++) {
            float a = j * 6.2832f / 24;
            glVertex2f(gx + cosf(a) * r * 1.5f, gy + sinf(a) * r * 1.5f);
        }
        glEnd();
    }

    // jogador
    if (alive) {
        glColor3f(0.2f, 0.5f, 1.0f);
        glRectf(px, py, px + PS, py + PS);
    }

    // HUD - nivel
    glColor3f(1, 1, 1);
    sprintf_s(buf, sizeof(buf), "Nivel: %d", level);
    drawText(4, H - 10, buf);

    // HUD - vidas como quadrados vermelhos
    drawText(4, H - 20, "Vidas:");
    for (i = 0; i < vidas; i++) {
        glColor3f(1.0f, 0.1f, 0.1f);
        glRectf(34 + i * 10, H - 22, 42 + i * 10, H - 14);
    }

    // controlos
    glColor3f(1, 1, 1);
    drawText(70, H - 10, "Setas:mover  Cima:saltar  R:restart");

    // overlay: morreu mas ainda tem vidas
    if (!alive && !game_over) {
        glColor3f(1.0f, 0.3f, 0.3f);
        drawText(W / 2 - 30, H / 2 + 6, "MORRESTE!");
        sprintf_s(buf, sizeof(buf), "Vidas: %d  Prima R", vidas);
        drawText(W / 2 - 30, H / 2 - 4, buf);
    }

    // overlay: game over
    if (game_over) {
        glColor3f(1.0f, 0.1f, 0.1f);
        drawText(W / 2 - 25, H / 2 + 6, "GAME OVER");
        drawText(W / 2 - 30, H / 2 - 4, "Prima R para recomecar");
    }

    // overlay: ganhou
    if (won) {
        glColor3f(0.3f, 1.0f, 0.3f);
        drawText(W / 2 - 30, H / 2, "PARABENS! Prima R");
    }

    glutSwapBuffers();
}

// ---------------------------
// RESHAPE
// ---------------------------
void reshape(int w, int h) {
    glViewport(0, 0, w, h ? h : 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, W, 0, H);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// ---------------------------
// TECLAS ESPECIAIS
// ---------------------------
void specialDown(int k, int x, int y) {
    switch (k) {
    case GLUT_KEY_LEFT:  leftK = 1;  break;
    case GLUT_KEY_RIGHT: rightK = 1; break;
    case GLUT_KEY_UP:
        if (onGround) { pvy = 5; onGround = 0; }
        break;
    }
}

void specialUp(int k, int x, int y) {
    switch (k) {
    case GLUT_KEY_LEFT:  leftK = 0;  break;
    case GLUT_KEY_RIGHT: rightK = 0; break;
    }
}

// ---------------------------
// TECLAS NORMAIS
// ---------------------------
void keyboard(unsigned char k, int x, int y) {
    switch (k) {
    case 'r': case 'R':
        level = 1;
        won = 0;
        game_over = 0;
        vidas = 3;
        spawn();
        break;
    case 27:
        exit(0);
        break;
    }
}

// ---------------------------
// MAIN
// ---------------------------
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 480);
    glutCreateWindow("Plataformas - 3 Niveis");

    glClearColor(0.05f, 0.05f, 0.1f, 1);

    spawn();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(specialDown);
    glutSpecialUpFunc(specialUp);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, timer, 0);

    glutMainLoop();
    return 0;
}