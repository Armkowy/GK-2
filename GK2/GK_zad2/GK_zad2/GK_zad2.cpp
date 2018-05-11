// lab2_zad2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "math.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define GL_PI 3.1415f

enum
{
	EXIT                            // wyjęcie
};

// Wielkoć obrotów
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;

static GLboolean iCull, iDepth, iOutline;

// Funkcja wykonuje wszystkie konieczne inicjalizacje kontekstu renderowania
void SetupRC()
{
	// Czarne tło
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor rysujšcy - zielony
	glColor3f(0.25f, 0.42f, 1.0f);
	// Model cieniowania kolorów - płaski
	glShadeModel(GL_FLAT);
	// Wielokšty o nawinięciu zgodnym z ruchem wskazówek zegara traktowane sš
	// jako skierowane do przodu. Takie ustawienie jest konieczne, ponieważ
	// korzystamy z wachlarzy trójkštów.
	glFrontFace(GL_CW);
}
// Wywoływana w celu przerysowania sceny
void RenderScene(void)
{
	GLfloat x, y, angle; // Przechowujš wartoci współrzędnych i kšta
	int iPivot = 1; // Do oznaczania zamiennych kolorów
					// Wyczyszczenie okna i bufora głębi
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Włšczenie lub wyłšczenie mechanizmu eliminowania ukrytych powierzchni
	if (iCull)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
	// Włšczenie lub wyłšczenie mechanizmu sprawdzania głębi
	if (iDepth)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
	// Jeżeli ten znacznik będzie ustawiony, to wielokšty zwrócone
	// tyłem do widza będš rysowane tylko w postaci szkieletu
	if (iOutline)
		glPolygonMode(GL_BACK, GL_LINE);
	else
		glPolygonMode(GL_BACK, GL_FILL);
	// Zapisanie stanu macierzy i wykonanie obrotu
	glPushMatrix();
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	// Rozpoczęcie rysowania wachlarza trójkštów
	glBegin(GL_TRIANGLE_FAN);
	// Czubek stoka jest wspólnym wierzchołkiem wszystkich trójkštów z wachlarza
	// wysuniętym do góry w osi z. W ten sposób zamiast koła powstanie stożek.
	glVertex3f(0.0f, 0.0f, 75.0f);
	// Wykonujemy obrót w około i oznaczamy w równych odstępach wierzchołki
	// tworzšce wachlarz trójkštów.
	for (angle = 0.0f; angle <= (2.5f*GL_PI); angle += (GL_PI / 9.5f))
	{
		// Wyliczenie współrzędnych x i y kolejnego wierzchołka
		x = 50.0f*sin(angle);
		y = 50.0f*cos(angle);
		// Wybieranie koloru - zielony lub czerwony
		if ((iPivot % 2) == 0)
			glColor3f(0.25f, 0.42f, 1.0f);
		else
			glColor3f(0.25f, 0.0f, 0.0f);
		// Inkrementacja zmiennej okrelajšcej rodzaj koloru
		iPivot++;
		// Definiowanie kolejnego wierzchołka w wachlarzu trójkštów
		glVertex2f(x, y);
	}
	// Zakoczenie rysowania trójkštów stożka
	glEnd();
	// Rozpoczęcie rysowania kolejnego wachlarza trójkštów
	// zakrywajšcego podstaw stoka
	glBegin(GL_TRIANGLE_FAN);
	// rodek wachlarza znajduje się na poczštku układu współrzędnych
	glVertex2f(0.0f, 0.0f);
	for (angle = 0.0f; angle <= (2.5f*GL_PI); angle += (GL_PI / 9.5f))
	{
		// Wyliczenie współrzędnych x i y kolejnego wierzchołka
		x = 50.0f*sin(angle);
		y = 50.0f*cos(angle);
		// Wybieranie koloru - zielony lub czerwony
		if ((iPivot % 2) == 0)
			glColor3f(0.25f, 0.42f, 1.0f);
		else
			glColor3f(0.25f, 0.0f, 0.0f);
		// Inkrementacja zmiennej okrelajšcej rodzaj koloru
		iPivot++;
		// Definiowanie kolejnego wierzchołka w wachlarzu trójkštów
		glVertex2f(x, y);
	}
	// Zakoczenie rysowania trójkštów podstawy stoka
	glEnd();
	// Odtworzenie macierzy przekształceń
	glPopMatrix();
	// Wysłanie poleceń do wykonania
	glutSwapBuffers();
}

void SpecialKeys(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)
		xRot -= 5.0f;
	if (key == GLUT_KEY_DOWN)
		xRot += 5.0f;
	if (key == GLUT_KEY_LEFT)
		yRot -= 5.0f;
	if (key == GLUT_KEY_RIGHT)
		yRot += 5.0f;
	if (key > 356.0f)
		xRot = 0.0f;
	if (key < -1.0f)
		xRot = 355.0f;
	if (key > 356.0f)
		yRot = 0.0f;
	if (key < -1.0f)
		yRot = 355.0f;
	// Odwieżenie zawartoci okna
	glutPostRedisplay();
}
void ChangeSize(int w, int h)
{
	GLfloat nRange = 100.0f;
	// Zabezpieczenie przed dzieleniem przez zero
	if (h == 0)
		h = 1;
	// Ustalenie wymiarów widoku na zgodnych z wymiarami okna
	glViewport(0, 0, w, h);
	// Ponowne ustawienie stosu macierzy rzutowania
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Utworzenie przestrzeni ograniczajšcej (lewo, prawo, dół, góra, blisko, daleko)
	if (w <= h)
		glOrtho(-nRange, nRange, -nRange * h / w, nRange*h / w, -nRange, nRange);
	else
		glOrtho(-nRange * w / h, nRange*w / h, -nRange, nRange, -nRange, nRange);
	// Ponowne ustawienie stosu macierzy rzutowania
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Menu(int value)
{
	switch (value)
	{
		//
	case GL_CULL_FACE:
		iCull = !iCull;
		glutSetWindowTitle("Stozek - GL_CULL_FACE");
		RenderScene();
		break;

		//
	case GL_DEPTH_TEST:
		iDepth = !iDepth;
		glutSetWindowTitle("Stozek - GL_DEPTH_TEST");
		RenderScene();
		break;

		//
	case GL_LINE:
		iOutline = true;
		glutSetWindowTitle("Stozek - GL_LINE");
		RenderScene();
		break;

		//
	case GL_FILL:
		iOutline = false;
		glutSetWindowTitle("Stozek - GL_LINE");
		RenderScene();
		break;

		// wyjęcie
	case EXIT:
		exit(0);
	}
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Triangles Constructing Fan Example");
	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(SpecialKeys);
	glutDisplayFunc(RenderScene);
	SetupRC();
	// utworzenie podmenu - Prymitywy
	int MenuPrimitive = glutCreateMenu(Menu);
	glutAddMenuEntry("GL_CULL_FACE", GL_CULL_FACE);
	glutAddMenuEntry("GL_DEPTH_TEST", GL_DEPTH_TEST);
	glutAddMenuEntry("Polygon mode LINE", GL_LINE);
	glutAddMenuEntry("Polygon mode FILL", GL_FILL);
	glutAddMenuEntry("Wyjecie", EXIT);

	// okrelenie przycisku myszki obsługujšcego menu podręczne
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();
	return 0;
}