#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

float x = 0.0;
float velocidade = 0.02;

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Camera aqui dentro,  para onde esta a olhar (de frente )
    gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);

    // Move a esfera, se tiramos ja ano a move
    glTranslatef(x, 0.0, 0.0);

    // Cor rosa
    glColor3f(1.0, 0.4, 0.7);
    glutSolidSphere(0.5, 32, 32);

    glutSwapBuffers();// usamos quando ha uma animacao
    //glFlush() -> imagem estática
}

void update(int value) { // vai mover a  esfera ===========================
    x += velocidade; // anda para a direita
    if (x > 2.5) x = -2.5; //se sai do "eca volta ao sitio inicial"
    glutPostRedisplay(); //redesenha
    glutTimerFunc(16, update, 0); // repete a cada 16ms
}

void reshape(int width, int height) { // serve para a janela mude de tamanho e nao ficar distorcida netse caso esticada ============
    glViewport(0, 0, width, height); //define a area de desenho da janela
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)width / height, 1.0, 100.0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv); //inicializa o glut (TEM DE LA ESTAR SEMPRE)
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // (TEM DE LA ESTAR SEMPRE)
    glutInitWindowSize(600, 400); // inicializa a janela em pixeis
    glutCreateWindow("Esfera Animada");// titulo do "jogo"
    glClearColor(0.0, 0.0, 0.0, 1.0); // define a cor de fundo do jogo
    glutDisplayFunc(display); // funcoes que vai buscar 
    glutReshapeFunc(reshape);// funcoes que vai buscar 
    glutTimerFunc(16, update, 0); //inicia o timer
    glutMainLoop(); // entra no loop infinito (TEM DE LA ESTAR SEMPRE)
    return 0;
}