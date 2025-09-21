#include <GL/glut.h>

GLfloat escala = 1.0f;

GLfloat angulo = 0.0f;

GLfloat transX = 0.0f; // deslocamento horizontal

GLfloat transY = 0.0f; // deslocamento vertical



// Função para desenhar a letra A
void desenhaA(float x, float y) {
    glBegin(GL_LINES);
    glVertex2f(x, y);
    glVertex2f(x+0.5, y+1);

    glVertex2f(x+0.5, y+1);
    glVertex2f(x+1.0, y);

    glVertex2f(x+0.25, y+0.5);
    glVertex2f(x+0.75, y+0.5);
    glEnd();
}

// Função para desenhar a letra S
void desenhaS(float x, float y) {
    glBegin(GL_LINE_STRIP);
    glVertex2f(x+1.0, y+1);
    glVertex2f(x, y+1);
    glVertex2f(x, y+0.5);
    glVertex2f(x+1.0, y+0.5);
    glVertex2f(x+1.0, y);
    glVertex2f(x, y);
    glEnd();
}

// Função para desenhar a letra H
void desenhaH(float x, float y) {
    glBegin(GL_LINES);
    glVertex2f(x, y);     glVertex2f(x, y+1);
    glVertex2f(x+1.0, y); glVertex2f(x+1.0, y+1);
    glVertex2f(x, y+0.5); glVertex2f(x+1.0, y+0.5);
    glEnd();
}

// Função para desenhar a letra F
void desenhaF(float x, float y) {
    glBegin(GL_LINES);
    glVertex2f(x, y);     glVertex2f(x, y+1);
    glVertex2f(x, y+1);   glVertex2f(x+1.0, y+1);
    glVertex2f(x, y+0.5); glVertex2f(x+0.75, y+0.5);
    glEnd();
}

// Função para desenhar a letra E
void desenhaE(float x, float y) {
    glBegin(GL_LINES);
    glVertex2f(x, y);     glVertex2f(x, y+1);
    glVertex2f(x, y+1);   glVertex2f(x+1.0, y+1);
    glVertex2f(x, y+0.5); glVertex2f(x+0.75, y+0.5);
    glVertex2f(x, y);     glVertex2f(x+1.0, y);
    glEnd();
}

void desenhaNome() {
    float x = -4.2f, y = -0.5f; // posição inicial

    glColor3f(0, 0, 0);
    desenhaA(x, y); x += 1.5f;
    desenhaS(x, y); x += 1.5f;
    desenhaA(x, y); x += 1.5f;
    desenhaH(x, y); x += 1.5f;
    desenhaF(x, y); x += 1.5f;
    desenhaE(x, y);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat metade = 7.0f / escala;
    gluOrtho2D(-metade, metade, -metade, metade);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // aplica translação
    glTranslatef(transX, transY, 0.0f);

    // aplica rotação
    glRotatef(angulo, 0.0f, 0.0f, 1.0f);

    // aplica escala
    glScalef(escala, escala, 1.0f);

    desenhaNome();

    glFlush();
}


void teclado(unsigned char tecla, int x, int y) {
    switch (tecla) {
        case '+':
            escala += 0.1f;
            break;
        case '-':
            escala -= 0.1f;
            if (escala < 0.1f) escala = 0.1f; // F4
            break;
        case 'q': // rotação anti-horário
            angulo += 5.0f;
            break;
        case 'e': // rotação horário
            angulo -= 5.0f;
            break;
        case 'a': // esquerda
            transX -= 0.5f;
            break;
        case 'd': // direita
            transX += 0.5f;
            break;
        case 'w': // cima
            transY += 0.5f;
            break;
        case 's': // baixo
            transY -= 0.5f;
            break;
        case 27:
            exit(0);
    }
    glutPostRedisplay();
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Nome com Linhas");

    glClearColor(1.0, 1.0, 1.0, 1.0);

    glutDisplayFunc(display);
    glutKeyboardFunc(teclado);

    glutMainLoop();
    return 0;
}
