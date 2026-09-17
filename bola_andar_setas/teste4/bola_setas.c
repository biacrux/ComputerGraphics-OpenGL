#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>

float xf = 0, yf = 0; //variaveis globais da posicao da esfera 
float limX = 0, limY = 0; // LIMITES DO ECRAAA
float raio = 0.5f; // rai da esfera

void display() { //desenhar a cena de cada frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // limpa o buffer de cor e profundidade
    glMatrixMode(GL_MODELVIEW); // onde fazemos a transformacoes dos objetos 
    glLoadIdentity(); //reset da matriz 
    glTranslatef(xf, yf, 0.0f); //move a esfera
    glColor3f(1.0f, 0.4f, 0.7f); //define a cor da esfera
    glutSolidSphere(raio, 32, 32); //desenha a esfera solida
    glutSwapBuffers(); //evita flickerling
}

void SpecialKeys(int key, int x, int y) { // teclas pressionadas
    float passo = 0.3f; //quantidade de moviemnto que ela anda ao carregar na tecla
    switch (key) {
    case GLUT_KEY_UP:    yf += passo; break; //seta cima
    case GLUT_KEY_DOWN:  yf -= passo; break; //seta baixo
    case GLUT_KEY_LEFT:  xf -= passo; break; //seta esq
    case GLUT_KEY_RIGHT: xf += passo; break; //seta direita
    }
    xf = fmaxf(-limX, fminf(xf, limX)); // garanete que a esfera nao passe o limite direito nem esquerda
    yf = fmaxf(-limY, fminf(yf, limY)); // garante que a esfera nao passe o limite cima e de baixo

    glutPostRedisplay(); // avisa que a janela tem de ser redesenhada
}

void reshape(int width, int height) { // chamada quando a janela e redesenhada 
    if (height == 0) height = 1; // evita divisao por 0
    float aspect = (float)width / height; //largura /altura da janeça

    glViewport(0, 0, width, height); // define a janela
    glMatrixMode(GL_PROJECTION); // define o modo de projecao no ecra da esfera nao estar distorcida 
    glLoadIdentity(); //reset da matriz 

    // Projecao ortografica — sem distorcao nos cantos
    glOrtho(-aspect * 5, aspect * 5, -5, 5, -10, 10);

    // Limites exatos baseados na projecao
    limX = aspect * 5 - raio;
    limY = 5.0f - raio;

    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); //modo de displei animacao, cor, profundidade(para a esfera nao ficar distorcida)
    glutInitWindowSize(600, 400); // tamanho da janela
    glutCreateWindow("Esfera Animada"); //titulo do jogo
    glEnable(GL_DEPTH_TEST);// ative a prof ,necessario para rend corretamente)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // define cor de fundo

    //funcoes que fiz
    glutDisplayFunc(display); // oque desnhar 
    glutSpecialFunc(SpecialKeys); // teclas
    glutReshapeFunc(reshape); //rederizar o ecra 

    glutMainLoop(); //loop infinito ate carregar no X
    return 0;
}