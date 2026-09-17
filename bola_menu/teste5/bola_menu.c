#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>

float xf = 0, yf = 0;
float limX = 0, limY = 0;
float raio = 0.5f;
float velocidade = 0.1f;
float corR = 1.0f, corG = 0.4f, corB = 0.7f; // cor inicial rosa

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(xf, yf, 0.0f);
    glColor3f(corR, corG, corB); // usa a cor atual
    glutSolidSphere(raio, 32, 32);
    glutSwapBuffers();
}

void SpecialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:    yf += velocidade; break;
    case GLUT_KEY_DOWN:  yf -= velocidade; break;
    case GLUT_KEY_LEFT:  xf -= velocidade; break;
    case GLUT_KEY_RIGHT: xf += velocidade; break;
    }
    xf = fmaxf(-limX, fminf(xf, limX));
    yf = fmaxf(-limY, fminf(yf, limY));
    glutPostRedisplay();
}

// ============================================================
// MENUS
// ============================================================

// submenu das cores
void menuCor(int opcao) {
    switch (opcao) {
    case 1: corR = 1.0f; corG = 0.0f; corB = 0.0f; break; // vermelho
    case 2: corR = 0.0f; corG = 1.0f; corB = 0.0f; break; // verde
    case 3: corR = 0.0f; corG = 0.0f; corB = 1.0f; break; // azul
    case 4: corR = 1.0f; corG = 0.4f; corB = 0.7f; break; // rosa
    }
    glutPostRedisplay();
}

// submenu da velocidade
void menuVelocidade(int opcao) {
    switch (opcao) {
    case 1: velocidade = 0.05f; break; // lenta
    case 2: velocidade = 0.1f;  break; // normal
    case 3: velocidade = 0.3f;  break; // rapida
    }
    glutPostRedisplay();
}

// menu principal
void menuPrincipal(int opcao) {
    switch (opcao) {
    case 1: xf = 0; yf = 0; glutPostRedisplay(); break; // resetar posicao
    case 2: exit(0); break; // sair
    }
}

void criarMenus() {
    // cria o submenu das cores
    int subCor = glutCreateMenu(menuCor);
    glutAddMenuEntry("Vermelho", 1);
    glutAddMenuEntry("Verde", 2);
    glutAddMenuEntry("Azul", 3);
    glutAddMenuEntry("Rosa", 4);

    // cria o submenu da velocidade
    int subVel = glutCreateMenu(menuVelocidade);
    glutAddMenuEntry("Lenta", 1);
    glutAddMenuEntry("Normal", 2);
    glutAddMenuEntry("Rapida", 3);

    // cria o menu principal
    glutCreateMenu(menuPrincipal);
    glutAddSubMenu("Cor", subCor); // adiciona submenu das cores
    glutAddSubMenu("Velocidade", subVel); // adiciona submenu da velocidade
    glutAddMenuEntry("Resetar posicao", 1);
    glutAddMenuEntry("Sair", 2);

    // associa o menu ao botao direito do rato
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
    glutCreateWindow("Esferaaaaaaaaaa Animada");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glutDisplayFunc(display);
    glutSpecialFunc(SpecialKeys);
    glutReshapeFunc(reshape);
    criarMenus(); // ← cria os menus
    glutMainLoop();
    return 0;
}


