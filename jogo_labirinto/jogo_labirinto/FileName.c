#define _CRT_SECURE_NO_WARNINGS
#include <GL/glut.h>
#include <stdio.h>

/*
 * LABIRINTO 2 JOGADORES
 * Jogador 1 (AZUL)  - setas do teclado
 * Jogador 2 (VERMELHO) - WASD
 * Cada um tem de chegar a sua saida
 * Conceitos dos slides: Display Lists, glutKeyboardFunc,
 * glutSpecialFunc, glutReshapeFunc, glRectf, glOrtho
 */

 /* ======== CONSTANTES ======== */
#define TILE  40
#define COLS  20
#define ROWS  15
#define WIN_W (COLS * TILE)
#define WIN_H (ROWS * TILE + 60)

/* tipos de tile */
#define VAZIO  0
#define PAREDE 1
#define META1  2   /* saida jogador 1 azul  */
#define META2  3   /* saida jogador 2 vermelho */

/* ======== ESTADO ======== */
static int mapa[ROWS][COLS];
static int nivel = 0;
static int ganhou1 = 0, ganhou2 = 0;

/* posicoes dos jogadores (em tiles) */
static int j1x, j1y;   /* jogador 1 azul - setas */
static int j2x, j2y;   /* jogador 2 vermelho - wasd */

/* display lists */
static GLuint dl_parede, dl_meta1, dl_meta2, dl_j1, dl_j2;

/* ======== NIVEIS ======== */
/*
 * # = parede
 * . = vazio (corredor)
 * A = inicio jogador 1 (azul)
 * B = inicio jogador 2 (vermelho)
 * 1 = meta jogador 1
 * 2 = meta jogador 2
 */
static const char* niveis[3][ROWS] = {
    /* nivel 1 */
    {
        "####################",
        "#A.................#",
        "#.################.#",
        "#.#................#",
        "#.#.############...#",
        "#.#.#....1.........#",
        "#.#.#.##########.#.#",
        "#.#.#............#.#",
        "#.#.##############.#",
        "#.#................#",
        "#.################.#",
        "#..................#",
        "#.################.#",
        "#B.................2",
        "####################",
    },
    /* nivel 2 */
    {
        "####################",
        "#A..#.......#.....1#",
        "#.#.#.#####.#.###.##",
        "#.#...#...#.#.#....#",
        "#.#####.#.#.#.#.####",
        "#.......#.#...#....#",
        "#########.#####.##.#",
        "#.........#.....##.#",
        "#.#########.#######.",
        "#.#.........#......#",
        "#.#.#########.####.#",
        "#.#...........#....#",
        "#.#############.##.#",
        "#B..............#.2#",
        "####################",
    },
    /* nivel 3 */
    {
        "####################",
        "#A.#...#...........#",
        "#.##.#.#.##########.",
        "#....#.#...........#",
        "#.####.#########.###",
        "#.#....#.........#.#",
        "#.#.####.#######.#.#",
        "#.#.#....#.....#.#.#",
        "#.#.#.####.###.#.#.#",
        "#.#.#.#....#.#.#.#.#",
        "#.#.#.#.####.#.#.#.#",
        "#...#.#......#...#.#",
        "#.###.########.###.#",
        "#B....#........#..2#",
        "####################",
    },
};

/* ======== PARSE NIVEL ======== */
static void parse_nivel(int n) {
    int r, c;
    ganhou1 = 0;
    ganhou2 = 0;

    for (r = 0; r < ROWS; r++) {
        for (c = 0; c < COLS; c++) {
            char ch = niveis[n][r][c];
            switch (ch) {
            case '#': mapa[r][c] = PAREDE; break;
            case '1': mapa[r][c] = META1;  break;
            case '2': mapa[r][c] = META2;  break;
            case 'A':
                j1x = c; j1y = r;
                mapa[r][c] = VAZIO;
                break;
            case 'B':
                j2x = c; j2y = r;
                mapa[r][c] = VAZIO;
                break;
            default: mapa[r][c] = VAZIO; break;
            }
        }
    }
}

/* ======== DISPLAY LISTS ======== */
static void build_lists(void) {
    float h = (float)TILE - 1.0f;

    /* parede - cinzento escuro */
    dl_parede = glGenLists(1);
    glNewList(dl_parede, GL_COMPILE);
    glColor3f(0.3f, 0.3f, 0.4f);
    glRectf(0, 0, h, h);
    glColor3f(0.5f, 0.5f, 0.6f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(0, 0); glVertex2f(h, 0); glVertex2f(h, h); glVertex2f(0, h);
    glEnd();
    glEndList();

    /* meta jogador 1 - azul claro */
    dl_meta1 = glGenLists(1);
    glNewList(dl_meta1, GL_COMPILE);
    glColor3f(0.0f, 0.5f, 1.0f);
    glRectf(2, 2, h - 2, h - 2);
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(2, 2); glVertex2f(h - 2, 2); glVertex2f(h - 2, h - 2); glVertex2f(2, h - 2);
    glEnd();
    /* estrela simples */
    glBegin(GL_LINES);
    glVertex2f(h / 2, 5); glVertex2f(h / 2, h - 5);
    glVertex2f(5, h / 2); glVertex2f(h - 5, h / 2);
    glEnd();
    glEndList();

    /* meta jogador 2 - vermelho claro */
    dl_meta2 = glGenLists(1);
    glNewList(dl_meta2, GL_COMPILE);
    glColor3f(1.0f, 0.2f, 0.2f);
    glRectf(2, 2, h - 2, h - 2);
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(2, 2); glVertex2f(h - 2, 2); glVertex2f(h - 2, h - 2); glVertex2f(2, h - 2);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(h / 2, 5); glVertex2f(h / 2, h - 5);
    glVertex2f(5, h / 2); glVertex2f(h - 5, h / 2);
    glEnd();
    glEndList();

    /* jogador 1 - quadrado azul */
    dl_j1 = glGenLists(1);
    glNewList(dl_j1, GL_COMPILE);
    glColor3f(0.1f, 0.5f, 1.0f);
    glRectf(3, 3, h - 3, h - 3);
    glColor3f(0.7f, 0.9f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(3, 3); glVertex2f(h - 3, 3); glVertex2f(h - 3, h - 3); glVertex2f(3, h - 3);
    glEnd();
    glEndList();

    /* jogador 2 - quadrado vermelho */
    dl_j2 = glGenLists(1);
    glNewList(dl_j2, GL_COMPILE);
    glColor3f(1.0f, 0.1f, 0.1f);
    glRectf(3, 3, h - 3, h - 3);
    glColor3f(1.0f, 0.7f, 0.7f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(3, 3); glVertex2f(h - 3, 3); glVertex2f(h - 3, h - 3); glVertex2f(3, h - 3);
    glEnd();
    glEndList();
}

/* ======== CONVERSAO GRID -> PIXELS ======== */
static float gx(int col) { return (float)(col * TILE); }
static float gy(int row) { return (float)((ROWS - 1 - row) * TILE) + 60.0f; }

/* ======== DESENHO ======== */
static void draw_string(float x, float y, const char* s) {
    glRasterPos2f(x, y);
    while (*s) { glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *s++); }
}

static void display(void) {
    int r, c;
    char buf[64];

    glClear(GL_COLOR_BUFFER_BIT);

    /* fundo do labirinto */
    glColor3f(0.12f, 0.12f, 0.18f);
    glRectf(0, 60, WIN_W, WIN_H);

    /* desenha mapa usando display lists e glTranslatef */
    for (r = 0; r < ROWS; r++) {
        for (c = 0; c < COLS; c++) {
            if (mapa[r][c] == VAZIO) continue;
            glPushMatrix();
            glTranslatef(gx(c), gy(r), 0);
            switch (mapa[r][c]) {
            case PAREDE: glCallList(dl_parede); break;
            case META1:  glCallList(dl_meta1);  break;
            case META2:  glCallList(dl_meta2);  break;
            }
            glPopMatrix();
        }
    }

    /* jogador 2 (vermelho) */
    glPushMatrix();
    glTranslatef(gx(j2x), gy(j2y), 0);
    glCallList(dl_j2);
    glPopMatrix();

    /* jogador 1 (azul) */
    glPushMatrix();
    glTranslatef(gx(j1x), gy(j1y), 0);
    glCallList(dl_j1);
    glPopMatrix();

    /* HUD */
    glColor3f(0.05f, 0.05f, 0.1f);
    glRectf(0, 0, WIN_W, 58);

    sprintf(buf, "LABIRINTO 2P - Nivel %d/3", nivel + 1);
    glColor3f(1, 1, 1);
    draw_string(10, 40, buf);

    glColor3f(0.3f, 0.6f, 1.0f);
    draw_string(10, 22, "J1 AZUL [Setas]");
    glColor3f(ganhou1 ? 0.2f : 0.7f, ganhou1 ? 1.0f : 0.7f, ganhou1 ? 0.2f : 0.7f);
    draw_string(130, 22, ganhou1 ? "CHEGOU!" : "...");

    glColor3f(1.0f, 0.3f, 0.3f);
    draw_string(220, 22, "J2 VERMELHO [WASD]");
    glColor3f(ganhou2 ? 0.2f : 0.7f, ganhou2 ? 1.0f : 0.7f, ganhou2 ? 0.2f : 0.7f);
    draw_string(400, 22, ganhou2 ? "CHEGOU!" : "...");

    glColor3f(0.6f, 0.6f, 0.6f);
    draw_string(10, 6, "R: reiniciar   N: proximo nivel   ESC: sair");

    /* mensagem vitoria */
    if (ganhou1 && ganhou2) {
        glColor3f(0, 0, 0);
        glRectf(WIN_W / 2 - 180, WIN_H / 2 - 25, WIN_W / 2 + 180, WIN_H / 2 + 25);
        glColor3f(1.0f, 0.9f, 0.0f);
        if (nivel < 2)
            draw_string(WIN_W / 2 - 160, WIN_H / 2, "OS DOIS CHEGARAM! Prima N para continuar.");
        else
            draw_string(WIN_W / 2 - 130, WIN_H / 2, "JOGO COMPLETO! Parabens aos dois!");
    }
    else if (ganhou1 && !ganhou2) {
        glColor3f(0, 0, 0);
        glRectf(WIN_W / 2 - 150, WIN_H / 2 - 20, WIN_W / 2 + 150, WIN_H / 2 + 20);
        glColor3f(0.3f, 0.7f, 1.0f);
        draw_string(WIN_W / 2 - 120, WIN_H / 2, "J1 chegou! Falta o J2 vermelho...");
    }
    else if (!ganhou1 && ganhou2) {
        glColor3f(0, 0, 0);
        glRectf(WIN_W / 2 - 150, WIN_H / 2 - 20, WIN_W / 2 + 150, WIN_H / 2 + 20);
        glColor3f(1.0f, 0.4f, 0.4f);
        draw_string(WIN_W / 2 - 120, WIN_H / 2, "J2 chegou! Falta o J1 azul...");
    }

    glFlush();
}

/* ======== RESHAPE ======== */
static void reshape(GLsizei w, GLsizei h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIN_W, 0, WIN_H, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/* mover jogador 1 */
static void mover_j1(int dx, int dy) {
    int nx = j1x + dx;
    int ny = j1y + dy;
    if (nx < 0 || nx >= COLS || ny < 0 || ny >= ROWS) return;
    if (mapa[ny][nx] == PAREDE) return;
    if (nx == j2x && ny == j2y) return;
    j1x = nx;
    j1y = ny;
    if (mapa[ny][nx] == META1) ganhou1 = 1;
    glutPostRedisplay();
}

/* mover jogador 2 */
static void mover_j2(int dx, int dy) {
    int nx = j2x + dx;
    int ny = j2y + dy;
    if (nx < 0 || nx >= COLS || ny < 0 || ny >= ROWS) return;
    if (mapa[ny][nx] == PAREDE) return;
    if (nx == j1x && ny == j1y) return;
    j2x = nx;
    j2y = ny;
    if (mapa[ny][nx] == META2) ganhou2 = 1;
    glutPostRedisplay();
}

/* ======== INPUT ======== */
/* jogador 1 - setas especiais (nao-ASCII) */
static void special_keys(int key, int x, int y) {
    (void)x; (void)y;
    if (ganhou1) return;
    switch (key) {
    case GLUT_KEY_LEFT:  mover_j1(-1, 0); break;
    case GLUT_KEY_RIGHT: mover_j1(+1, 0); break;
    case GLUT_KEY_UP:    mover_j1(0, -1); break;
    case GLUT_KEY_DOWN:  mover_j1(0, +1); break;
    }
}

/* jogador 2 - WASD (ASCII) */
static void keyboard(unsigned char key, int x, int y) {
    (void)x; (void)y;
    switch (key) {
    case 'a': case 'A': if (!ganhou2) mover_j2(-1, 0); break;
    case 'd': case 'D': if (!ganhou2) mover_j2(+1, 0); break;
    case 'w': case 'W': if (!ganhou2) mover_j2(0, -1); break;
    case 's': case 'S': if (!ganhou2) mover_j2(0, +1); break;
    case 'r': case 'R':
        parse_nivel(nivel);
        glutPostRedisplay();
        break;
    case 'n': case 'N':
        if (ganhou1 && ganhou2 && nivel < 2) {
            nivel++;
            parse_nivel(nivel);
            glutPostRedisplay();
        }
        break;
    case 27: exit(0); break;
    }
}

/* ======== MAIN ======== */
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WIN_W, WIN_H);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Labirinto 2 Jogadores - Computacao Grafica");

    glClearColor(0.05f, 0.05f, 0.08f, 1.0f);

    parse_nivel(nivel);
    build_lists();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(special_keys);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}