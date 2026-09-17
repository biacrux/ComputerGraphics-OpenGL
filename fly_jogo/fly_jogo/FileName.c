#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <gl/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// ---------------------------
// CONSTANTES DO MUNDO
// ---------------------------
#define W      200.0f   // largura do mundo
#define H      150.0f   // altura do mundo

// cobra (jogador)
#define CW       8.0f   // largura da cobra
#define CH       6.0f   // altura da cobra

// obstaculos (canos)
#define NUM_CANOS  3            // numero de canos em jogo
#define CANO_W    12.0f         // largura do cano
#define CANO_GAP  40.0f         // espaco entre cano de cima e de baixo
#define CANO_VEL   1.2f         // velocidade dos canos
#define CANO_DIST 80.0f         // distancia entre canos consecutivos

// pontos para ganhar
#define MAX_PONTOS 10

// ---------------------------
// COBRA (jogador)
// ---------------------------
float cx = 30.0f;           // posicao x (fixa)
float cy = H / 2.0f;        // posicao y
float cvy = 0.0f;           // velocidade vertical
float gravidade = -0.18f;   // gravidade
float flap = 2.0f;          // impulso ao carregar enter (mais lento)

// ---------------------------
// CANOS
// ---------------------------
typedef struct {
    float x;       // posicao horizontal
    float abertura; // posicao y do centro do espaco
} Cano;

Cano canos[NUM_CANOS];

// ---------------------------
// ESTADO DO JOGO
// ---------------------------
int pontos = 0;
int vidas = 3;   // vidas do jogador
int morreu = 0;   // perdeu uma vida, a mostrar overlay
int game_over = 0;
int iniciado = 0;

// ---------------------------
// INICIALIZA OS CANOS
// ---------------------------
void init_canos() {
    int i;
    for (i = 0; i < NUM_CANOS; i++) {
        canos[i].x = W + i * CANO_DIST;
        canos[i].abertura = 30.0f + rand() % (int)(H - CANO_GAP - 40);
    }
}

// ---------------------------
// REPOE O JOGADOR (perde 1 vida)
// ---------------------------
void reset_posicao() {
    cy = H / 2.0f;
    cvy = 0.0f;
    morreu = 0;
    iniciado = 0;
    init_canos();
}

// ---------------------------
// REINICIA O JOGO TODO
// ---------------------------
void reset_jogo() {
    cy = H / 2.0f;
    cvy = 0.0f;
    pontos = 0;
    vidas = 3;
    morreu = 0;
    game_over = 0;
    iniciado = 0;
    init_canos();
}

// ---------------------------
// COLISAO AABB
// ---------------------------
int aabb(float ax, float ay, float aw, float ah,
    float bx, float by, float bw, float bh) {
    return ax + aw > bx && ax < bx + bw &&
        ay + ah > by && ay < by + bh;
}

// ---------------------------
// TEXTO
// ---------------------------
void drawText(float x, float y, char* s) {
    glRasterPos2f(x, y);
    while (*s) glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *s++);
}

// ---------------------------
// DESENHA O JOGADOR
// quadrado simples azul
// ---------------------------
void draw_cobra() {
    glColor3f(0.2f, 0.4f, 1.0f);
    glRectf(cx, cy, cx + CW, cy + CH);
}

// ---------------------------
// DESENHA UM CANO
// dois rectangulos simples cinzentos
// ---------------------------
void draw_cano(Cano c) {
    float topo_y = c.abertura + CANO_GAP / 2.0f;
    float baixo_y = c.abertura - CANO_GAP / 2.0f;

    glColor3f(0.5f, 0.5f, 0.5f);

    /* cano de baixo */
    glRectf(c.x, 0, c.x + CANO_W, baixo_y);

    /* cano de cima */
    glRectf(c.x, topo_y, c.x + CANO_W, H);
}

// ---------------------------
// DISPLAY
// ---------------------------
void display() {
    int i;
    char buf[32];

    glClear(GL_COLOR_BUFFER_BIT);

    /* canos */
    for (i = 0; i < NUM_CANOS; i++)
        draw_cano(canos[i]);

    /* cobra */
    draw_cobra();

    /* HUD - pontos */
    glColor3f(1.0f, 1.0f, 1.0f);
    sprintf(buf, "Pontos: %d", pontos);
    drawText(4, H - 12, buf);
    drawText(W / 2 - 50, H - 12, "Enter: voar   R: reiniciar");

    /* HUD - vidas como quadrados vermelhos */
    drawText(4, H - 22, "Vidas:");
    for (i = 0; i < vidas; i++) {
        glColor3f(1.0f, 0.1f, 0.1f);
        glRectf(34 + i * 10, H - 24, 42 + i * 10, H - 16);
    }

    /* overlay: inicio */
    if (!iniciado && !game_over && !morreu) {
        glColor3f(0.0f, 0.0f, 0.0f);
        glRectf(W / 2 - 70, H / 2 - 20, W / 2 + 70, H / 2 + 20);
        glColor3f(0.1f, 0.9f, 0.2f);
        drawText(W / 2 - 45, H / 2 + 8, "COBRA VOADORA");
        glColor3f(1.0f, 1.0f, 0.3f);
        drawText(W / 2 - 55, H / 2 - 8, "Prima Enter para comecar!");
    }

    /* overlay: morreste - ainda tem vidas */
    if (morreu && !game_over) {
        glColor3f(0.0f, 0.0f, 0.0f);
        glRectf(W / 2 - 70, H / 2 - 20, W / 2 + 70, H / 2 + 20);
        glColor3f(1.0f, 0.3f, 0.3f);
        drawText(W / 2 - 38, H / 2 + 8, "MORRESTE!");
        glColor3f(1.0f, 1.0f, 1.0f);
        sprintf(buf, "Vidas: %d", vidas);
        drawText(W / 2 - 20, H / 2 - 4, buf);
        glColor3f(0.9f, 0.9f, 0.9f);
        drawText(W / 2 - 55, H / 2 - 14, "Prima Enter para continuar");
    }

    /* overlay: game over */
    if (game_over) {
        glColor3f(0.0f, 0.0f, 0.0f);
        glRectf(W / 2 - 65, H / 2 - 20, W / 2 + 65, H / 2 + 20);
        glColor3f(1.0f, 0.2f, 0.2f);
        drawText(W / 2 - 38, H / 2 + 8, "GAME OVER");
        glColor3f(1.0f, 1.0f, 0.3f);
        sprintf(buf, "Pontos: %d", pontos);
        drawText(W / 2 - 25, H / 2 - 4, buf);
        glColor3f(0.9f, 0.9f, 0.9f);
        drawText(W / 2 - 45, H / 2 - 14, "Prima R para reiniciar");
    }

    glutSwapBuffers();
}

// ---------------------------
// RESHAPE
// ---------------------------
void reshape(GLsizei w, GLsizei h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, W, 0, H, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// ---------------------------
// UPDATE (timer a cada 16ms)
// ---------------------------
void update(int val) {
    int i;

    if (iniciado && !game_over && !morreu) {

        /* gravidade */
        cvy += gravidade;
        cy += cvy;

        /* atualiza canos */
        for (i = 0; i < NUM_CANOS; i++) {
            canos[i].x -= CANO_VEL;

            /* quando sai do ecra volta ao fim e gera nova abertura */
            if (canos[i].x + CANO_W < 0) {
                /* encontra o cano mais a direita */
                float max_x = canos[0].x;
                int j;
                for (j = 1; j < NUM_CANOS; j++)
                    if (canos[j].x > max_x) max_x = canos[j].x;

                canos[i].x = max_x + CANO_DIST;
                canos[i].abertura = 30.0f + rand() % (int)(H - CANO_GAP - 40);
                pontos++;
            }
        }

        /* colisao com chao e teto */
        if (cy <= 0.0f || cy + CH >= H) {
            vidas--;
            if (vidas <= 0) { vidas = 0; game_over = 1; }
            else { morreu = 1; }
        }

        /* colisao com canos */
        for (i = 0; i < NUM_CANOS; i++) {
            float topo_y = canos[i].abertura + CANO_GAP / 2.0f;
            float baixo_y = canos[i].abertura - CANO_GAP / 2.0f;

            /* cano de baixo */
            if (aabb(cx, cy, CW, CH, canos[i].x, 0, CANO_W, baixo_y)) {
                vidas--;
                if (vidas <= 0) { vidas = 0; game_over = 1; }
                else { morreu = 1; }
            }

            /* cano de cima */
            if (aabb(cx, cy, CW, CH, canos[i].x, topo_y, CANO_W, H - topo_y)) {
                vidas--;
                if (vidas <= 0) { vidas = 0; game_over = 1; }
                else { morreu = 1; }
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// ---------------------------
// TECLADO NORMAL (Enter / R / ESC)
// ---------------------------
void teclado(unsigned char key, int x, int y) {
    switch (key) {
    case '\r': case '\n': /* Enter */
        if (morreu) {
            reset_posicao();   /* continua com as vidas que tem */
            iniciado = 1;
        }
        else if (!game_over) {
            if (!iniciado) iniciado = 1;
            cvy = flap;        /* voa para cima */
        }
        break;
    case 'r': case 'R':
        reset_jogo();
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
    srand((unsigned int)time(NULL));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(200, 100);
    glutCreateWindow("Cobra Voadora - Computacao Grafica");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    reset_jogo();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(teclado);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}