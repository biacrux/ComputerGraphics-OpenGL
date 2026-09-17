#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <gl/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

/* tamanho de cada quadrado do grid */
#define TAM 20
#define COLS 25
#define ROWS 20

/* direcoes */
#define CIMA   0
#define BAIXO  1
#define ESQ    2
#define DIR    3

/* posicao de cada parte da cobra */
int cobra_x[500];
int cobra_y[500];
int tamanho = 3;

/* posicao da comida */
int comida_x, comida_y;

/* estado do jogo */
int dir = DIR;
int nova_dir = DIR;
int pontos = 0;
int vidas = 3;
int game_over = 0;
int morreu = 0;
int iniciado = 0;

/* gera comida em posicao aleatoria */
void nova_comida() {
    int i, livre;
    do {
        comida_x = rand() % COLS;
        comida_y = rand() % ROWS;
        livre = 1;
        for (i = 0; i < tamanho; i++) {
            if (cobra_x[i] == comida_x && cobra_y[i] == comida_y) {
                livre = 0;
                break;
            }
        }
    } while (!livre);
}

/* coloca a cobra no inicio */
void reset_cobra() {
    int i;
    tamanho = 3;
    dir = DIR;
    nova_dir = DIR;
    morreu = 0;
    iniciado = 0;
    for (i = 0; i < tamanho; i++) {
        cobra_x[i] = COLS / 2 - i;
        cobra_y[i] = ROWS / 2;
    }
    nova_comida();
}

/* reinicia o jogo todo */
void reset_jogo() {
    pontos = 0;
    vidas = 3;
    game_over = 0;
    reset_cobra();
}

/* desenha texto na posicao x,y */
void texto(float x, float y, char* s) {
    glRasterPos2f(x, y);
    while (*s)
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *s++);
}

/* funcao de desenho */
void display() {
    int i, estado;
    char buf[50];

    glClear(GL_COLOR_BUFFER_BIT);

    /* fundo do jogo */
    glColor3f(0.1f, 0.1f, 0.1f);
    glRectf(0, 40, COLS * TAM, ROWS * TAM + 40);

    /* comida - quadrado vermelho */
    glColor3f(1.0f, 0.3f, 0.0f);
    glRectf(comida_x * TAM, comida_y * TAM + 40,
        comida_x * TAM + TAM, comida_y * TAM + TAM + 40);

    /* corpo da cobra - quadrados azuis */
    for (i = 1; i < tamanho; i++) {
        glColor3f(0.2f, 0.5f, 1.0f);
        glRectf(cobra_x[i] * TAM + 1, cobra_y[i] * TAM + 40 + 1,
            cobra_x[i] * TAM + TAM - 1, cobra_y[i] * TAM + TAM + 40 - 1);
    }

    /* cabeca - quadrado verde */
    glColor3f(0.0f, 0.9f, 0.2f);
    glRectf(cobra_x[0] * TAM + 1, cobra_y[0] * TAM + 40 + 1,
        cobra_x[0] * TAM + TAM - 1, cobra_y[0] * TAM + TAM + 40 - 1);

    /* HUD - barra em baixo */
    glColor3f(0.05f, 0.05f, 0.05f);
    glRectf(0, 0, COLS * TAM, 38);

    glColor3f(1.0f, 1.0f, 1.0f);
    sprintf(buf, "Pontos: %d", pontos);
    texto(10, 24, buf);

    /* vidas como quadrados vermelhos */
    glColor3f(1.0f, 1.0f, 1.0f);
    texto(10, 8, "Vidas:");
    for (i = 0; i < vidas; i++) {
        glColor3f(1.0f, 0.1f, 0.1f);
        glRectf(60 + i * 22, 6, 76 + i * 22, 22);
    }

    glColor3f(0.6f, 0.6f, 0.6f);
    texto(COLS * TAM / 2 - 80, 8, "Setas:mover  P:pausa  R:reiniciar");

    /* overlay consoante o estado do jogo */
    /* 0 = inicio, 1 = a jogar, 2 = morreu, 3 = game over */
    if (game_over)              estado = 3;
    else if (morreu)                 estado = 2;
    else if (!iniciado)              estado = 0;
    else                             estado = 1;

    switch (estado) {
    case 0: /* inicio */
        glColor3f(0.0f, 0.0f, 0.0f);
        glRectf(COLS * TAM / 2 - 140, ROWS * TAM / 2, COLS * TAM / 2 + 140, ROWS * TAM / 2 + 60);
        glColor3f(0.2f, 0.9f, 0.2f);
        texto(COLS * TAM / 2 - 75, ROWS * TAM / 2 + 42, "JOGO DA COBRA");
        glColor3f(1.0f, 1.0f, 1.0f);
        texto(COLS * TAM / 2 - 105, ROWS * TAM / 2 + 22, "Come os quadrados laranjas!");
        glColor3f(1.0f, 1.0f, 0.3f);
        texto(COLS * TAM / 2 - 95, ROWS * TAM / 2 + 6, "Prima uma seta para comecar");
        break;
    case 2: /* morreste - perdeu 1 vida */
        glColor3f(0.0f, 0.0f, 0.0f);
        glRectf(COLS * TAM / 2 - 140, ROWS * TAM / 2, COLS * TAM / 2 + 140, ROWS * TAM / 2 + 60);
        glColor3f(1.0f, 0.3f, 0.3f);
        texto(COLS * TAM / 2 - 50, ROWS * TAM / 2 + 42, "MORRESTE!");
        glColor3f(1.0f, 1.0f, 1.0f);
        sprintf(buf, "Vidas restantes: %d", vidas);
        texto(COLS * TAM / 2 - 65, ROWS * TAM / 2 + 22, buf);
        glColor3f(0.8f, 0.8f, 0.8f);
        texto(COLS * TAM / 2 - 95, ROWS * TAM / 2 + 6, "Prima uma seta para continuar");
        break;
    case 3: /* game over - sem vidas */
        glColor3f(0.0f, 0.0f, 0.0f);
        glRectf(COLS * TAM / 2 - 140, ROWS * TAM / 2, COLS * TAM / 2 + 140, ROWS * TAM / 2 + 60);
        glColor3f(1.0f, 0.2f, 0.2f);
        texto(COLS * TAM / 2 - 45, ROWS * TAM / 2 + 42, "GAME OVER");
        glColor3f(1.0f, 1.0f, 0.3f);
        sprintf(buf, "Pontos: %d", pontos);
        texto(COLS * TAM / 2 - 35, ROWS * TAM / 2 + 22, buf);
        glColor3f(0.8f, 0.8f, 0.8f);
        texto(COLS * TAM / 2 - 80, ROWS * TAM / 2 + 6, "Prima R para jogar novamente");
        break;
    default: /* estado 1 - a jogar, nao mostra overlay */
        break;
    }

    glFlush();
}

/* reshape - ajusta a projecao quando a janela muda de tamanho */
void reshape(GLsizei w, GLsizei h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, COLS * TAM, 0, ROWS * TAM + 40, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/* move a cobra - chamada pelo timer */
void mover(int val) {
    int i;
    int nx, ny;

    if (!game_over && !morreu && iniciado) {

        dir = nova_dir;

        /* nova posicao da cabeca */
        nx = cobra_x[0];
        ny = cobra_y[0];
        switch (dir) {
        case DIR:   nx++; break;
        case ESQ:   nx--; break;
        case CIMA:  ny++; break;
        case BAIXO: ny--; break;
        }

        /* bateu na parede? */
        if (nx < 0 || nx >= COLS || ny < 0 || ny >= ROWS) {
            vidas--;
            if (vidas <= 0) {
                vidas = 0;
                game_over = 1;
            }
            else {
                morreu = 1;
            }
            glutPostRedisplay();
            glutTimerFunc(150, mover, 0);
            return;
        }

        /* bateu no proprio corpo? */
        for (i = 1; i < tamanho; i++) {
            if (nx == cobra_x[i] && ny == cobra_y[i]) {
                vidas--;
                if (vidas <= 0) {
                    vidas = 0;
                    game_over = 1;
                }
                else {
                    morreu = 1;
                }
                glutPostRedisplay();
                glutTimerFunc(150, mover, 0);
                return;
            }
        }

        /* comeu a comida? */
        if (nx == comida_x && ny == comida_y) {
            pontos += 10;
            tamanho++;
            nova_comida();
        }

        /* move o corpo */
        for (i = tamanho - 1; i > 0; i--) {
            cobra_x[i] = cobra_x[i - 1];
            cobra_y[i] = cobra_y[i - 1];
        }
        cobra_x[0] = nx;
        cobra_y[0] = ny;
    }

    glutPostRedisplay();
    glutTimerFunc(150, mover, 0);
}

/* teclas especiais (setas) */
void teclas_especiais(int key, int x, int y) {
    if (game_over) return;

    /* continua apos perder uma vida */
    if (morreu) {
        reset_cobra();
        iniciado = 1;
        glutPostRedisplay();
        return;
    }

    if (!iniciado) iniciado = 1;

    switch (key) {
    case GLUT_KEY_UP:    if (dir != BAIXO) nova_dir = CIMA;  break;
    case GLUT_KEY_DOWN:  if (dir != CIMA)  nova_dir = BAIXO; break;
    case GLUT_KEY_LEFT:  if (dir != DIR)   nova_dir = ESQ;   break;
    case GLUT_KEY_RIGHT: if (dir != ESQ)   nova_dir = DIR;   break;
    }
}

/* teclas normais */
void teclado(unsigned char key, int x, int y) {
    switch (key) {
    case 'r': case 'R': reset_jogo();  break;
    case 27:             exit(0);       break;
    }
}

/* programa principal */
int main(int argc, char** argv) {
    srand(time(NULL));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(COLS * TAM, ROWS * TAM + 40);
    glutInitWindowPosition(200, 100);
    glutCreateWindow("Jogo da Cobra");

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    reset_jogo();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(teclas_especiais);
    glutKeyboardFunc(teclado);
    glutTimerFunc(150, mover, 0);

    glutMainLoop();
    return 0;
}