
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

float bolaZ = -10.0f;       // posi��o inicial da bola (eixo Z, negativo = longe da c�mera)
const float posInicialZ = -10.0f;
int bolaEmMovimento = 0;
float velocidadeBola = 0.6f; // quanto a bola avan�a por frame

const float linhaGolZ = -40.0f; // posi��o da linha do gol
const float bolaRaio = 1.0f;

int gols = 0;
int erros = 0;

/* Mensagem tempor�ria exibida ap�s chute */
char mensagem[128] = "";
float mensagemTimer = 0.0f; // segundos restantes para exibir

/* Goleiro */
float goleiroX = 0.0f;
float goleiroMinX = -4.0f;
float goleiroMaxX = 4.0f;
float goleiroDir = 1.0f;
float goleiroVel = 0.09f;
float goleiroLargura = 2.5f;
float goleiroAltura = 2.5f;
int usarGoleiro = 1;

/* Tempo entre frames para anima��o (simples) */
int lastTime = 0;

/* C�mera simples */
float camX = 0.0f, camY = 8.0f, camZ = 5.0f;

// Identificador da janela principal e da janela do nome
int janelaPrincipalID;
int janelaNomeID = 0;


GLfloat escalaNome = 1.0f; // escala geral
GLfloat anguloNome = 0.0f; // angulo rota�ao
GLfloat transXNome = 0.0f; // horizontal
GLfloat transYNome = 0.0f; // vertical




void desenhaTextoBitmap(float x, float y, const char* str) {
    // Mantido o mesmo
    glRasterPos2f(x, y);
    while (*str) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *str++);
    }
}

void desenharGol() {
    // desenha traves (cubos) e travess�o (um cubo esticado)
    glColor3f(1.0f, 1.0f, 1.0f);

    // trave esquerda
    glPushMatrix();
        glTranslatef(-5.0f, 1.5f, linhaGolZ);
        glScalef(0.5f, 3.0f, 0.5f);
        glutSolidCube(1.0f);
    glPopMatrix();

    // trave direita
    glPushMatrix();
        glTranslatef(5.0f, 1.5f, linhaGolZ);
        glScalef(0.5f, 3.0f, 0.5f);
        glutSolidCube(1.0f);
    glPopMatrix();

    // travess�o
    glPushMatrix();
        glTranslatef(0.0f, 3.0f, linhaGolZ);
        glScalef(11.0f, 0.5f, 0.5f);
        glutSolidCube(1.0f);
    glPopMatrix();

    // linha do gol
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
        glVertex3f(-6.0f, 0.01f, linhaGolZ - 0.2f);
        glVertex3f(6.0f, 0.01f, linhaGolZ - 0.2f);
        glVertex3f(6.0f, 0.01f, linhaGolZ + 0.2f);
        glVertex3f(-6.0f, 0.01f, linhaGolZ + 0.2f);
    glEnd();
}


void desenharBola() {
    // ... (Mantido o mesmo) ...
    glPushMatrix();
        glTranslatef(0.0f, bolaRaio, bolaZ);
        glColor3f(1.0f, 0.8f, 0.0f);
        glutSolidSphere(bolaRaio, 24, 24);
    glPopMatrix();
}


void desenharGoleiro() {

    if (!usarGoleiro) return;

    glPushMatrix();
        glTranslatef(goleiroX, goleiroAltura / 2.0f, linhaGolZ + 1.0f); // ligeiramente � frente da linha
        glScalef(goleiroLargura, goleiroAltura, 1.0f);
        glColor3f(0.2f, 0.2f, 0.8f);
        glutSolidCube(1.0f);
    glPopMatrix();
}


void desenharCampo() {

    glColor3f(0.0f, 0.6f, 0.0f);
    glBegin(GL_QUADS);
        glVertex3f(-60.0f, 0.0f, 60.0f);
        glVertex3f(60.0f, 0.0f, 60.0f);
        glVertex3f(60.0f, 0.0f, -120.0f);
        glVertex3f(-60.0f, 0.0f, -120.0f);
    glEnd();
}


void desenharHUD() {

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
        glLoadIdentity();
        int w = glutGet(GLUT_WINDOW_WIDTH);
        int h = glutGet(GLUT_WINDOW_HEIGHT);
        gluOrtho2D(0, w, 0, h);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            glLoadIdentity();
            glDisable(GL_LIGHTING);

            char placar[128];
            sprintf(placar, "Gols: %d    Erros: %d", gols, erros);
            glColor3f(1,1,1);
            glRasterPos2i(10, h - 24);
            const char *p = placar;
            while (*p) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p++);

            if (mensagemTimer > 0.0f && strlen(mensagem) > 0) {
                int len = strlen(mensagem);
                // Calcula a posi��o x para centralizar a mensagem
                // 9 � uma estimativa de largura de caractere com GLUT_BITMAP_HELVETICA_18
                int x = w/2 - (len * 9)/2;
                glRasterPos2i(x, h - 60);
                p = mensagem;
                while (*p) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p++);
            }

            // Mensagem de controle na janela principal
            if (janelaNomeID != 0) {
                snprintf(mensagem, sizeof(mensagem), "JANELA 'NOME' ABERTA! (CONTROLES: '+', '-', 'q', 'e', 'w', 'a', 's', 'd')");
                int len = strlen(mensagem);
                int x = w/2 - (len * 9)/2;
                glColor3f(1.0f, 0.0f, 0.0f); // Cor vermelha para destacar
                glRasterPos2i(x, 10); // Inferior da tela
                p = mensagem;
                while (*p) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p++);
            }


            glEnable(GL_LIGHTING);
        glPopMatrix();

        glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}


void checarGolOuDefesa() {
    // ... (Mantido o mesmo) ...
    // Consideramos que a bola est� centralizada em x=0
    int foiGol = 0;
    int defendido = 0;

    // Primeiro verificar se a bola est� entre as traves (x entre -5 e 5)
    float bolaX = 0.0f;
    if (bolaX < -5.0f || bolaX > 5.0f) {
        // bola fora das traves -> erro (n�o foi gol)
        foiGol = 0;
    } else {
        // Agora checar goleiro
        if (usarGoleiro) {
            // Se a proje��o do goleiro cobre o eixo X da bola
            float metadeGoleiro = goleiroLargura / 2.0f;
            if ( (goleiroX - metadeGoleiro) <= bolaX + bolaRaio &&
                 (goleiroX + metadeGoleiro) >= bolaX - bolaRaio ) {
                defendido = 1;
            } else {
                foiGol = 1;
            }
        } else {
            foiGol = 1;
        }
    }

    if (defendido) {
        erros++; // conta como chute errado (defesa)
        snprintf(mensagem, sizeof(mensagem), "DEFENDIDO!");
    } else if (foiGol) {
        gols++;
        snprintf(mensagem, sizeof(mensagem), "GOL!");
    } else {
        erros++;
        snprintf(mensagem, sizeof(mensagem), "NAO FOI GOL!");
    }
    mensagemTimer = 2.0f; // mostra mensagem por 2 segundos
}

/* Redefine bola para posi��o inicial (C�DIGO 1) */
void resetBola() {
    // ... (Mantido o mesmo) ...
    bolaZ = posInicialZ;
    bolaEmMovimento = 0;
}

/* IDLE/ANIMA��O (C�DIGO 1) */
void idle(void) {
    // ... (Mantido o mesmo) ...
    int current = glutGet(GLUT_ELAPSED_TIME);
    float delta = (current - lastTime) / 1000.0f; // segundos
    if (lastTime == 0) delta = 0;
    lastTime = current;

    /* Atualiza goleiro */
    if (usarGoleiro) {
        goleiroX += goleiroDir * goleiroVel * (delta * 60.0f); // normalizar para frames
        if (goleiroX > goleiroMaxX) {
            goleiroX = goleiroMaxX;
            goleiroDir = -1.0f;
        } else if (goleiroX < goleiroMinX) {
            goleiroX = goleiroMinX;
            goleiroDir = 1.0f;
        }
    }

    /* Se bola em movimento, avan�a */
    if (bolaEmMovimento) {
        bolaZ -= velocidadeBola * (delta * 60.0f);

        /* Checar se passou a linha do gol */
        if (bolaZ <= linhaGolZ) {
            checarGolOuDefesa();
            resetBola();
        }
    }

    /* Atualiza timer da mensagem */
    if (mensagemTimer > 0.0f) {
        mensagemTimer -= delta;
        if (mensagemTimer <= 0.0f) {
            mensagemTimer = 0.0f;
            if (janelaNomeID == 0) { // Limpa a mensagem apenas se a janela do nome n�o estiver aberta
                mensagem[0] = '\0';
            }
        }
    }

    // Apenas redesenha a janela principal
    glutPostRedisplay();
}

/* Desenha cena 3D (C�DIGO 1) */
void display(void) {
    // ... (Mantido o mesmo) ...
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Define a c�mera para a cena 3D
    gluLookAt(camX + 0.0f, camY + 6.0f, camZ + 25.0f,  // cam pos
              0.0f, 1.0f, -20.0f,  // look at
              0.0f, 1.0f, 0.0f);    // up

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat lightpos[] = { 50.0f, 50.0f, 50.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

    /* Campo */
    desenharCampo();

    /* Gol */
    desenharGol();

    /* Goleiro */
    desenharGoleiro();

    /* Bola */
    desenharBola();

    /* HUD em 2D */
    desenharHUD();

    glutSwapBuffers();
}

/* Redimensiona janela (C�DIGO 1) */
void reshape(int w, int h) {
    // ... (Mantido o mesmo) ...
    if (h == 0) h = 1;
    float ratio = (float)w / (float)h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, ratio, 0.1f, 500.0f);
    glMatrixMode(GL_MODELVIEW);
}


/* --- FUN��ES DO SEGUNDO C�DIGO (NOME) ADAPTADAS --- */

// Fun��o de fechamento da janela do nome, para resetar o ID e permitir abrir novamente
void fechaJanelaNome() {
    janelaNomeID = 0; // Marca que a janela foi fechada
    mensagemTimer = 0.0f;
    mensagem[0] = '\0';
    glutPostRedisplay(); // Redesenha a janela principal para atualizar o HUD
}

// Fun��o para desenhar a letra a
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

// Fun��o para desenhar a letra s
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

// Fun��o para desenhar a letra h
void desenhaH(float x, float y) {
    glBegin(GL_LINES);
    glVertex2f(x, y);      glVertex2f(x, y+1);
    glVertex2f(x+1.0, y); glVertex2f(x+1.0, y+1);
    glVertex2f(x, y+0.5); glVertex2f(x+1.0, y+0.5);
    glEnd();
}

// Fun��o para desenhar a letra f
void desenhaF(float x, float y) {
    glBegin(GL_LINES);
    glVertex2f(x, y);      glVertex2f(x, y+1);
    glVertex2f(x, y+1);    glVertex2f(x+1.0, y+1);
    glVertex2f(x, y+0.5); glVertex2f(x+0.75, y+0.5);
    glEnd();
}

// Fun�ao para desenhar a letra e
void desenhaE(float x, float y) {
    glBegin(GL_LINES);
    glVertex2f(x, y);      glVertex2f(x, y+1);
    glVertex2f(x, y+1);    glVertex2f(x+1.0, y+1);
    glVertex2f(x, y+0.5); glVertex2f(x+0.75, y+0.5);
    glVertex2f(x, y);      glVertex2f(x+1.0, y);
    glEnd();
}

void desenhaNome() {
    float x = -4.2f, y = -0.5f; // posi�ao inicial

    glColor3f(0.0f, 0.0f, 0.0f); // Cor preta para o nome
    desenhaA(x, y); x += 1.5f;
    desenhaS(x, y); x += 1.5f;
    desenhaA(x, y); x += 1.5f;
    desenhaH(x, y); x += 1.5f;
    desenhaF(x, y); x += 1.5f;
    desenhaE(x, y);
}

// Fun��o de desenho para a janela do nome
void displayNome() {
    glutSetWindow(janelaNomeID); // Garante que estamos desenhando na janela correta
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Ajusta o gluOrtho2D para que a escala afete a 'janela' de visualiza��o
    GLfloat metade = 7.0f / escalaNome;
    gluOrtho2D(-metade, metade, -metade, metade);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // aplica transla��o
    glTranslatef(transXNome, transYNome, 0.0f);

    // aplica rota��o
    glRotatef(anguloNome, 0.0f, 0.0f, 1.0f);

    desenhaNome();

    // Desenha as instru��es no canto
    glColor3f(0.5f, 0.5f, 0.5f); // Cinza
    glLoadIdentity(); // Reseta a matriz de vis�o para texto 2D


    glutSwapBuffers(); // glutSwapBuffers para evitar flickering
}

// Fun��o de teclado para a janela do nome
void tecladoNome(unsigned char tecla, int x, int y) {
    glutSetWindow(janelaNomeID); // Garante que os comandos afetem a janela do nome

    switch (tecla) {
        case '+':
            escalaNome += 0.1f;
            break;
        case '-':
            escalaNome -= 0.1f;
            if (escalaNome < 0.1f) escalaNome = 0.1f;
            break;
        case 'q': // rota�ao anti-horario
            anguloNome += 5.0f;
            break;
        case 'e': // rota�ao horario
            anguloNome -= 5.0f;
            break;
        case 'a': // esquerda
            transXNome -= 0.5f;
            break;
        case 'd': // direita
            transXNome += 0.5f;
            break;
        case 'w': // cima
            transYNome += 0.5f;
            break;
        case 's': // baixo
            transYNome -= 0.5f;
            break;
        case 27: // ESC
            glutDestroyWindow(janelaNomeID);
            fechaJanelaNome();
            return; // Retorna para n�o chamar glutPostRedisplay
    }
    glutPostRedisplay();
}

void reshapeNome(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glutPostRedisplay();
}

/* --- FUN��O PRINCIPAL DE EXECU��O CHAMADA PELO 'c' (C�DIGO 1) --- */

void executaDemoNome() {
    if (janelaNomeID != 0) {
        // Se a janela j� estiver aberta, apenas a traz para a frente (foco)
        glutSetWindow(janelaNomeID);
        return;
    }

    // Configura a mensagem de status na janela principal
    snprintf(mensagem, sizeof(mensagem), "JANELA 'NOME' ABERTA. Controles: '+', '-', 'q', 'e', 'w', 'a', 's', 'd'");
    mensagemTimer = 5.0f; // Exibe por um tempo maior

    // Cria a nova janela
    janelaNomeID = glutCreateWindow("Trabalho 1: Transformacoes (Nome)");

    // Define o que acontece com a nova janela
    glutDisplayFunc(displayNome);
    glutKeyboardFunc(tecladoNome);
    glutReshapeFunc(reshapeNome);

    // Configura��es visuais iniciais da nova janela
    glClearColor(1.0, 1.0, 1.0, 1.0); // Fundo Branco

    // Volta o contexto para a janela principal (importante para que o idle continue atualizando o jogo)
    glutSetWindow(janelaPrincipalID);
}


/* Teclado normal (chars) (C�DIGO 1) */
void keyboard(unsigned char key, int x, int y) {


    if (janelaNomeID != 0 && key != 27) {
        // Se a janela do nome estiver aberta, n�o processa comandos do jogo
        // exceto o ESC, que j� est� tratado abaixo.
        return;
    }

    switch (key) {
        case 'q': // afasta a bola do observador (diminuir Z)
            if (!bolaEmMovimento) bolaZ -= 1.0f;
            break;
        case 'a': // aproxima a bola do observador (aumentar Z)
            if (!bolaEmMovimento) bolaZ += 1.0f;
            break;
        case 'r': // reseta placar
            gols = 0;
            erros = 0;
            snprintf(mensagem, sizeof(mensagem), "PLACAR ZERADO");
            mensagemTimer = 1.5f;
            break;
        case 'c':
            // FUN��O PRINCIPAL: CHAMA A NOVA JANELA
            executaDemoNome();
            break;
        case 27: // ESC
            // Se a janela do nome estiver aberta, fecha primeiro
            if (janelaNomeID != 0) {
                 glutDestroyWindow(janelaNomeID);
                 fechaJanelaNome();
            } else {
                 exit(0);
            }
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

/* Tecla de espa�o (chute) (C�DIGO 1) */
void keyboardDown(unsigned char key, int x, int y) {
    if (key == 32) { // espa�o
        if (!bolaEmMovimento && janelaNomeID == 0) { // Chuta apenas se a janela do nome estiver fechada
            bolaEmMovimento = 1;
            snprintf(mensagem, sizeof(mensagem), "CHUTE!");
            mensagemTimer = 0.8f;
        }
    } else {
        keyboard(key, x, y);
    }
}

/* Inicializa��o OpenGL (C�DIGO 1) */
void initGL() {
    // ... (Mantido o mesmo) ...
    glClearColor(0.53f, 0.81f, 0.98f, 1.0f); // c�u
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glShadeModel(GL_SMOOTH);
}

/* Exibe instru��es no console (C�DIGO 1) */
void printInstructions() {
    printf("Controles do Jogo:\n");
    printf("  Espaco - chutar a bola\n");
    printf("  q - afastar a bola do observador (antes de chutar)\n");
    printf("  a - aproximar a bola do observador (antes de chutar)\n");
    printf("  r - resetar placar\n");
    printf("  c - EXECUTAR DEMO DO TRABALHO 1 (ABRE NOVA JANELA)\n");
    printf("  ESC - sair\n");
    printf("\nControles da Janela 'Nome' (Tecla 'c'):\n");
    printf("  '+', '-' - Zoom/Escala\n");
    printf("  'q', 'e' - Rotacao\n");
    printf("  'w', 'a', 's', 'd' - Translacao\n\n");
}

/* Main (C�DIGO 1) */
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(1000, 600);
    glutInitWindowPosition(100, 100);
    janelaPrincipalID = glutCreateWindow("Trabalho: Chute a Bola - FreeGLUT (C) - com goleiro");

    initGL();
    printInstructions();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    // usamos uma vers�o que captura a barra de espa�o corretamente:
    glutKeyboardFunc(keyboardDown);

    lastTime = 0;
    resetBola();

    glutMainLoop();
    return 0;
}
