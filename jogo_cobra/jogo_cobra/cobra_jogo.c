#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include "game.h"



void display(void) {
	glClearColor(0, 0, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
}

void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Exercicio 0");
	glutDisplayFunc(display);
	glutMainLoop();
}