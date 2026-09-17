#include <windows.h>
#include <GL/glut.h>

GLfloat X_move = 0.0f, Y_move = 0.0f, Z_move = -25.0f; // Velocidade de movimento do retângulo
int WindowsAltura = 1080, WindowsLargura = 1920;
int WindowsPosX = 50, WindowsPosY = 50; // Posição da janela
float FiltroRed = 0.0f, FiltroGreen = 0.0f, FiltroBlue = 0.0f, FiltroAlpha = 1.0f;

void Fundo(GLvoid) {
	glClearColor(FiltroRed, FiltroGreen, FiltroBlue, FiltroAlpha); // Cor de fundo (preto)
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void Cenario() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Limpa o buffer de cor	

	glLoadIdentity(); // Carrega a matriz identidade
	glTranslatef(X_move, Y_move, Z_move);

	glBegin(GL_QUADS); // Desenha um quadrado
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);
	glVertex2f(-1.0f, -1.0f);
	glVertex2f(1.0f, -1.0f);
	glVertex2f(1.0f, 1.0f);
	glEnd();

	glTranslatef(X_move + 2.3f, Y_move, 0.0f);

	glBegin(GL_QUADS); // Desenha um quadrado
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);
	glVertex2f(-1.0f, -1.0f);
	glVertex2f(1.0f, -1.0f);
	glVertex2f(1.0f, 1.0f);
	glEnd();

	glTranslatef(X_move + 2.3f, Y_move, 0.0f);

	glBegin(GL_QUADS); // Desenha um quadrado
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);
	glVertex2f(-1.0f, -1.0f);
	glVertex2f(1.0f, -1.0f);
	glVertex2f(1.0f, 1.0f);
	glEnd();

	glTranslatef(X_move + 2.3f, Y_move, 0.0f);

	glBegin(GL_QUADS); // Desenha um quadrado
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);
	glVertex2f(-1.0f, -1.0f);
	glVertex2f(1.0f, -1.0f);
	glVertex2f(1.0f, 1.0f);
	glEnd();

	glutSwapBuffers();
}

void Teclado(unsigned char key, int x, int y) {

	switch (key) {
	case 27: // Tecla ESC
		exit(0); // Encerra o programa
		break;

	case 'W': // Tecla W
	case 'w':
		Y_move += 0.1f; // Move para cima
		break;

	case 'S': // Tecla W
	case 's':
		Y_move -= 0.1f; // Move para cima
		break;

	case 'A': // Tecla W
	case 'a':
		X_move -= 0.1f; // Move para cima
		break;

	case 'D': // Tecla W
	case 'd':
		X_move += 0.1f; // Move para cima
		break;

	default:
		break;
	}
}

void AlterarTamanho(GLsizei w, GLsizei h)
{
	// Prevent a divide by zero
	if (h == 0)
		h = 1;

	// Set Viewport to window dimensions
	glViewport(0, 0, w, h);

	// Reset coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0, (float)WindowsLargura / (float)WindowsAltura, 0.1, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(WindowsAltura, WindowsLargura);
	glutInitWindowPosition(WindowsPosX, WindowsPosY);
	glutCreateWindow("Game");
	glutFullScreen();

	glutDisplayFunc(Cenario); // Define a função de exibição
	glutReshapeFunc(AlterarTamanho); // Define a função de redimensionamento
	glutKeyboardFunc(Teclado);
	glutIdleFunc(Cenario);
	Fundo(); // Chama a função de fundo

	glutMainLoop(); // Inicia o loop principal do GLUT
	return 0;
}
