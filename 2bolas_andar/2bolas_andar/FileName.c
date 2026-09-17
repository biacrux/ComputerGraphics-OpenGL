#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>

float limX = 0, limY = 0;
float raio = 0.5f;
float velocidade = 0.1f;

// ?? Bola 1 (setas) ??????????????????????????????????????????
float xf1 = -1.0f, yf1 = 0.0f;
float cor1R = 1.0f, cor1G = 0.4f, cor1B = 0.7f; // rosa

// ?? Bola 2 (WASD) ???????????????????????????????????????????
float xf2 = 1.0f, yf2 = 0.0f;
float cor2R = 0.3f, cor2G = 0.8f, cor2B = 1.0f; // azul claro

// ?? Função auxiliar: desenha uma bola ???????????????????????
void desenharBola(float x, float y, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glColor3f(r, g, b);
    glutSolidSphere(raio, 32, 32);
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    desenharBola(xf1, yf1, cor1R, cor1G, cor1B); // bola 1
    desenharBola(xf2, yf2, cor2R, cor2G, cor2B); // bola 2

    glutSwapBuffers();
}

// ?? Setas ? Bola 1 ??????????????????????????????????????????
void SpecialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:    yf1 += velocidade; break;
    case GLUT_KEY_DOWN:  yf1 -= velocidade; break;
    case GLUT_KEY_LEFT:  xf1 -= velocidade; break;
    case GLUT_KEY_RIGHT: xf1 += velocidade; break;
    }
    xf1 = fmaxf(-limX, fminf(xf1, limX)); 
    yf1 = fmaxf(-limY, fminf(yf1, limY));
    glutPostRedisplay();
}

// ?? WASD ? Bola 2 ???????????????????????????????????????????
void NormalKeys(unsigned char key, int x, int y) {
    switch (key) {
    case 'w': case 'W': yf2 += velocidade; break;
    case 's': case 'S': yf2 -= velocidade; break;
    case 'a': case 'A': xf2 -= velocidade; break;
    case 'd': case 'D': xf2 += velocidade; break;
    case 27: exit(0); // ESC sai
    }
    xf2 = fmaxf(-limX, fminf(xf2, limX));
    yf2 = fmaxf(-limY, fminf(yf2, limY));
    glutPostRedisplay();
}

// ?? Menus (afetam ambas) ?????????????????????????????????????
void menuCor(int opcao) {
    float r, g, b;
    switch (opcao) {
    case 1: r = 1.0f; g = 0.0f; b = 0.0f; break; // vermelho
    case 2: r = 0.0f; g = 1.0f; b = 0.0f; break; // verde
    case 3: r = 0.0f; g = 0.0f; b = 1.0f; break; // azul
    case 4: r = 1.0f; g = 0.4f; b = 0.7f; break; // rosa
    default: return;
    }
    // aplica a ambas
    cor1R = cor2R = r;
    cor1G = cor2G = g;
    cor1B = cor2B = b;
    glutPostRedisplay();
}

void menuVelocidade(int opcao) {
    switch (opcao) {
    case 1: velocidade = 0.05f; break; // lenta
    case 2: velocidade = 0.1f;  break; // normal
    case 3: velocidade = 0.3f;  break; // rápida
    }
    glutPostRedisplay();
}

void menuPrincipal(int opcao) {
    switch (opcao) {
    case 1: // resetar posições
        xf1 = -1.0f; yf1 = 0.0f;
        xf2 = 1.0f; yf2 = 0.0f;
        glutPostRedisplay();
        break;
    case 2: exit(0);
    }
}

void criarMenus() {
    int subCor = glutCreateMenu(menuCor);
    glutAddMenuEntry("Vermelho", 1);
    glutAddMenuEntry("Verde", 2);
    glutAddMenuEntry("Azul", 3);
    glutAddMenuEntry("Rosa", 4);

    int subVel = glutCreateMenu(menuVelocidade);
    glutAddMenuEntry("Lenta", 1);
    glutAddMenuEntry("Normal", 2);
    glutAddMenuEntry("Rapida", 3);

    glutCreateMenu(menuPrincipal);
    glutAddSubMenu("Cor", subCor);
    glutAddSubMenu("Velocidade", subVel);
    glutAddMenuEntry("Resetar posicoes", 1);
    glutAddMenuEntry("Sair", 2);

    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void reshape(int width, int height) {
    if (height == 0) height = 1;
    float aspect = (float)width / height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-aspect * 5, aspect * 5, -5, 5, -10, 10);
    limX = aspect * 5 - raio;
    limY = 5.0f - raio;
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 400);
    glutCreateWindow("Duas Bolas");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glutDisplayFunc(display);
    glutSpecialFunc(SpecialKeys);
    glutKeyboardFunc(NormalKeys);  // ? regista WASD
    glutReshapeFunc(reshape);
    criarMenus();
    glutMainLoop();
    return 0;
}