#include <stdlib.h>
#include <stdio.h>
#include <freeglut.h>
#include <math.h>

#define windowWidth 960
#define windowHeight 540
#define PI 3.141592653589793
#define GRID_SIZE 100 // size of width and height of grid in verticies. Default 100. That is, 100x100
#define C 5 // distance between verticies for the grid
#define PROPELLER_SPEED 10
#define ITERATIONS 8 // for mountain generation
#define MOUNTAIN_HEIGHT 9 // height of mountains
#define HEIGHT_FOR_SNOW 11.0
#define HEIGHT_FOR_GREY 10.0
#define HEIGHT_FOR_GRASS 5.0
#define HEIGHT_FOR_DENSE_GRASS 0.0
#define MAX_X1 40 // width of first mountain
#define MAX_Z1 55 // length of first mountain
#define MAX_X2 40 // width of second mountain
#define MAX_Z2 40 // length of second mountain
#define MAX_X3 45 // width of third mountain
#define MAX_Z3 50 // length of third mountain

typedef GLfloat point3[3];
typedef enum { false, true } bool; // a boolean in C
bool isWireframeEnabled = false;
bool showSkyAndSea = true;
bool isFullScreen = false;
bool isFogEnabled = true;
bool isMountainsEnabled = false;
bool cameraUp = false;
bool cameraDown = false;
bool increaseSpeed = false;
bool decreaseSpeed = false;

GLfloat cameraVerticalConstant = 0.2;
GLfloat planeSpeed = 0.02;
GLfloat mouseDirectionAngle = 0.0;
GLfloat x = 0.0;
GLfloat y = 0.0;
GLfloat z = 0.0;

GLfloat verticies[10000][3] = { { 0 } };
GLfloat normals[10000][3] = { { 0 } };
GLint faces[6000][18] = { { 0 } }; 
GLint j = -1;

GLfloat propellerTheta = 0.0;
GLfloat propellerVerticies[7000][3] = { { 0 } };
GLfloat propellerNormals[7000][3] = { { 0 } };
GLint propellerFaces[2000][14] = { { 0 } };
GLint v = -1;

GLfloat colors[4][4] = { {1.0, 1.0, 0.0, 1.0}, {0.0, 0.0, 0.0, 1.0}, {1.0, 0.0, 1.0, 1.0}, {0.0, 0.0, 1.0, 1.0} }; // yell, black, purp, blue 
GLint colorPattern[33] = { 0, 0, 0, 0, 1, 1, 2, 3, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0 };
GLint subObjectIndexes[33];
GLint k = 1;

GLfloat upVector[3] = { 0.0, 1.0, 0.0 };
GLfloat cameraPosition[3] = { -91.5, 15.5, 0.0 };
GLfloat planePosition[3] = { -90.0, 15.0, 0.0 };
GLfloat forwardVector[3] = { 1.0, 0.0, 0.0 };
GLfloat theta = 0.0;

point3 mGrid1[MAX_X1][MAX_Z1]; // 2D array of point3s for first mountain, this also includes vertex normals
point3 mGrid2[MAX_X2][MAX_Z2]; // 2D array of point3s for second mountain, this also includes vertex normals
point3 mGrid3[MAX_X3][MAX_Z3]; // 2D array of point3s for third mountain, this also includes vertex normals

GLfloat red[] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat green[] = { 0.0, 1.0, 0.0, 1.0 };
GLfloat darkGreen[] = { 0.0, 0.4, 0.0, 1.0 };
GLfloat blue[] = { 0.0, 0.0, 1.0, 1.0 };
GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat grey[] = { 0.3, 0.3, 0.3, 1.0 };
GLfloat cessnaShine = 20.0; // shininess for cessna
GLfloat mountainShine = 25.0; // shininess for mountains

GLuint seaTexture;
GLuint skyTexture;

GLuint createTexture(const char *img, int width, int height)
{
	GLuint texture = 0;
	BYTE * data = NULL;
	FILE * file;

	fopen_s(&file, img, "rb");

	if (&file == NULL) return 0;
	data = (BYTE*)malloc(width * height * 3);

	fread(data, width * height * 3, 1, file);
	fclose(file);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
 
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_BGRA_EXT, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_TEXTURE_ENV_COLOR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
	free(data);

	return texture;

}

void DrawSky()
{
	glPushMatrix();
	GLUquadricObj *quadric;
	quadric = gluNewQuadric();
	glEnable(GL_TEXTURE_2D);
	if (isWireframeEnabled)
		gluQuadricDrawStyle(quadric, GLU_LINE);
	else
		gluQuadricDrawStyle(quadric, GLU_FILL);
	gluQuadricNormals(quadric, GLU_SMOOTH);
	glBindTexture(GL_TEXTURE_2D, skyTexture);
	gluQuadricTexture(quadric, GL_TRUE);
	glTranslatef(0.0, -1.0, 0.0);
	glRotatef(90.0, -1.0, 0.0, 0.0);
	gluCylinder(quadric, 249.5, 249.5, 150.0, 100.0, 100.0);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void DrawSea()
{
	glPushMatrix();
	if (isFogEnabled)
		glEnable(GL_FOG);
	GLUquadricObj *quadric;
	quadric = gluNewQuadric();
	glEnable(GL_TEXTURE_2D);
	if (isWireframeEnabled)
		gluQuadricDrawStyle(quadric, GLU_LINE);
	else
		gluQuadricDrawStyle(quadric, GLU_FILL);
	gluQuadricNormals(quadric, GLU_SMOOTH);
	glBindTexture(GL_TEXTURE_2D, seaTexture);
	gluQuadricTexture(quadric, GL_TRUE);
	glTranslatef(0.0, 0.1, 0.0);
	glRotatef(90.0, -1.0, 0.0, 0.0);
	gluDisk(quadric, 0.0, 250.0, 100.0, 100.0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_FOG);
	glPopMatrix();
}

void DrawOriginLines()
{
	glPushMatrix();
	glLoadIdentity();

	glLineWidth(5.0);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
	glBegin(GL_LINES);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(10.0, 0.0, 0.0);
	glEnd();

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green);
	glBegin(GL_LINES);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 10.0, 0.0);
	glEnd();

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, blue);
	glBegin(GL_LINES);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 10.0);
	glEnd();

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
	GLUquadric *quad;
	quad = gluNewQuadric();
	gluSphere(quad, 0.1, 10.0, 10.0);

	glPopMatrix();
}

void DrawGrid()
{
	glPushMatrix();
	glLoadIdentity();

	if (isWireframeEnabled) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		DrawOriginLines();
	}
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glLineWidth(1.0);
	GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);

	GLfloat x = 0.0;
	GLfloat z = 0.0;
	for (x = -((GRID_SIZE / 2) * C); x < (GRID_SIZE / 2) * C; x += C) {
		for (z = -((GRID_SIZE / 2) * C); z < (GRID_SIZE / 2) * C; z += C) {
			glBegin(GL_QUADS);
				glVertex3f(x, 0.0, z);
				glVertex3f(x + C, 0.0, z);
				glVertex3f(x + C, 0.0, z + C);
				glVertex3f(x, 0.0, z + C);
			glEnd();
		}
	}

	glPopMatrix();
}

void DrawCessna(GLfloat propeller_x_offset, GLfloat propeller_y_offset)
{
	GLint colorIndex = 0;
	int l = 0;

	// get angle between forward vector and origin (to keep plane pointed forward on user's screen)
	GLfloat angle = -(atan2f(forwardVector[2], forwardVector[0]) * (180 / PI));

	// transform & rotate plane
	glTranslatef(x, planePosition[1] + y, z);
	glRotatef(angle, 0.0, 1.0, 0.0);
	glRotatef(mouseDirectionAngle * (50 / PI), 1.0, 0.0, 0.0);
	glRotatef(180.0, 0.0, 1.0, 0.0);
	glScalef(0.8, 0.8, 0.8);

	// draw cessna
	for (int a = 0; a < j; a++) {

		// handle colors
		if (subObjectIndexes[l] == a) {
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, colors[colorPattern[colorIndex]]);
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, cessnaShine);
			colorIndex++;
			l++;
		}

		glBegin(GL_POLYGON);
		for (int b = 0; b < 18; b++) {
			if (faces[a][b] != 0) {
				glNormal3f(normals[faces[a][b] - 1][0], normals[faces[a][b] - 1][1], normals[faces[a][b] - 1][2]);
				glVertex3f(verticies[faces[a][b] - 1][0], verticies[faces[a][b] - 1][1], verticies[faces[a][b] - 1][2]);
			}
			else {
				break;
			}
		}
		glEnd();
	}

	// draw propeller #1
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(x + propeller_x_offset, planePosition[1] + y + propeller_y_offset, z);
	glRotatef(angle, 0.0, 1.0, 0.0);
	glRotatef(mouseDirectionAngle * (50 / PI), 1.0, 0.0, 0.0);
	glRotatef(180.0, 0.0, 1.0, 0.0);
	glScalef(0.8, 0.8, 0.8);
	glTranslatef(-0.55, -0.15, 0.35); // moving propeller to origin doesn't make it center to origin, this is for correction
	glRotatef(propellerTheta, 1.0, 0.0, 0.0);
	glTranslatef(0.55, 0.15, -0.35); // moving propeller to origin doesn't make it center to origin, this is for correction
	for (int a = 0; a < v; a++) {
		glBegin(GL_POLYGON);
		for (int b = 0; b < 14; b++) {
			if (propellerFaces[a][b] != 0) {
				glNormal3f(propellerNormals[propellerFaces[a][b] - 1][0], propellerNormals[propellerFaces[a][b] - 1][1], propellerNormals[propellerFaces[a][b] - 1][2]);
				glVertex3f(propellerVerticies[propellerFaces[a][b] - 1][0], propellerVerticies[propellerFaces[a][b] - 1][1], propellerVerticies[propellerFaces[a][b] - 1][2]);
			}
			else {
				break;
			}
		}
		glEnd();
	}
	glPopMatrix();

	// draw propeller #2
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(x + propeller_x_offset, planePosition[1] + y + propeller_y_offset, z);
	glRotatef(angle, 0.0, 1.0, 0.0);
	glRotatef(mouseDirectionAngle * (50 / PI), 1.0, 0.0, 0.0);
	glRotatef(180.0, 0.0, 1.0, 0.0);
	glScalef(0.8, 0.8, 0.8);
	glTranslatef(0.0, 0.0, -0.7); // offset from other propeller
	glTranslatef(-0.55, -0.15, 0.35); // moving propeller to origin doesn't make it center to origin, this is for correction
	glRotatef(propellerTheta, 1.0, 0.0, 0.0);
	glTranslatef(0.55, 0.15, -0.35); // moving propeller to origin doesn't make it center to origin, this is for correction
	for (int a = 0; a < v; a++) {
		glBegin(GL_POLYGON);
		for (int b = 0; b < 14; b++) {
			if (propellerFaces[a][b] != 0) {
				glNormal3f(propellerNormals[propellerFaces[a][b] - 1][0], propellerNormals[propellerFaces[a][b] - 1][1], propellerNormals[propellerFaces[a][b] - 1][2]);
				glVertex3f(propellerVerticies[propellerFaces[a][b] - 1][0], propellerVerticies[propellerFaces[a][b] - 1][1], propellerVerticies[propellerFaces[a][b] - 1][2]);
			}
			else {
				break;
			}
		}
		glEnd();
	}
	glPopMatrix();
}

// init default values for mountain grid arrays
void InitMountainGrids()
{
	for (GLint x = 0; x < MAX_X1; x++) {
		for (GLint z = 0; z < MAX_Z1; z++) {
			mGrid1[x][z][0] = x;
			mGrid1[x][z][1] = 0.1;
			mGrid1[x][z][2] = z;
		}
	}

	for (GLint x = 0; x < MAX_X2; x++) {
		for (GLint z = 0; z < MAX_Z2; z++) {
			mGrid2[x][z][0] = x;
			mGrid2[x][z][1] = 0.1;
			mGrid2[x][z][2] = z;
		}
	}

	for (GLint x = 0; x < MAX_X3; x++) {
		for (GLint z = 0; z < MAX_Z3; z++) {
			mGrid3[x][z][0] = x;
			mGrid3[x][z][1] = 0.1;
			mGrid3[x][z][2] = z;
		}
	}
}

// return a randomly generated number
GLint GetRand()
{
	return (rand() % MOUNTAIN_HEIGHT);
}

// build the first mountain into local arrays
void BuildMountain1(GLint x1, GLint z1, GLint x2, GLint z2, GLfloat div, GLint level)
{
	if (level < 1)
		return;

	// diamong step
	for (GLint i = x1 + level; i < x2; i += level) {
		for (GLint n = z1 + level; n < z2; n += level) {
			GLfloat a = mGrid1[i - level][n - level][1];
			GLfloat b = mGrid1[i][n - level][1];
			GLfloat c = mGrid1[i - level][n][1];
			GLfloat d = mGrid1[i][n][1];
			GLfloat e = ((a + b + c + d) / 4) + GetRand() * div;

			mGrid1[i - level / 2][n - level / 2][1] = e;
		}
	}

	// square step
	for (GLint i = x1 + 2 * level; i < x2; i += level) {
		for (GLint n = z1 + 2 * level; n < z2; n += level) {
			GLfloat a = mGrid1[i - level][n - level][1];
			GLfloat b = mGrid1[i][n - level][1];
			GLfloat c = mGrid1[i - level][n][1];
			GLfloat d = mGrid1[i][n][1];
			GLfloat e = mGrid1[i - level / 2][n - level / 2][1];

			GLfloat f = (a + c + e + mGrid1[i - 3 * level / 2][n - level / 2][1]) / 4 + GetRand() * div;
			mGrid1[i - level][n - level / 2][1] = f;

			GLfloat g = (a + b + e + mGrid1[i - level / 2][n - 3 * level / 2][1]) / 4 + GetRand() * div;
			mGrid1[i - level / 2][n - level][1] = g;
		}
	}

	// recurse again
	BuildMountain1(x1, z1, x2, z2, div / 2, level / 2);

}

// build the second mountain into local arrays
void BuildMountain2(GLint x1, GLint z1, GLint x2, GLint z2, GLfloat div, GLint level)
{
	if (level < 1)
		return;

	// diamong step
	for (GLint i = x1 + level; i < x2; i += level) {
		for (GLint n = z1 + level; n < z2; n += level) {
			GLfloat a = mGrid2[i - level][n - level][1];
			GLfloat b = mGrid2[i][n - level][1];
			GLfloat c = mGrid2[i - level][n][1];
			GLfloat d = mGrid2[i][n][1];
			GLfloat e = ((a + b + c + d) / 4) + GetRand() * div;

			mGrid2[i - level / 2][n - level / 2][1] = e;
		}
	}

	// square step
	for (GLint i = x1 + 2 * level; i < x2; i += level) {
		for (GLint n = z1 + 2 * level; n < z2; n += level) {
			GLfloat a = mGrid2[i - level][n - level][1];
			GLfloat b = mGrid2[i][n - level][1];
			GLfloat c = mGrid2[i - level][n][1];
			GLfloat d = mGrid2[i][n][1];
			GLfloat e = mGrid2[i - level / 2][n - level / 2][1];

			GLfloat f = (a + c + e + mGrid2[i - 3 * level / 2][n - level / 2][1]) / 4 + GetRand() * div;
			mGrid2[i - level][n - level / 2][1] = f;

			GLfloat g = (a + b + e + mGrid2[i - level / 2][n - 3 * level / 2][1]) / 4 + GetRand() * div;
			mGrid2[i - level / 2][n - level][1] = g;
		}
	}

	// recurse again
	BuildMountain2(x1, z1, x2, z2, div / 2, level / 2);

}

// build the third mountain into local arrays
void BuildMountain3(GLint x1, GLint z1, GLint x2, GLint z2, GLfloat div, GLint level)
{
	if (level < 1)
		return;

	// diamong step
	for (GLint i = x1 + level; i < x2; i += level) {
		for (GLint n = z1 + level; n < z2; n += level) {
			GLfloat a = mGrid3[i - level][n - level][1];
			GLfloat b = mGrid3[i][n - level][1];
			GLfloat c = mGrid3[i - level][n][1];
			GLfloat d = mGrid3[i][n][1];
			GLfloat e = ((a + b + c + d) / 4) + GetRand() * div;

			mGrid3[i - level / 2][n - level / 2][1] = e;
		}
	}

	// square step
	for (GLint i = x1 + 2 * level; i < x2; i += level) {
		for (GLint n = z1 + 2 * level; n < z2; n += level) {
			GLfloat a = mGrid3[i - level][n - level][1];
			GLfloat b = mGrid3[i][n - level][1];
			GLfloat c = mGrid3[i - level][n][1];
			GLfloat d = mGrid3[i][n][1];
			GLfloat e = mGrid3[i - level / 2][n - level / 2][1];

			GLfloat f = (a + c + e + mGrid3[i - 3 * level / 2][n - level / 2][1]) / 4 + GetRand() * div;
			mGrid3[i - level][n - level / 2][1] = f;

			GLfloat g = (a + b + e + mGrid3[i - level / 2][n - 3 * level / 2][1]) / 4 + GetRand() * div;
			mGrid3[i - level / 2][n - level][1] = g;
		}
	}

	// recurse again
	BuildMountain3(x1, z1, x2, z2, div / 2, level / 2);

}

// draw the mountains on display from local arrays
void DrawMountains()
{
	if (isWireframeEnabled)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// draw first mountain
	glPushMatrix();
	glRotatef(-10.0, 0.0, 0.0, 1.0);
	glTranslatef(5.0, -2.0, 10.0);
	for (GLint z = 0; z < MAX_Z1 - 1; z++) {
		for (GLint x = 0; x < MAX_X1 - 1; x++) {
			// mountain colors
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mountainShine);
			if (mGrid1[x][z][1] >= HEIGHT_FOR_SNOW)
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
			else if (mGrid1[x][z][1] >= HEIGHT_FOR_GREY && mGrid1[x][z][1] < HEIGHT_FOR_SNOW)
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, grey);
			else if (mGrid1[x][z][1] >= HEIGHT_FOR_GRASS && mGrid1[x][z][1] < HEIGHT_FOR_GREY)
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green);
			else if (mGrid1[x][z][1] >= HEIGHT_FOR_DENSE_GRASS && mGrid1[x][z][1] < HEIGHT_FOR_GRASS)
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, darkGreen);

			// draw verticies and assign normals
			glBegin(GL_POLYGON);
				glNormal3f(mGrid1[x][z][0], mGrid1[x][z][1], mGrid1[x][z][2]);
				glVertex3f(mGrid1[x][z][0], mGrid1[x][z][1], mGrid1[x][z][2]);
				glNormal3f(mGrid1[x + 1][z][0], mGrid1[x + 1][z][1], mGrid1[x + 1][z][2]);
				glVertex3f(mGrid1[x + 1][z][0], mGrid1[x + 1][z][1], mGrid1[x + 1][z][2]);
				glNormal3f(mGrid1[x + 1][z + 1][0], mGrid1[x + 1][z + 1][1], mGrid1[x + 1][z + 1][2]);
				glVertex3f(mGrid1[x + 1][z + 1][0], mGrid1[x + 1][z + 1][1], mGrid1[x + 1][z + 1][2]);
				glNormal3f(mGrid1[x][z + 1][0], mGrid1[x][z + 1][1], mGrid1[x][z + 1][2]);
				glVertex3f(mGrid1[x][z + 1][0], mGrid1[x][z + 1][1], mGrid1[x][z + 1][2]);
			glEnd();
		}
	}
	glPopMatrix();

	// draw second mountain
	glPushMatrix();
	glTranslatef(25.0, -1.0, 40.0);
	glRotatef(-10.0, 0.0, 0.0, 1.0);
	for (GLint z = 0; z < MAX_Z2 - 1; z++) {
		for (GLint x = 0; x < MAX_X2 - 1; x++) {
			// mountain colors
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mountainShine);
			if (mGrid1[x][z][1] > HEIGHT_FOR_SNOW)
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
			else
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green);

			// draw verticies and assign normals
			glBegin(GL_POLYGON);
				glNormal3f(mGrid2[x][z][0], mGrid2[x][z][1], mGrid2[x][z][2]);
				glVertex3f(mGrid2[x][z][0], mGrid2[x][z][1], mGrid2[x][z][2]);
				glNormal3f(mGrid2[x + 1][z][0], mGrid2[x + 1][z][1], mGrid2[x + 1][z][2]);
				glVertex3f(mGrid2[x + 1][z][0], mGrid2[x + 1][z][1], mGrid2[x + 1][z][2]);
				glNormal3f(mGrid2[x + 1][z + 1][0], mGrid2[x + 1][z + 1][1], mGrid2[x + 1][z + 1][2]);
				glVertex3f(mGrid2[x + 1][z + 1][0], mGrid2[x + 1][z + 1][1], mGrid2[x + 1][z + 1][2]);
				glNormal3f(mGrid2[x][z + 1][0], mGrid2[x][z + 1][1], mGrid2[x][z + 1][2]);
				glVertex3f(mGrid2[x][z + 1][0], mGrid2[x][z + 1][1], mGrid2[x][z + 1][2]);
			glEnd();
		}
	}
	glPopMatrix();

	// draw third mountain
	glPushMatrix();
	glTranslatef(80.0, -0.5, 0.0);
	glRotatef(-10.0, 0.0, 0.0, 1.0);
	glRotatef(45.0, 0.0, 1.0, 0.0);
	for (GLint z = 0; z < MAX_Z3 - 1; z++) {
		for (GLint x = 0; x < MAX_X3 - 1; x++) {
			// mountain colors
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mountainShine);
			if (mGrid1[x][z][1] > HEIGHT_FOR_SNOW)
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
			else
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green);

			// draw verticies and assign normals
			glBegin(GL_POLYGON);
				glNormal3f(mGrid3[x][z][0], mGrid3[x][z][1], mGrid3[x][z][2]);
				glVertex3f(mGrid3[x][z][0], mGrid3[x][z][1], mGrid3[x][z][2]);
				glNormal3f(mGrid3[x + 1][z][0], mGrid3[x + 1][z][1], mGrid3[x + 1][z][2]);
				glVertex3f(mGrid3[x + 1][z][0], mGrid3[x + 1][z][1], mGrid3[x + 1][z][2]);
				glNormal3f(mGrid3[x + 1][z + 1][0], mGrid3[x + 1][z + 1][1], mGrid3[x + 1][z + 1][2]);
				glVertex3f(mGrid3[x + 1][z + 1][0], mGrid3[x + 1][z + 1][1], mGrid3[x + 1][z + 1][2]);
				glNormal3f(mGrid3[x][z + 1][0], mGrid3[x][z + 1][1], mGrid3[x][z + 1][2]);
				glVertex3f(mGrid3[x][z + 1][0], mGrid3[x][z + 1][1], mGrid3[x][z + 1][2]);
			glEnd();
		}
	}
	glPopMatrix();

}

void myDisplay()
{
	// clear the screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// display grid with origin axis
	DrawGrid();

	// display sea & sky
	if (showSkyAndSea) {
		DrawSea();
		DrawSky();
	}

	// display mountains
	if (isMountainsEnabled)
		DrawMountains();

	// change into projection mode so that we can change the camera properties
	glMatrixMode(GL_PROJECTION);

	// load the identity matrix into the projection matrix
	glLoadIdentity();

	// set camera to perspective and position and look direction
	gluPerspective(75.0, 1.77777778, 0.01, 600.0);
	gluLookAt(x - forwardVector[0], 15.5 + y, z - forwardVector[2], x, 15.0 + y, z, upVector[0], upVector[1], upVector[2]);
	//gluLookAt(5.0, 100.0, 30.0, 25.0, 10.0, 25.0, upVector[0], upVector[1], upVector[2]);

	// set light position
	GLfloat position[] = { 90.0, 100.0, 55.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	// back to matrix mode
	glMatrixMode(GL_MODELVIEW);

	// draw middle cessna
	glPushMatrix();
	glLoadIdentity();
	DrawCessna(0.0, 0.0);
	glPopMatrix();

	// draw autopilot on the left
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(-1.5, 0.5, 0.0);
	DrawCessna(-1.5, 0.5);
	glPopMatrix();

	// draw autopilot on the right
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(1.5, 0.5, 0.0);
	DrawCessna(1.5, 0.5);
	glPopMatrix();


	// swap the drawing buffers
	glutSwapBuffers();
}

void printControls()
{
	printf("------ Scene Controls ------\n");
	printf("f:	toggle fullscreen\n");
	printf("g:	toggle fog\n");
	printf("m:	toggle mountains\n");
	printf("s:	toggle sea & sky\n");
	printf("w:	toggle wire frame\n");
	printf("q:	exit game\n\n");

	printf("------ Camera Controls ------\n");
	printf("Page Up:	faster\n");
	printf("Page Down:	slower\n");
	printf("Up Arrow:	move up\n");
	printf("Down Arrow:	move down\n");
	printf("Mouse Right:	move right\n");
	printf("Mouse Left:	move left\n");
}

void myIdle()
{
	// calculate the forward vector
	theta += mouseDirectionAngle;
	forwardVector[0] = cos(theta * 0.005);
	forwardVector[2] = sin(theta * 0.005);

	// calculate new position of plane
	x += (forwardVector[0] * planeSpeed);
	z += (forwardVector[2] * planeSpeed);

	// calculate rotation for cessna propellers
	propellerTheta += PROPELLER_SPEED;

	if (increaseSpeed) {
		planeSpeed += 0.006;
		increaseSpeed = false;
	}

	if (decreaseSpeed) {
		planeSpeed -= 0.006;
		decreaseSpeed = false;
	}

	if (cameraUp) {
		y += cameraVerticalConstant;
		cameraUp = false;
	}

	if (cameraDown) {
		y -= cameraVerticalConstant;
		cameraDown = false;
	}

	// redraw the new state
	glutPostRedisplay();
}

void mySpecialKeyboard(unsigned char key, int x, int y)
{

	if (key == GLUT_KEY_UP)
		cameraUp = true;
	
	if (key == GLUT_KEY_DOWN)
		cameraDown = true;

	if (key == GLUT_KEY_PAGE_UP)
		increaseSpeed = true;

	if (key == GLUT_KEY_PAGE_DOWN)
		decreaseSpeed = true;

}

void myKeyboard(unsigned char key, int x, int y)
{

	if (key == 'q')
		exit(0);

	if (key == 'w') {
		isWireframeEnabled = !isWireframeEnabled;
	}

	if (key == 's') {
		showSkyAndSea = !showSkyAndSea;
	}

	if (key == 'f') {
		glMatrixMode(GL_PROJECTION);
		if (isFullScreen)
			glutLeaveFullScreen();
		else
			glutFullScreen();
		isFullScreen = !isFullScreen;
		glMatrixMode(GL_MODELVIEW);
	}

	if (key == 'b')
		isFogEnabled = !isFogEnabled;

	if (key == 'm')
		isMountainsEnabled = !isMountainsEnabled;
}

void buildCessnaPropellers()
{

	// IMPORTANT
	// "fopen" will make compiler complain about being depricated.
	// To fix this, I wen to project configuration -> C/C++ -> Preprocessor
	// And for the "ProcessorDefinitions" field, I added ";_CRT_SECURE_NO_WARNINGS"
	FILE *file;
	char buff[255];
	int i = 1;
	int l = 0;

	file = fopen("propeller.txt", "r");

	fscanf(file, "%s", buff);
	fscanf(file, "%s", buff);
	sscanf(buff, "%f", &propellerVerticies[0][0]);
	fscanf(file, "%s", buff);
	sscanf(buff, "%f", &propellerVerticies[0][1]);
	fscanf(file, "%s", buff);
	sscanf(buff, "%f", &propellerVerticies[0][2]);

	while (fscanf(file, "%s", buff) != EOF) {
		if (strcmp(buff, "v") == 0) {
			fscanf(file, "%s", buff);
			sscanf(buff, "%f", &propellerVerticies[i][0]);
			fscanf(file, "%s", buff);
			sscanf(buff, "%f", &propellerVerticies[i][1]);
			fscanf(file, "%s", buff);
			sscanf(buff, "%f", &propellerVerticies[i][2]);
			i++;
		}
		else if (strcmp(buff, "n") == 0) {
			fscanf(file, "%s", buff);
			sscanf(buff, "%f", &propellerNormals[l][0]);
			fscanf(file, "%s", buff);
			sscanf(buff, "%f", &propellerNormals[l][1]);
			fscanf(file, "%s", buff);
			sscanf(buff, "%f", &propellerNormals[l][2]);
			l++;
		}
		else if (strcmp(buff, "g") == 0) {
			GLint idx = 0;

			fscanf(file, "%s", buff); // junk
			while (fscanf(file, "%s", buff) != EOF) { // f/g
				if (strcmp(buff, "g") == 0) {
					fscanf(file, "%s", buff); // junk
				}
				else if (strcmp(buff, "f") == 0) {
					idx = 0;
					v++;
				}
				else {
					sscanf(buff, "%d", &propellerFaces[v][idx]);
					idx++;
				}
			}
		}
	}
	fclose(file);
}

void buildCessna()
{

	// IMPORTANT
	// "fopen" will make compiler complain about being depricated.
	// To fix this, I wen to project configuration -> C/C++ -> Preprocessor
	// And for the "ProcessorDefinitions" field, I added ";_CRT_SECURE_NO_WARNINGS"
	FILE *file;
	char buff[255];
	int i = 1;
	int l = 0;

	file = fopen("cessna.txt", "r");

	fscanf(file, "%s", buff);
	fscanf(file, "%s", buff);
	sscanf(buff, "%f", &verticies[0][0]);
	fscanf(file, "%s", buff);
	sscanf(buff, "%f", &verticies[0][1]);
	fscanf(file, "%s", buff);
	sscanf(buff, "%f", &verticies[0][2]);

	while (fscanf(file, "%s", buff) != EOF) {
		if (strcmp(buff, "v") == 0) {
			fscanf(file, "%s", buff);
			sscanf(buff, "%f", &verticies[i][0]);
			fscanf(file, "%s", buff);
			sscanf(buff, "%f", &verticies[i][1]);
			fscanf(file, "%s", buff);
			sscanf(buff, "%f", &verticies[i][2]);
			i++;
		}
		else if (strcmp(buff, "n") == 0) {
			fscanf(file, "%s", buff);
			sscanf(buff, "%f", &normals[l][0]);
			fscanf(file, "%s", buff);
			sscanf(buff, "%f", &normals[l][1]);
			fscanf(file, "%s", buff);
			sscanf(buff, "%f", &normals[l][2]);
			l++;
		}
		else if (strcmp(buff, "g") == 0) {
			GLint idx = 0;

			fscanf(file, "%s", buff); // junk
			while (fscanf(file, "%s", buff) != EOF) { // f/g
				if (strcmp(buff, "g") == 0) {
					fscanf(file, "%s", buff); // junk
					subObjectIndexes[k] = j;
					k++;
				}
				else if (strcmp(buff, "f") == 0) {
					idx = 0;
					j++;
				}
				else {
					sscanf(buff, "%d", &faces[j][idx]);
					idx++;
				}
			}	
		}
	}
	fclose(file);
}

void myMouseMotion(int x, int y)
{
	mouseDirectionAngle = ((x - (windowWidth / 2)) * PI) / (windowWidth / 2);
}

void myReshape(int w, int h) 
{
	if (h == 0)
		h = 1;

	// calc ratio
	float ratio = 1.0* w / h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// set viewport to be the entire window
	glViewport(0, 0, w, h);

	// set perspective
	gluPerspective(45, ratio, 1, 1000);

	glMatrixMode(GL_MODELVIEW);
}

void initializeGL()
{
	// enable depth testing
	glEnable(GL_DEPTH_TEST);

	// set background color to be black
	glClearColor(0, 0, 0, 1.0);

	// lighting configuration
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);
	GLfloat lightDiffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat lightSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat lightAmbient[] = { 1.0, 1.0, 1.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	GLfloat globalAmbient[] = { 0.0, 0.0, 0.2 };
	glLightfv(GL_LIGHT0, GL_LIGHT_MODEL_AMBIENT, globalAmbient);
	GLfloat specularMat[] = { 1.0, 1.0, 1.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularMat);

	// fog configuration
	GLfloat fogColor[] = { 1.0, 0.65, 0.65, 1.0 };
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_MODE, GL_EXP);
	glFogf(GL_FOG_DENSITY, 0.005);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	// change into projection mode so that we can change the camera properties
	glMatrixMode(GL_PROJECTION);

	// load the identity matrix into the projection matrix
	glLoadIdentity();

	// set camera to perspective and position and look direction
	gluPerspective(75.0, 1.77777778, 0.01, 600.0);
	gluLookAt(-90.0, 15.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	// change into model-view mode so that we can change the object positions
	glMatrixMode(GL_MODELVIEW);
}

void main(int argc, char** argv)
{
	// initialize the toolkit
	glutInit(&argc, argv);
	// set display mode
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	// set window size
	glutInitWindowSize(windowWidth, windowHeight);
	// set window position on screen
	glutInitWindowPosition(100, 20);
	// open the screen window
	glutCreateWindow("Kerbal Plane Program");

	// register redraw function
	glutDisplayFunc(myDisplay);
	// register the idle function
	glutIdleFunc(myIdle);
	// register the keyboard function
	glutKeyboardFunc(myKeyboard);
	// register the keyboard function for special keys
	glutSpecialFunc(mySpecialKeyboard);
	// passive mouse
	glutPassiveMotionFunc(myMouseMotion);
	// register the window resize function
	glutReshapeFunc(myReshape);

	//initialize the rendering context
	initializeGL();

	// read verticies, faces, and normals from file into local arrays
	buildCessna();
	buildCessnaPropellers();

	// print key bindings to debug console
	printControls();

	// build three mountains
	InitMountainGrids();
	BuildMountain1(0, 0, MAX_X1, MAX_Z1, 1, ITERATIONS);
	BuildMountain2(0, 0, MAX_X2, MAX_Z2, 1, ITERATIONS);
	BuildMountain3(0, 0, MAX_X3, MAX_Z3, 1, ITERATIONS);

	// initialize and link textures
	seaTexture = createTexture("sea02.bmp", 1600, 1200);
	skyTexture = createTexture("sky08.bmp", 896, 385);

	// go into perpetual loop
	glutMainLoop();

}