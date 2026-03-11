#include <GL/glut.h> // inclui as librarias (include)

void display(void) {
	glClearColor(0.0, 0.0, 0.0, 0.0); // limpa buffers
	glClear(GL_COLOR_BUFFER_BIT); // indica a cor de fundo a ser utilizada por glClear
	glColor3f(1, 0, 0); // cor vermelha 
	glMatrixMode(GL_PROJECTION); // sistema  das cordenadas 
	glLoadIdentity(); // apaga qualauquer alteracao anterior
	gluOrtho2D(-2, 2, -2, 2); // define uma projecao 2D -2 esquerda 2 direita -2 baixo 2 cima 

	glBegin(GL_LINE_STRIP); // pontos ligados em linha em sequencia continua 

		// este codigo vai ser para fazer um quadrado1 
		glVertex2f(-0.5, -0.5);
		//glVertex2f(-0.5, 0.5); //  vai ficar igual um quadrado sem estas 

		glVertex2f(-0.5, 0.5);
		//glVertex2f(0.5, 0.5);

		glVertex2f(0.5, 0.5);
		//glVertex2f(0.5, -0.5);

		glVertex2f(0.5, -0.5);
		glVertex2f(-0.5, -0.5); // no fim e que temos de unir as dp inicio
		glEnd();
	
	glColor3f(0, 1, 0);// cor verde
		glBegin(GL_LINES); 

		//este codigo vai ser para criar um quadrado 2 maior (se fosse com GL_LINE_STRIP  so precisava de um sem duplicados)
		glVertex2f(-1, -1);
		glVertex2f(-1, 1);
		glVertex2f(-1, 1);
		glVertex2f(1, 1);
		
		glVertex2f(1, 1);
		glVertex2f(1, -1);
		glVertex2f(1, -1);
		glVertex2f(-1, -1);
		glEnd();
		
	glColor3f(0, 0, 1);// cor azul
		glBegin(GL_LINES);

		//este cldigo vai nos dar linhas na diagonal pq estamos a usar gl_lines
		glVertex2f(-1, -1);
		glVertex2f(-0.5, -0.5);
		glVertex2f(1, 1);
		glVertex2f(0.5, 0.5);
		glVertex2f(-1, 1);
		glVertex2f(-0.5, 0.5);
		glVertex2f(1, -1);
		glVertex2f(0.5, -0.5);
		glEnd();

		glFlush(); // obriga o openGl a completar todas as operações pendentes

}

void main(int argc, char** argv) {
	glutInit(&argc, argv); // inicializa a glut
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); //b indica o modo de apresentação a utilizar
	glutInitWindowSize(500, 500); // Alterar tamanho da janela antes de começar
	glutCreateWindow("Exercicio 0"); // Alterar nome da janela antes de começar
	glutDisplayFunc(display); // chamada da função
	glutMainLoop(); // entra no ciclo principal
}