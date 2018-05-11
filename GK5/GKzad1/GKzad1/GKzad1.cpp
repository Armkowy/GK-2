// lab5.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <GL/glut.h>
#include "glext.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "materials.h"
#include "colors.h"
#include <iostream>

bool rescale_normal = false;


enum
{
	BRASS,                // mosiądz
	BRONZE,               // brąz
	POLISHED_BRONZE,      // polerowany brąz
	CHROME,               // chrom
	COPPER,               // miedź
	POLISHED_COPPER,      // polerowana miedź
	GOLD,                 // złoto
	POLISHED_GOLD,        // polerowane złoto
	PEWTER,               // grafit (cyna z ołowiem)
	SILVER,               // srebro
	POLISHED_SILVER,      // polerowane srebro
	EMERALD,              // szmaragd
	JADE,                 // jadeit
	OBSIDIAN,             // obsydian
	PEARL,                // perła
	RUBY,                 // rubin
	TURQUOISE,            // turkus
	BLACK_PLASTIC,        // czarny plastik
	BLACK_RUBBER,         // czarna guma
	NORMALS_SMOOTH,       // jeden wektor normalny na wierzchołek
	NORMALS_FLAT,         // jeden wektor normalny na ścianę
	FULL_WINDOW,          // aspekt obrazu - całe okno
	ASPECT_1_1,           // aspekt obrazu 1:1
	GLOBAL_LIGHT_OPTION,
	REFLECTOR_OPTION,
	POINT_LIGHT_OPTION,
	EXIT                  // wyjście
};

int aspect = FULL_WINDOW;

const int N = 12;
const int VertexNumber = N + 2;
const int TrianglesNumber = N * 2;

GLfloat vertex[VertexNumber * 3];
GLint triangles[TrianglesNumber * 3];

#ifdef near
#undef near
#endif
#ifdef far
#undef far
#endif

// rozmiary bryły obcinania
const GLdouble left = -1.0;
const GLdouble right = 1.0;
const GLdouble bottom = -1.0;
const GLdouble top = 1.0;
const GLdouble near = 3.0;
const GLdouble far = 7.0;

// kąty obrotu bryly
GLfloat rotatex = 0.0;
GLfloat rotatey = 0.0;
//kąty obrotu światła
GLfloat light_rotatex = 0.0;
GLfloat light_rotatey = 0.0;
//Rodzaj Światła
GLint LightMode = GLOBAL_LIGHT_OPTION;

// właściwości materiału - domyślnie brąz
const GLfloat *ambient = BronzeAmbient;
const GLfloat *diffuse = BronzeDiffuse;
const GLfloat *specular = BronzeSpecular;
GLfloat shininess = BronzeShininess;

//Wektory normalne
int normals = NORMALS_FLAT;

// wskaźnik naciśnięcia lewego przycisku myszki
int button_state = GLUT_UP;
// poło¿enie kursora myszki
int button_x, button_y;

GLfloat ambient_light[4] =
{
	0.2,0.2,0.2,1.0
};

GLfloat spot_direction[3] =
{
	0.0,0.0,-1.0
};

// kąt odciêcia reflektora
GLfloat spot_cutoff = 180.0;

// wykładnik tłumienia kątowego reflektora
GLfloat spot_exponent = 128.0;

// stały współczynnik tłumienia światła
GLfloat constant_attenuation = 1.0;

// liniowy współczynnik tłumienia światła
GLfloat linear_attenuation = 0.0;

// kwadratowy współczynnik tłumienia światła
GLfloat quadratic_attenuation = 0.0;

GLfloat light_position[4] =
{
	0.0,0.0,1.0,0.0
};

void GenerateVerticles(GLfloat* vertices, int Sides)
{
	for (auto i = 0; i < Sides; ++i)
	{
		vertices[i * 3 + 0] = sinf(i / static_cast<float>(Sides) * 2 * M_PI) / 2;
		vertices[i * 3 + 1] = -0.5f;
		vertices[i * 3 + 2] = cosf(i / static_cast<float>(Sides) * 2 * M_PI) / 2;
	}
	int l = Sides * 3;
	vertices[l + 0] = 0.f;
	vertices[l + 1] = -0.5f;
	vertices[l + 2] = 0.f;
	vertices[l + 3] = 0.f;
	vertices[l + 4] = 0.5f;
	vertices[l + 5] = 0.f;
}
void GenerateTriangles(GLint* triangles, int Sides)
{
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < Sides; ++j)
		{
			triangles[i*Sides * 3 + j * 3] = j;
			triangles[i*Sides * 3 + j * 3 + 1] = (j + 1) % Sides;
			triangles[i*Sides * 3 + j * 3 + 2] = Sides + i;
		}
	for (int i = 0; i < Sides * 2; ++i)
	{
		std::cout << triangles[i * 3] << " " << triangles[i * 3 + 1] << " " << triangles[i * 3 + 2] << std::endl;
	}
}

// obliczanie wektora normalnego dla wybranej ściany
void Normal(GLfloat *n, int i)
{
	GLfloat v1[3], v2[3];
	// obliczenie wektorów na podstawie współrzędnych wierzchołków trójkątów
	v1[0] = vertex[3 * triangles[3 * i + 1] + 0] - vertex[3 * triangles[3 * i + 0] + 0];
	v1[1] = vertex[3 * triangles[3 * i + 1] + 1] - vertex[3 * triangles[3 * i + 0] + 1];
	v1[2] = vertex[3 * triangles[3 * i + 1] + 2] - vertex[3 * triangles[3 * i + 0] + 2];
	v2[0] = vertex[3 * triangles[3 * i + 2] + 0] - vertex[3 * triangles[3 * i + 1] + 0];
	v2[1] = vertex[3 * triangles[3 * i + 2] + 1] - vertex[3 * triangles[3 * i + 1] + 1];
	v2[2] = vertex[3 * triangles[3 * i + 2] + 2] - vertex[3 * triangles[3 * i + 1] + 2];
	// obliczenie waktora normalnego przy pomocy iloczynu wektorowego
	n[0] = v1[1] * v2[2] - v1[2] * v2[1];
	n[1] = v1[2] * v2[0] - v1[0] * v2[2];
	n[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

// normalizacja wektora
void Normalize(GLfloat *v)
{
	// obliczenie długości wektora
	GLfloat d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	// normalizacja wektora
	if (d)
	{
		v[0] /= d;
		v[1] /= d;
		v[2] /= d;
	}
}

GLfloat scale = 1.0;
void Display()
{
	// kolor tła - zawartośæ bufora koloru
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// czyszczenie bufora koloru i bufora głębokości
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// wybór macierzy modelowania
	glMatrixMode(GL_MODELVIEW);

	// macierz modelowania = macierz jednostkowa
	glLoadIdentity();

	// przesunięcie układu współrzędnych obiektu do środka bryły odcinania
	glTranslatef(0, 0, -(near + far) / 2);

	// obroty obiektu
	glRotatef(rotatex, 1.0, 0, 0);
	glRotatef(rotatey, 0, 1.0, 0);

	// skalowanie obiektu - klawisze "+" i "-"
	glScalef(scale, scale, scale);

	//Normalne
	// włączenie testu bufora głębokości
	//glEnable(GL_DEPTH_TEST);

	// włączenie oświetlenia
	//glEnable(GL_LIGHTING);

	// włączenie światła GL_LIGHT0 z parametrami domyślnymi
	//glEnable(GL_LIGHT0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	if (LightMode == GLOBAL_LIGHT_OPTION)
	{
		// włączenie światła GL_LIGHT0
		glEnable(GL_LIGHT0);
		// włączenie automatycznej normalizacji wektorów normalnych
		//glEnable(GL_NORMALIZE);

		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_light);


		// włączenie testu bufora głębokości
		//glColor3fv(Brown);
	}
	else if (LightMode == REFLECTOR_OPTION)
	{
		// włączenie światła GL_LIGHT0
		glEnable(GL_LIGHT0);
		// włączenie automatycznej normalizacji wektorów normalnych
		glEnable(GL_NORMALIZE);

		// kąt odciêcia reflektora
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, spot_cutoff);
		// wykładnik tłumienia kątowego reflektora
		glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, spot_exponent);
		// stały współczynnik tłumienia światła,
		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, constant_attenuation);
		// liniowy współczynnik tłumienia światła
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, linear_attenuation);
		// kwadratowy współczynnik tłumienia światła
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, quadratic_attenuation);

		// zmiana położenia źródła światła jest wykonywana niezależnie
		// od obrotów obiektu, stąd odłożenie na stos macierzy modelowania
		glPushMatrix();

		// macierz modelowania = macierz jednostkowa
		glLoadIdentity();

		// przesuniêcie układu współrzêdnych źródła światła do środka bryły odcinania
		glTranslatef(0, 0, -(near + far) / 2);

		// obroty położenia źródła światła - klawisze kursora
		glRotatef(light_rotatex, 1.0, 0, 0);
		glRotatef(light_rotatey, 0, 1.0, 0);

		// przesuniêcie źródła światła
		glTranslatef(light_position[0], light_position[1], light_position[2]);

		// ustalenie pozycji źródła światła
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);

		// ustalenie kierunku reflektora
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);

		// odłożenie na stos zmiennych stanu związanych z oświetleniem sceny
		glPushAttrib(GL_LIGHTING_BIT);

		// wyłacznie źródła światła GL_LIGHT0
		glDisable(GL_LIGHT0);

		// materiał imitujący świecenie kuli na czerwono
		glMaterialfv(GL_FRONT, GL_EMISSION, Red);

		// narysowanie kuli położonej w początku źródła światła
		glutSolidSphere(0.1, 30, 20);

		// przywrócenie zmiennych stanu związanych z oświetleniem sceny
		glPopAttrib();

		// przywrócenie pierwotnej macierzy modelowania
		glPopMatrix();
	}
	else if (LightMode == POINT_LIGHT_OPTION)
	{
		glEnable(GL_LIGHT0);
		glEnable(GL_NORMALIZE);


		glRotatef(light_rotatex, 1.0, 0, 0);
		glRotatef(light_rotatey, 0, 1.0, 0);

		// ustalenie kierunku źródła światła
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	}


	// właściwości materiału
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);



	if (rescale_normal == true)
		glEnable(GL_RESCALE_NORMAL);
	else
		glEnable(GL_NORMALIZE);
	// początek definicji obiektu
	glBegin(GL_TRIANGLES);
	// generowanie obiektu gładkiego - jeden uśredniony
	// wektor normalny na wierzchołek
	if (normals == NORMALS_SMOOTH)
		for (int i = 0; i < TrianglesNumber; i++)
		{
			// obliczanie wektora normalnego dla pierwszego wierzchołka
			GLfloat n[3];
			n[0] = n[1] = n[2] = 0.0;

			// wyszukanie wszystkich ścian posiadających bie¿ący wierzchołek
			for (int j = 0; j < TrianglesNumber; j++)
				if (3 * triangles[3 * i + 0] == 3 * triangles[3 * j + 0] ||
					3 * triangles[3 * i + 0] == 3 * triangles[3 * j + 1] ||
					3 * triangles[3 * i + 0] == 3 * triangles[3 * j + 2])
				{
					// dodawanie wektorów normalnych poszczególnych ścian
					GLfloat nv[3];
					Normal(nv, j);
					n[0] += nv[0];
					n[1] += nv[1];
					n[2] += nv[2];
				}

			// uśredniony wektor normalny jest normalizowany tylko, gdy biblioteka
			// obsługuje automatyczne skalowania jednostkowych wektorów normalnych
			if (rescale_normal == true)
				Normalize(n);
			glNormal3fv(n);
			glVertex3fv(&vertex[3 * triangles[3 * i + 0]]);

			// obliczanie wektora normalnego dla drugiego wierzchołka
			n[0] = n[1] = n[2] = 0.0;

			// wyszukanie wszystkich ścian posiadających bie¿ący wierzchołek
			for (int j = 0; j < TrianglesNumber; j++)
				if (3 * triangles[3 * i + 1] == 3 * triangles[3 * j + 0] ||
					3 * triangles[3 * i + 1] == 3 * triangles[3 * j + 1] ||
					3 * triangles[3 * i + 1] == 3 * triangles[3 * j + 2])
				{
					// dodawanie wektorów normalnych poszczególnych ścian
					GLfloat nv[3];
					Normal(nv, j);
					n[0] += nv[0];
					n[1] += nv[1];
					n[2] += nv[2];
				}

			// uśredniony wektor normalny jest normalizowany tylko, gdy biblioteka
			// obsługuje automatyczne skalowania jednostkowych wektorów normalnych
			if (rescale_normal == true)
				Normalize(n);
			glNormal3fv(n);
			glVertex3fv(&vertex[3 * triangles[3 * i + 1]]);

			// obliczanie wektora normalnego dla trzeciego wierzchołka
			n[0] = n[1] = n[2] = 0.0;

			// wyszukanie wszystkich ścian posiadających bie¿ący wierzchołek
			for (int j = 0; j < TrianglesNumber; j++)
				if (3 * triangles[3 * i + 2] == 3 * triangles[3 * j + 0] ||
					3 * triangles[3 * i + 2] == 3 * triangles[3 * j + 1] ||
					3 * triangles[3 * i + 2] == 3 * triangles[3 * j + 2])
				{
					// dodawanie wektorów normalnych poszczególnych ścian
					GLfloat nv[3];
					Normal(nv, j);
					n[0] += nv[0];
					n[1] += nv[1];
					n[2] += nv[2];
				}

			// uśredniony wektor normalny jest normalizowany tylko, gdy biblioteka
			// obsługuje automatyczne skalowania jednostkowych wektorów normalnych
			if (rescale_normal == true)
				Normalize(n);
			glNormal3fv(n);
			glVertex3fv(&vertex[3 * triangles[3 * i + 2]]);
		}
	else

		// generowanie obiektu "płaskiego" - jeden wektor normalny na ścianę
		for (int i = 0; i < TrianglesNumber; i++)
		{
			GLfloat n[3];
			Normal(n, i);

			// uśredniony wektor normalny jest normalizowany tylko, gdy biblioteka
			// obsługuje automatyczne skalowania jednostkowych wektorów normalnych
			if (rescale_normal == true)
				Normalize(n);
			glNormal3fv(n);
			glVertex3fv(&vertex[3 * triangles[3 * i + 0]]);
			glVertex3fv(&vertex[3 * triangles[3 * i + 1]]);
			glVertex3fv(&vertex[3 * triangles[3 * i + 2]]);
		}

	// koniec definicji obiektu
	glEnd();

	if (LightMode == GLOBAL_LIGHT_OPTION)
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glDisable(GL_NORMALIZE);
		glDisable(GL_COLOR_MATERIAL);

	}
	// wyłączenie oświetlenia

	// wyłączenie obsługi właściwości materiałów

	// skierowanie poleceñ do wykonania
	glFlush();

	// zamiana buforów koloru
	glutSwapBuffers();
}

void Reshape(int width, int height)

{

	// obszar renderingu - całe okno
	glViewport(0, 0, width, height);

	// wybór macierzy rzutowania
	glMatrixMode(GL_PROJECTION);

	// macierz rzutowania = macierz jednostkowa
	glLoadIdentity();

	// parametry bryły obcinania
	if (aspect == ASPECT_1_1)
	{
		// wysokość okna większa od wysokości okna
		if (width < height && width > 0)
			glFrustum(left, right, bottom*height / width, top*height / width, near, far);
		else
			// szerokość okna większa lub równa wysokości okna
			if (width >= height && height > 0)
				glFrustum(left*width / height, right*width / height, bottom, top, near, far);
	}
	else
		glFrustum(left, right, bottom, top, near, far);

	// generowanie sceny 3D
	Display();

}

void Keyboard(unsigned char key, int x, int y)
{
	if (key == '+') scale += 0.05;
	else if (key == '-' && scale > 0.05) scale -= 0.05;

	if (LightMode == GLOBAL_LIGHT_OPTION)
	{
		// zmiana wartości składowej R
		if (key == 'R' && ambient_light[0] < 1.0)
			ambient_light[0] += 0.05;
		else
			if (key == 'r' && ambient_light[0] > -1.0)
				ambient_light[0] -= 0.05;

		// zmiana wartości składowej G
		if (key == 'G' && ambient_light[1] < 1.0)
			ambient_light[1] += 0.05;
		else
			if (key == 'g' && ambient_light[1] > -1.0)
				ambient_light[1] -= 0.05;

		// zmiana wartości składowej B
		if (key == 'B' && ambient_light[2] < 1.0)
			ambient_light[2] += 0.05;
		else
			if (key == 'b' && ambient_light[2] > -1.0)
				ambient_light[2] -= 0.05;
	}
	else if (LightMode == REFLECTOR_OPTION)
	{
		switch (key)
		{
			// klawisz "S" - zwiêkszenie kąta obciêcia reflektora
		case 'S':
			if (spot_cutoff == 90)
				spot_cutoff = 180;
			else
				if (spot_cutoff < 90)
					spot_cutoff++;
			break;

			// klawisz "s" - zmniejszenie kąta obciêcia reflektora
		case 's':
			if (spot_cutoff == 180)
				spot_cutoff = 90;
			else
				if (spot_cutoff > 0)
					spot_cutoff--;
			break;

			// klawisz "E" - zwiêkszenie wykładnika tłumienia kątowego reflektora
		case 'E':
			if (spot_exponent < 128)
				spot_exponent++;
			break;

			// klawisz "e" - zmniejszenie wykładnika tłumienia kątowego reflektora
		case 'e':
			if (spot_exponent > 0)
				spot_exponent--;
			break;

			// klawisz "C" - zwiêkszenie stałego współczynnika tłumienia światła
		case 'C':
			constant_attenuation += 0.1;
			break;

			// klawisz "c" - zmniejszenie stałego współczynnika tłumienia światła
		case 'c':
			if (constant_attenuation > 0)
				constant_attenuation -= 0.1;
			break;

			// klawisz "L" - zwiêkszenie liniowego współczynnika tłumienia światła
		case 'L':
			linear_attenuation += 0.1;
			break;

			// klawisz "l" - zmniejszenie liniowego współczynnika tłumienia światła
		case 'l':
			if (linear_attenuation > 0)
				linear_attenuation -= 0.1;
			break;

			// klawisz "Q" - zwiêkszenie kwadratowego współczynnika tłumienia światła
		case 'Q':
			quadratic_attenuation += 0.1;
			break;

			// klawisz "q" - zmniejszenie kwadratowego współczynnika tłumienia światła
		case 'q':
			if (quadratic_attenuation > 0)
				quadratic_attenuation -= 0.1;
			break;
		}
	}

	Display();
}

void SpecialKeys(int key, int x, int y)
{
	if (LightMode == GLOBAL_LIGHT_OPTION)
	{
		switch (key)
		{
			// kursor w lewo
		case GLUT_KEY_LEFT:
			rotatey -= 1;
			break;

			// kursor w górę
		case GLUT_KEY_UP:
			rotatex -= 1;
			break;

			// kursor w prawo
		case GLUT_KEY_RIGHT:
			rotatey += 1;
			break;

			// kursor w dół
		case GLUT_KEY_DOWN:
			rotatex += 1;
			break;
		}
	}
	else
	{
		switch (key)
		{
			// kursor w lewo
		case GLUT_KEY_LEFT:
			light_rotatey -= 5;
			break;

			// kursor w prawo
		case GLUT_KEY_RIGHT:
			light_rotatey += 5;
			break;

			// kursor w dół
		case GLUT_KEY_DOWN:
			light_rotatex += 5;
			break;

			// kursor w górê
		case GLUT_KEY_UP:
			light_rotatex -= 5;
			break;
		}
	}
	Display();
}

void MouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		// zapamiętanie stanu lewego przycisku myszki
		button_state = state;

		// zapamiętanie poło¿enia kursora myszki
		if (state == GLUT_DOWN)
		{
			button_x = x;
			button_y = y;
		}
	}
}

void MouseMotion(int x, int y)
{
	if (button_state == GLUT_DOWN)
	{
		rotatey += 30 * (right - left) / glutGet(GLUT_WINDOW_WIDTH) * (x - button_x);
		button_x = x;
		rotatex -= 30 * (top - bottom) / glutGet(GLUT_WINDOW_HEIGHT) * (button_y - y);
		button_y = y;
		glutPostRedisplay();
	}
}

void Menu(int value)
{


	switch (value)
	{

		// materiał - mosiądz

	case BRASS:

		ambient = BrassAmbient;

		diffuse = BrassDiffuse;

		specular = BrassSpecular;

		shininess = BrassShininess;

		Display();

		break;

		// materiał - brąz

	case BRONZE:

		ambient = BronzeAmbient;

		diffuse = BronzeDiffuse;

		specular = BronzeSpecular;

		shininess = BronzeShininess;

		Display();

		break;

		// materiał - polerowany brąz

	case POLISHED_BRONZE:

		ambient = PolishedBronzeAmbient;

		diffuse = PolishedBronzeDiffuse;

		specular = PolishedBronzeSpecular;

		shininess = PolishedBronzeShininess;

		Display();

		break;

		// materiał - chrom

	case CHROME:

		ambient = ChromeAmbient;

		diffuse = ChromeDiffuse;

		specular = ChromeSpecular;

		shininess = ChromeShininess;

		Display();

		break;

		// materiał - miedź

	case COPPER:

		ambient = CopperAmbient;

		diffuse = CopperDiffuse;

		specular = CopperSpecular;

		shininess = CopperShininess;

		Display();

		break;

		// materiał - polerowana miedź

	case POLISHED_COPPER:

		ambient = PolishedCopperAmbient;

		diffuse = PolishedCopperDiffuse;

		specular = PolishedCopperSpecular;

		shininess = PolishedCopperShininess;

		Display();

		break;

		// materiał - złoto

	case GOLD:

		ambient = GoldAmbient;

		diffuse = GoldDiffuse;

		specular = GoldSpecular;

		shininess = GoldShininess;

		Display();

		break;

		// materiał - polerowane złoto

	case POLISHED_GOLD:

		ambient = PolishedGoldAmbient;

		diffuse = PolishedGoldDiffuse;

		specular = PolishedGoldSpecular;

		shininess = PolishedGoldShininess;

		Display();

		break;

		// materiał - grafit (cyna z ołowiem)

	case PEWTER:

		ambient = PewterAmbient;

		diffuse = PewterDiffuse;

		specular = PewterSpecular;

		shininess = PewterShininess;

		Display();

		break;

		// materiał - srebro

	case SILVER:

		ambient = SilverAmbient;

		diffuse = SilverDiffuse;

		specular = SilverSpecular;

		shininess = SilverShininess;

		Display();

		break;

		// materiał - polerowane srebro

	case POLISHED_SILVER:

		ambient = PolishedSilverAmbient;

		diffuse = PolishedSilverDiffuse;

		specular = PolishedSilverSpecular;

		shininess = PolishedSilverShininess;

		Display();

		break;

		// materiał - szmaragd

	case EMERALD:

		ambient = EmeraldAmbient;

		diffuse = EmeraldDiffuse;

		specular = EmeraldSpecular;

		shininess = EmeraldShininess;

		Display();

		break;

		// materiał - jadeit

	case JADE:

		ambient = JadeAmbient;

		diffuse = JadeDiffuse;

		specular = JadeSpecular;

		shininess = JadeShininess;

		Display();

		break;

		// materiał - obsydian

	case OBSIDIAN:

		ambient = ObsidianAmbient;

		diffuse = ObsidianDiffuse;

		specular = ObsidianSpecular;

		shininess = ObsidianShininess;

		Display();

		break;

		// materiał - perła

	case PEARL:

		ambient = PearlAmbient;

		diffuse = PearlDiffuse;

		specular = PearlSpecular;

		shininess = PearlShininess;

		Display();

		break;

		// metariał - rubin

	case RUBY:

		ambient = RubyAmbient;

		diffuse = RubyDiffuse;

		specular = RubySpecular;

		shininess = RubyShininess;

		Display();

		break;

		// materiał - turkus

	case TURQUOISE:

		ambient = TurquoiseAmbient;

		diffuse = TurquoiseDiffuse;

		specular = TurquoiseSpecular;

		shininess = TurquoiseShininess;

		Display();

		break;

		// materiał - czarny plastik

	case BLACK_PLASTIC:

		ambient = BlackPlasticAmbient;

		diffuse = BlackPlasticDiffuse;

		specular = BlackPlasticSpecular;

		shininess = BlackPlasticShininess;

		Display();

		break;

		// materiał - czarna guma

	case BLACK_RUBBER:

		ambient = BlackRubberAmbient;

		diffuse = BlackRubberDiffuse;

		specular = BlackRubberSpecular;

		shininess = BlackRubberShininess;

		Display();

		break;

		// wektory normalne - GLU_SMOOTH

	case NORMALS_SMOOTH:

		normals = NORMALS_SMOOTH;

		Display();

		break;

		// wektory normalne - GLU_FLAT

	case NORMALS_FLAT:

		normals = NORMALS_FLAT;

		Display();

		break;

		// obszar renderingu - całe okno

	case FULL_WINDOW:

		aspect = FULL_WINDOW;

		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

		break;

		// obszar renderingu - aspekt 1:1

	case ASPECT_1_1:

		aspect = ASPECT_1_1;

		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

		break;

	case GLOBAL_LIGHT_OPTION:
		LightMode = GLOBAL_LIGHT_OPTION;
		Display();
		break;
	case REFLECTOR_OPTION:
		LightMode = REFLECTOR_OPTION;
		Display();
		break;
	case POINT_LIGHT_OPTION:
		LightMode = POINT_LIGHT_OPTION;
		Display();
		break;
		// wyjście

	case EXIT:

		exit(0);
	}
}


void ExtensionSetup()
{

	// pobranie numeru wersji biblioteki OpenGL

	const char *version = (char*)glGetString(GL_VERSION);

	// odczyt wersji OpenGL

	int major = 0, minor = 0;

	if (sscanf_s(version, "%d.%d", &major, &minor) != 2)

	{

#ifndef WIN32

		printf("Błędny format wersji OpenGL\n");

#else

		printf("Bledny format wersji OpenGL\n");

#endif

		exit(0);

	}

	// sprawdzenie czy jest co najmniej wersja 1.2

	if (major > 1 || minor >= 2)

		rescale_normal = true;

	else

		// sprawdzenie czy jest obsługiwane rozszerzenie EXT_rescale_normal

		if (glutExtensionSupported("GL_EXT_rescale_normal"))

			rescale_normal = true;

}

int main(int argc, char *argv[])
{
	GenerateVerticles(vertex, N);
	GenerateTriangles(triangles, N);
	// inicjalizacja biblioteki GLUT
	glutInit(&argc, argv);
	// inicjalizacja bufora ramki
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	// rozmiary głównego okna programu
	glutInitWindowSize(500, 500);
	// utworzenie głównego okna programu
	glutCreateWindow("Stozek");
	// dołączenie funkcji generującej scenę 3D
	glutDisplayFunc(Display);
	// dołączenie funkcji wywoływanej przy zmianie rozmiaru okna
	glutReshapeFunc(Reshape);
	// dołączenie funkcji obsługi klawiatury
	glutKeyboardFunc(Keyboard);
	// dołączenie funkcji obsługi klawiszy funkcyjnych i klawiszy kursora
	glutSpecialFunc(SpecialKeys);
	// obsługa przycisków myszki
	glutMouseFunc(MouseButton);
	// obsługa ruchu kursora myszki
	glutMotionFunc(MouseMotion);
	// utworzenie menu podręcznego
	glutCreateMenu(Menu);

	int MenuMaterial = glutCreateMenu(Menu);

#ifdef WIN32

	glutAddMenuEntry("Mosiądz", BRASS);

	glutAddMenuEntry("Brąz", BRONZE);

	glutAddMenuEntry("Polerowany brąz", POLISHED_BRONZE);

	glutAddMenuEntry("Chrom", CHROME);

	glutAddMenuEntry("Miedź", COPPER);

	glutAddMenuEntry("Polerowana miedź", POLISHED_COPPER);

	glutAddMenuEntry("Złoto", GOLD);

	glutAddMenuEntry("Polerowane złoto", POLISHED_GOLD);

	glutAddMenuEntry("Grafit (cyna z ołowiem)", PEWTER);

	glutAddMenuEntry("Srebro", SILVER);

	glutAddMenuEntry("Polerowane srebro", POLISHED_SILVER);

	glutAddMenuEntry("Szmaragd", EMERALD);

	glutAddMenuEntry("Jadeit", JADE);

	glutAddMenuEntry("Obsydian", OBSIDIAN);

	glutAddMenuEntry("Perła", PEARL);

	glutAddMenuEntry("Rubin", RUBY);

	glutAddMenuEntry("Turkus", TURQUOISE);

	glutAddMenuEntry("Czarny plastik", BLACK_PLASTIC);

	glutAddMenuEntry("Czarna guma", BLACK_RUBBER);

#else

	glutAddMenuEntry("Mosiadz", BRASS);

	glutAddMenuEntry("Braz", BRONZE);

	glutAddMenuEntry("Polerowany braz", POLISHED_BRONZE);

	glutAddMenuEntry("Chrom", CHROME);

	glutAddMenuEntry("Miedz", COPPER);

	glutAddMenuEntry("Polerowana miedz", POLISHED_COPPER);

	glutAddMenuEntry("Zloto", GOLD);

	glutAddMenuEntry("Polerowane zloto", POLISHED_GOLD);

	glutAddMenuEntry("Grafit (cyna z ołowiem)", PEWTER);

	glutAddMenuEntry("Srebro", SILVER);

	glutAddMenuEntry("Polerowane srebro", POLISHED_SILVER);

	glutAddMenuEntry("Szmaragd", EMERALD);

	glutAddMenuEntry("Jadeit", JADE);

	glutAddMenuEntry("Obsydian", OBSIDIAN);

	glutAddMenuEntry("Perla", PEARL);

	glutAddMenuEntry("Rubin", RUBY);

	glutAddMenuEntry("Turkus", TURQUOISE);

	glutAddMenuEntry("Czarny plastik", BLACK_PLASTIC);

	glutAddMenuEntry("Czarna guma", BLACK_RUBBER);

#endif
	int MenuLight = glutCreateMenu(Menu);
	glutAddMenuEntry("Swiatlo Globalne", GLOBAL_LIGHT_OPTION);
	glutAddMenuEntry("Reflektor", REFLECTOR_OPTION);
	glutAddMenuEntry("Punktowe", POINT_LIGHT_OPTION);

	// utworzenie podmenu - Wektory normalne
	int MenuNormals = glutCreateMenu(Menu);

#ifndef WIN32

	glutAddMenuEntry("Jeden wektor normalny na wierzcholek", NORMALS_SMOOTH);

	glutAddMenuEntry("Jeden wektor normalny na sciane", NORMALS_FLAT);

#else

	glutAddMenuEntry("Jeden wektor normalny na wierzchołek", NORMALS_SMOOTH);

	glutAddMenuEntry("Jeden wektor normalny na ścianę", NORMALS_FLAT);

#endif

	// utworzenie podmenu - aspekt obrazu

	int MenuAspect = glutCreateMenu(Menu);

#ifndef WIN32

	glutAddMenuEntry("Aspekt obrazu - całe okno", FULL_WINDOW);

#else

	glutAddMenuEntry("Aspekt obrazu - cale okno", FULL_WINDOW);

#endif

	glutAddMenuEntry("Aspekt obrazu 1:1", ASPECT_1_1);

	// menu główne

	glutCreateMenu(Menu);

#ifdef WIN32

	glutAddSubMenu("Materiał", MenuMaterial);

#else

	glutAddSubMenu("Material", MenuMaterial);

#endif
	glutAddSubMenu("Oswietlenie", MenuLight);

	glutAddSubMenu("Wektory normalne", MenuNormals);

	glutAddSubMenu("Aspekt obrazu", MenuAspect);

#ifndef WIN32

	glutAddMenuEntry("Wyjście", EXIT);

#else

	glutAddMenuEntry("Wyjscie", EXIT);

#endif

	glutAttachMenu(GLUT_RIGHT_BUTTON);

	ExtensionSetup();


	// wprowadzenie programu do obsługi pętli komunikatów
	glutMainLoop();

	return 0;
}