#include <GL/glut.h>
#include <iostream>
typedef struct {
float x, y;
} Ponto;

typedef struct {
Ponto p1;
Ponto p2;
Ponto p3;
} Triangulo;

GLfloat escala;

Triangulo triangulo1;

void inicializaVariaveis(){

    escala=1.0f;
    triangulo1.p1.x = -2.0f; triangulo1.p1.y = -1.0f;
    triangulo1.p2.x = 2.0f;  triangulo1.p2.y = -1.0f;
    triangulo1.p3.x = 0.0f;  triangulo1.p3.y = 2.0f;

    }


void desenha() {
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();


    GLfloat metade = 5.0f / escala;
    gluOrtho2D(-metade, metade, -metade, metade);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBegin(GL_TRIANGLES);
    glColor3f(0, 1, 0);
    glVertex2f(triangulo1.p1.x, triangulo1.p1.y);
    glVertex2f(triangulo1.p2.x, triangulo1.p2.y);
    glColor3f(1, 0, 0);
    glVertex2f(triangulo1.p3.x, triangulo1.p3.y);

    glEnd();

    glFlush();
}

void teclado(unsigned char tecla, int x, int y) {
    switch (tecla) {
        case '+':
            escala += 0.1f;
            break;
        case '-':

                escala -= 0.1f;
            break;
        case 27:
            exit(0);
    }

    glutPostRedisplay();
}

int main(int argc, char *argv[]) {

    inicializaVariaveis();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Zoom com + e -");

    glClearColor(1.0, 1.0, 1.0, 1.0);

    glutDisplayFunc(desenha);
    glutKeyboardFunc(teclado);

    glutMainLoop();
    return 0;
}

