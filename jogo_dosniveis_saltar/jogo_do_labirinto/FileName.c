#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <gl/glut.h>
#include <stdio.h>

/* ======== CONSTANTES ======== */
#define TILE   48
#define COLS   16
#define ROWS   10
#define WIN_W  (COLS * TILE)
#define WIN_H  (ROWS * TILE + 80)

#define VIDAS_INICIAIS 3

/* tiles */
#define VAZIO  0
#define PAREDE 1
#define SAIDA  2
#define TOKEN  3

/* ======== ESTADO GLOBAL ======== */
static int mapa[ROWS][COLS];
static int nivel = 0;
static int ganhou = 0;
static int vidas = VIDAS_INICIAIS;
static int morreu = 0;
static int morreu_timer = 0;
static int game_over = 0;

/* jogador */
static float px, py;
static float vy = 0;
static int   no_chao = 0;

/* posicao de spawn guardada para respawn */
static float spawn_px, spawn_py;

/* display lists */
static GLuint dl_parede, dl_saida, dl_token, dl_jogador;

/* ======== NIVEIS ======== */
static const char* niveis[3][ROWS] = {
    /* nivel 1 */
    {
        "################",
        "#             E#",
        "#          ### #",
        "#         #    #",
        "#    ####      #",
        "#              #",
        "#  ###         #",
        "#P             #",
        "################",
        "                ",
    },
    /* nivel 2 - buracos no chao = morte */
    {
        "################",
        "#E             #",
        "## ##          #",
        "#              #",
        "#      ### ##  #",
        "#   ###        #",
        "#         ###  #",
        "#P             #",
        "###    #########",
        "                ",
    },
    /* nivel 3 */
    {
        "################",
        "#E    #        #",
        "#     #   ###  #",
        "# ### #        #",
        "#     #  ##    #",
        "#  ##          #",
        "#         ##   #",
        "#P      ###    #",
        "################",
        "                ",
    },
};

/* ======== PARSE NIVEL ======== */
static void parse_nivel(int n) {
    int r, c;
    ganhou = 0;
    morreu = 0;
    morreu_timer = 0;
    vy = 0;
    no_chao = 0;

    for (r = 0; r < ROWS; r++) {
        for (c = 0; c < COLS; c++) {
            char ch = niveis[n][r][c];
            if (ch == '#') mapa[r][c] = PAREDE;
            else if (ch == 'E') mapa[r][c] = SAIDA;
            else                mapa[r][c] = VAZIO;

            if (ch == 'P') {
                px = (float)(c * TILE);
                py = (float)((ROWS - 1 - r) * TILE) + 80.0f;
                spawn_px = px;
                spawn_py = py;
                mapa[r][c] = VAZIO;
            }
        }
    }
}

/* ======== RESPAWN ======== */
static void respawn(void) {
    px = spawn_px;
    py = spawn_py;
    vy = 0;
    no_chao = 0;
    morreu = 0;
    morreu_timer = 0;
}

/* ======== CONVERSAO pixel -> grid ======== */
static int pixel_col(float x) { return (int)(x / TILE); }
static int pixel_row(float y) { return ROWS - 1 - (int)((y - 80.0f) / TILE); }

/* ======== COLISAO ======== */
static int tile_solido(int r, int c) {
    if (r < 0 || r >= ROWS || c < 0 || c >= COLS) return 1;
    return (mapa[r][c] == PAREDE);
}

/* verifica se ha chao solido imediatamente abaixo de uma posicao */
static int tem_chao_abaixo(float fx, float fy) {
    int cl = pixel_col(fx + 2);
    int cr = pixel_col(fx + TILE - 3);
    int rb = pixel_row(fy - 1);   /* 1 pixel abaixo dos pes */
    return (tile_solido(rb, cl) || tile_solido(rb, cr));
}

/* ======== BUILD DISPLAY LISTS ======== */
static void build_lists(void) {
    float h = (float)TILE - 2.0f;

    /* --- parede --- */
    dl_parede = glGenLists(1);
    glNewList(dl_parede, GL_COMPILE);
    glColor3f(0.4f, 0.4f, 0.6f);
    glRectf(1, 1, h, h);
    glColor3f(0.2f, 0.2f, 0.35f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(1, 1); glVertex2f(h, 1);
    glVertex2f(h, h); glVertex2f(1, h);
    glEnd();
    glEndList();

    /* --- saida (verde) --- */
    dl_saida = glGenLists(1);
    glNewList(dl_saida, GL_COMPILE);
    glColor3f(0.2f, 0.9f, 0.3f);
    glRectf(4, 4, h - 4, h - 4);
    glColor3f(0.0f, 0.5f, 0.1f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(4, 4);     glVertex2f(h - 4, 4);
    glVertex2f(h - 4, h - 4); glVertex2f(4, h - 4);
    glEnd();
    glEndList();

    /* --- token (amarelo) --- */
    dl_token = glGenLists(1);
    glNewList(dl_token, GL_COMPILE);
    glColor3f(1.0f, 0.85f, 0.0f);
    glRectf(8, 8, h - 8, h - 8);
    glEndList();

    /* --- jogador (vermelho) --- */
    dl_jogador = glGenLists(1);
    glNewList(dl_jogador, GL_COMPILE);
    glColor3f(1.0f, 0.2f, 0.2f);
    glRectf(3, 3, h - 3, h - 3);
    glColor3f(0.6f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(3, 3);     glVertex2f(h - 3, 3);
    glVertex2f(h - 3, h - 3); glVertex2f(3, h - 3);
    glEnd();
    glEndList();
}

/* ======== AUXILIAR: texto ======== */
static void draw_string(float x, float y, const char* s) {
    glRasterPos2f(x, y);
    while (*s) { glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *s++); }
}

/* ======== AUXILIAR: coracao = 1 vida ======== */
static void draw_heart(float x, float y) {
    glColor3f(1.0f, 0.15f, 0.15f);
    glRectf(x, y, x + 14.0f, y + 14.0f);
    glColor3f(0.7f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + 14.0f, y);
    glVertex2f(x + 14.0f, y + 14.0f);
    glVertex2f(x, y + 14.0f);
    glEnd();
}

/* ======== DISPLAY - RenderScene ======== */
static void RenderScene(void) {
    int r, c, i;
    char buf[64];

    glClear(GL_COLOR_BUFFER_BIT);

    /* fundo do nivel */
    glColor3f(0.1f, 0.1f, 0.15f);
    glRectf(0, 80, WIN_W, WIN_H);

    /* mapa */
    for (r = 0; r < ROWS; r++) {
        for (c = 0; c < COLS; c++) {
            if (mapa[r][c] == VAZIO) continue;
            glPushMatrix();
            glTranslatef((float)(c * TILE),
                (float)((ROWS - 1 - r) * TILE) + 80.0f, 0);
            if (mapa[r][c] == PAREDE) glCallList(dl_parede);
            else if (mapa[r][c] == SAIDA)  glCallList(dl_saida);
            else if (mapa[r][c] == TOKEN)  glCallList(dl_token);
            glPopMatrix();
        }
    }

    /* jogador - pisca quando morreu */
    if (!morreu || (morreu_timer / 4) % 2 == 0) {
        glPushMatrix();
        glTranslatef(px, py, 0);
        if (morreu) {
            float h2 = (float)TILE - 2.0f;
            glColor3f(0.9f, 0.9f, 0.0f);
            glRectf(3, 3, h2 - 3, h2 - 3);
        }
        else {
            glCallList(dl_jogador);
        }
        glPopMatrix();
    }

    /* ---- HUD ---- */
    glColor3f(0.05f, 0.05f, 0.1f);
    glRectf(0, 0, WIN_W, 78);

    sprintf(buf, "Nivel %d/3", nivel + 1);
    glColor3f(1.0f, 1.0f, 1.0f);
    draw_string(10, 55, buf);

    draw_string(10, 33, "Vidas:");
    for (i = 0; i < vidas; i++)
        draw_heart(58.0f + i * 20.0f, 28.0f);

    glColor3f(0.7f, 0.7f, 0.7f);
    draw_string(10, 15, "Setas: mover/saltar  R: reiniciar  N: proximo nivel  ESC: sair");

    /* overlay: MORRESTE */
    if (morreu && !game_over) {
        glColor3f(0.0f, 0.0f, 0.0f);
        glRectf(WIN_W / 2 - 165, WIN_H / 2 - 30, WIN_W / 2 + 165, WIN_H / 2 + 30);
        glColor3f(1.0f, 0.3f, 0.3f);
        draw_string(WIN_W / 2 - 90, WIN_H / 2 + 10, "MORRESTE!");
        glColor3f(1.0f, 1.0f, 0.4f);
        sprintf(buf, "Vidas restantes: %d   Prima R para continuar", vidas);
        draw_string(WIN_W / 2 - 155, WIN_H / 2 - 12, buf);
    }

    /* overlay: GAME OVER */
    if (game_over) {
        glColor3f(0.0f, 0.0f, 0.0f);
        glRectf(WIN_W / 2 - 185, WIN_H / 2 - 35, WIN_W / 2 + 185, WIN_H / 2 + 35);
        glColor3f(1.0f, 0.1f, 0.1f);
        draw_string(WIN_W / 2 - 58, WIN_H / 2 + 12, "GAME OVER");
        glColor3f(0.9f, 0.9f, 0.9f);
        draw_string(WIN_W / 2 - 115, WIN_H / 2 - 12, "Prima R para recomecar do nivel 1");
    }

    /* overlay: NIVEL COMPLETO */
    if (ganhou) {
        glColor3f(0.0f, 0.0f, 0.0f);
        glRectf(WIN_W / 2 - 185, WIN_H / 2 - 25, WIN_W / 2 + 185, WIN_H / 2 + 25);
        glColor3f(0.2f, 1.0f, 0.4f);
        if (nivel < 2)
            draw_string(WIN_W / 2 - 148, WIN_H / 2, "NIVEL COMPLETO! Prima N para continuar.");
        else
            draw_string(WIN_W / 2 - 110, WIN_H / 2, "JOGO COMPLETO! Parabens!");
    }

    glFlush();
}

/* ======== RESHAPE - ChangeSize ======== */
static void ChangeSize(GLsizei w, GLsizei h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIN_W, 0, WIN_H, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/* ======== FISICA (glutTimerFunc) ======== */
static int tecla_esq = 0, tecla_dir = 0, tecla_cima = 0;

static void update(int val) {
    int col_esq, col_dir, row_topo, row_base;
    float gravidade = -4.0f;
    float vel_mov = 3.0f;
    float novo_py, novo_px;
    int row_meio;

    /* durante animacao de morte apenas avanca o contador */
    if (morreu) {
        morreu_timer++;
        glutPostRedisplay();
        glutTimerFunc(8, update, 0);
        return;
    }

    if (!ganhou && !game_over) {

        /* movimento horizontal */
        if (tecla_esq) {
            novo_px = px - vel_mov;
            row_meio = pixel_row(py + TILE / 2);
            row_base = pixel_row(py + 2);
            col_esq = pixel_col(novo_px + 2);
            if (!tile_solido(row_meio, col_esq) && !tile_solido(row_base, col_esq))
                px = novo_px;
        }
        if (tecla_dir) {
            novo_px = px + vel_mov;
            row_meio = pixel_row(py + TILE / 2);
            row_base = pixel_row(py + 2);
            col_dir = pixel_col(novo_px + TILE - 3);
            if (!tile_solido(row_meio, col_dir) && !tile_solido(row_base, col_dir))
                px = novo_px;
        }

        /* salto */
        if (tecla_cima && no_chao) {
            vy = 30.0f;
            no_chao = 0;
            tecla_cima = 0;
        }

        /* gravidade */
        vy += gravidade;
        novo_py = py + vy;

        col_esq = pixel_col(px + 2);
        col_dir = pixel_col(px + TILE - 3);

        /* colisao com chao */
        row_base = pixel_row(novo_py + 2);
        if (vy < 0) {
            if (tile_solido(row_base, col_esq) || tile_solido(row_base, col_dir)) {
                /* ha chao - pousa normalmente */
                novo_py = (float)((ROWS - 1 - row_base) * TILE) + 80.0f + TILE;
                vy = 0;
                no_chao = 1;
            }
            else {
                no_chao = 0;

                /* ===== MORTE: caiu no buraco ===== */
                /* deteta quando passa abaixo do nivel do chao do mapa */
                if (novo_py < 80.0f) {
                    vidas--;
                    if (vidas <= 0) {
                        vidas = 0;
                        game_over = 1;
                    }
                    else {
                        morreu = 1;
                        morreu_timer = 0;
                    }
                    /* repoe posicao para nao continuar a cair */
                    px = spawn_px;
                    py = spawn_py;
                    vy = 0;
                    glutPostRedisplay();
                    glutTimerFunc(8, update, 0);
                    return;
                }
            }
        }

        /* colisao com teto */
        row_topo = pixel_row(novo_py + TILE - 2);
        if (vy > 0) {
            if (tile_solido(row_topo, col_esq) || tile_solido(row_topo, col_dir)) {
                novo_py = (float)((ROWS - 1 - row_topo) * TILE) + 80.0f - TILE;
                vy = 0;
            }
        }

        py = novo_py;

        /* verifica saida */
        {
            int rc = pixel_row(py + TILE / 2);
            int cc = pixel_col(px + TILE / 2);
            if (rc >= 0 && rc < ROWS && cc >= 0 && cc < COLS)
                if (mapa[rc][cc] == SAIDA) ganhou = 1;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(8, update, 0);
}

/* ======== TECLAS ESPECIAIS - SpecialKeys ======== */
static void SpecialKeys(int key, int x, int y) {
    (void)x; (void)y;
    if (ganhou || morreu || game_over) return;
    if (key == GLUT_KEY_LEFT)  tecla_esq = 1;
    if (key == GLUT_KEY_RIGHT) tecla_dir = 1;
    if (key == GLUT_KEY_UP)    tecla_cima = 1;
}

static void SpecialKeysUp(int key, int x, int y) {
    (void)x; (void)y;
    if (key == GLUT_KEY_LEFT)  tecla_esq = 0;
    if (key == GLUT_KEY_RIGHT) tecla_dir = 0;
}

/* ======== TECLADO ASCII - HandleKeyboard ======== */
static void HandleKeyboard(unsigned char key, int x, int y) {
    (void)x; (void)y;

    switch (key) {
    case 'r': case 'R':
        if (game_over) {
            nivel = 0;
            vidas = VIDAS_INICIAIS;
            game_over = 0;
            parse_nivel(nivel);
        }
        else if (morreu) {
            respawn();
        }
        else {
            parse_nivel(nivel);
        }
        break;
    case 'n': case 'N':
        if (ganhou && nivel < 2) {
            nivel++;
            parse_nivel(nivel);
        }
        break;
    case 27:
        exit(0);
        break;
    }

    glutPostRedisplay();
}

/* ======== MAIN ======== */
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WIN_W, WIN_H);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Plataformas - Computacao Grafica");

    glClearColor(0.05f, 0.05f, 0.08f, 1.0f);

    vidas = VIDAS_INICIAIS;
    parse_nivel(nivel);
    build_lists();

    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    glutSpecialFunc(SpecialKeys);
    glutSpecialUpFunc(SpecialKeysUp);
    glutKeyboardFunc(HandleKeyboard);
    glutTimerFunc(8, update, 0);

    glutMainLoop();
    return 0;
}