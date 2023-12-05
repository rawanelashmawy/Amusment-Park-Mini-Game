#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>
#include <string> 
#include <iostream>
#include <sstream>

#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)
int windowWidth = 1000;
int windowHeight = 600;
float currentColor = 0.0f;
float playerX = 0.5f;
float playerY = 0.4f;
float playerZ = 0.5f;
float movementSpeed = 0.1f;
float playerRotation = 0.0f;
float minX = 0.2f;
float maxX = 7.8f;
float minZ = 0.2f;
float maxZ = 7.8f;
float triangleY = 2.0f;
bool movingUp = true;
bool kioskVisible = true;
float kioskX = 6.8f;
float kioskZ = 0.8f;
int winWidth = 400;
int winHeight = 200;
int winPosX = 100;
int winPosY = 100;
bool winVisible = false;
bool gameRunning = true;
time_t startTime;
int countdown = 50;
bool timesup = false;
bool displayYouWin = false;
bool rotateTeapots = false;
float rotationAngle1 = 0.0f;
const int numCabins = 12;
float angle = 0.0f;
bool rotateWheel = false;
float wheelAngle = 0.0f;
float treePosition = 0.0f;
bool animateTrees = false;
bool movingDown = false;
bool rotateLampCover = false;
float lampCoverRotation = 0.0f;
float benchPosition = 0.0f;
float benchSpeed = 0.005f;
bool moveForward = false;



enum CameraView {
	FRONT,
	TOP,
	SIDE
};
CameraView currentView = FRONT;

class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f& v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f& v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};

class Camera {
public:
	Vector3f eye, center, up;

	Camera(float eyeX = 1.0f, float eyeY = 1.0f, float eyeZ = 1.0f, float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}

	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}

	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}
};
Camera camera;

void drawGround(double thickness) {
	glPushMatrix();
	glColor3f(0.4, 0.8, 0.4);//green
	glTranslated(0.5, 0.5 * thickness, 0.5);
	glScaled(1.0, thickness, 1.0);
	glutSolidCube(1);
	glPopMatrix();
}
void drawWall(double thickness) {
	glPushMatrix();
	glColor3f(0.7f, currentColor - 1, currentColor);
	glTranslated(0.5, 0.5 * thickness, 0.5);
	glScaled(1.0, thickness, 1.0);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.7f, currentColor - 1, currentColor);
	glTranslated(0.5, 0.5 * thickness, 0.5);
	glScaled(1.0, thickness, 1.0);
	glutSolidCube(1);
	glPopMatrix();
}


void setupLights() {
	// Material Properties
	GLfloat ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat diffuse[] = { 0.6f, 0.6f, 0.6f, 1.0f };
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat shininess[] = { 50.0f };

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

	// Light Properties
	GLfloat lightIntensity[] = { 0.7f, 0.7f, 1.0f, 1.0f };
	GLfloat lightPosition[] = { -7.0f, 6.0f, 3.0f, 0.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
}

void initializeCamera() {
	Vector3f currentEye = camera.eye;
	switch (currentView) {
	case FRONT:
		camera.eye = Vector3f(4.0f, 3.0f, 12.0f);  //cam position
		camera.center = Vector3f(4.0f, 0.5f, 0.0f); //cam looking @
		camera.up = Vector3f(0.0f, 2.0f, 2.0f);


		break;
	case TOP:
		camera.eye = Vector3f(4.0f, 9.0f, 4.0f);
		camera.center = Vector3f(4.0f, 0.5f, 4.0f);
		camera.up = Vector3f(0.0f, 0.0f, -1.0f);

		break;
	case SIDE:
		camera.eye = Vector3f(13.0f, 3.5f, 4.0f);
		camera.center = Vector3f(0.4f, 0.5f, 4.0f);
		camera.up = Vector3f(0.0f, 1.0f, 0.0f);

		camera.moveY(1);
		break;
	}
}

void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 640 / 480, 0.001, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}

void animateWallsColor() {
	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	float colorChangeSpeed = 0.001f;
	currentColor = fmod(currentTime * colorChangeSpeed, 1.0);
}

void animateTriangle(int value) {
	const float step = 0.01f;

	if (movingUp) {
		triangleY += step;
		if (triangleY >= 2.28f) {
			movingUp = false;
		}
	}
	else {
		triangleY -= step;
		if (triangleY <= 2.0f) {
			movingUp = true;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(50, animateTriangle, 0);
}

void Idle() {
	animateWallsColor();
	glutPostRedisplay();
}
void drawPlayer() {
	glPushMatrix();
	glTranslated(playerX, playerY, playerZ);
	glRotatef(playerRotation, 0.0f, 1.0f, 0.0f);
	glScaled(0.3, 0.3, 0.3);
	// Head (Cube)
	glPushMatrix();
	glColor3f(1.0f, 1.0f, 0.1f); // Head color (yellow)
	glTranslated(0.0, 1.5, 0.0);
	glutSolidCube(1);
	glPopMatrix();
	// Eyes (Spheres)
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 0.0f);
	glTranslated(0.2, 1.7, 0.5);
	glutSolidSphere(0.1, 20, 20);
	glTranslated(-0.4, 0.0, 0.0);
	glutSolidSphere(0.1, 20, 20);
	glPopMatrix();
	// Mouth (Rectangle)
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 0.0f);
	glTranslated(0.0, 1.3, 0.55);
	glScaled(0.2, 0.05, 0.05);
	glutSolidCube(1);
	glPopMatrix();
	// Body (Cube)
	glPushMatrix();
	glColor3f(1.0, 0.1, 0);
	glTranslated(0.0, 0.5, 0.0);
	glScaled(0.8, 1.0, 0.4);
	glutSolidCube(1);
	glPopMatrix();

	// Limbs (Cubes)
	glPushMatrix();
	glColor3f(1.0f, 1.0f, 0.1f);
	glScaled(0.5, 1.0, 0.4);

	//  arms
	glPushMatrix();
	glTranslated(0.6, 0.8, -0.1);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-0.6, 0.8, -0.1);
	glutSolidCube(1);
	glPopMatrix();

	// legs
	glPushMatrix();
	glColor3f(0.1f, 0.2f, 0.5f);
	glTranslated(0.5, -0.3, 0.0);
	glutSolidCube(0.8);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-0.5, -0.3, 0.0);
	glutSolidCube(0.8);
	glPopMatrix();

	glPopMatrix();
	// Shoes (Cubes)
	glPushMatrix();
	glColor3f(0.6f, 0.6f, 0.6f);
	glTranslated(0.3, -1, 0.0);
	glutSolidCube(0.5);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-0.3, -1, 0.0);
	glutSolidCube(0.5);
	glPopMatrix();

	glPopMatrix();
}

void drawKiosk() {
	// Kiosk Booth
	glPushMatrix();
	glTranslated(6.8, 0, 0.8);
	glScaled(0.8, 0.8, 0.8);


	glPushMatrix();
	glColor3f(1.0, 0.6f, 0.6f);
	glTranslated(0.0, 0.5, 0.0);
	glScaled(1.5, 1.0, 1.0);
	glutSolidCube(1.0);
	// Window 
	glColor3f(1.0f, 0.6f, 0.0f);
	glTranslated(0.0, 0.2, 0.5);
	glScaled(0.8, 0.8, 0.05);
	glutSolidCube(1.0);
	glPopMatrix();

	// Roof 
	glPushMatrix();
	glColor3f(0.8f, 0.2f, 0.2f);
	glTranslated(0.0, 1.0, 0.0);
	glRotated(-90.0, 1.0, 0.0, 0.0);
	glutSolidCone(1.0, 1.0, 10, 10);
	glPopMatrix();



	// Signboard
	glPushMatrix();
	glColor3f(0.9f, 0.6f, 0.5f);
	glTranslated(-0.3, 2.0, 0.0);
	glRotated(180.0, 0.0, 1.0, 0.0);
	glScaled(0.05, 1.0, 0.05);
	glutSolidCube(1.0);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0.3, 2.0, 0.0);
	glRotated(180.0, 0.0, 1.0, 0.0);
	glScaled(0.05, 1.0, 0.05);
	glutSolidCube(1.0);
	glPopMatrix();

	// Signboard 
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 0.0f);
	glTranslated(0.0, 2.4, 0.0);
	glRotated(180.0, 0.0, 1.0, 0.0);
	glScaled(1.0, 0.8, 0.1);
	glutSolidCube(1.0);


	//arrow
	glPopMatrix();
	glPushMatrix();
	glColor3f(0.5f, 1.0f, 0.2f);
	glTranslated(0.0, triangleY, 0.0);
	glBegin(GL_TRIANGLES);
	glVertex3f(-0.3, 0.5, 0.07);
	glVertex3f(0.0, 0.0, 0.07);
	glVertex3f(0.3, 0.5, 0.07);
	glEnd();
	glPopMatrix();
	glPopMatrix();
}
bool checkCollision(float playerX, float playerZ, float kioskX, float kioskZ) {
	float distance = sqrt((playerX - kioskX) * (playerX - kioskX) + (playerZ - kioskZ) * (playerZ - kioskZ));
	if (distance < 1.0) {
		return true;
	}
	return false;
}
void YouWinDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);


	glColor3f(1.0f, 1.0f, 1.0f);

	glRasterPos2f(-0.5f, 0.0f);

	const char* message = "You Win!";
	for (int i = 0; message[i] != '\0'; ++i) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, message[i]);
	}

	glFlush();
}
void YouLoseDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);


	glColor3f(1.0f, 1.0f, 1.0f);

	glRasterPos2f(-0.5f, 0.0f);

	const char* message = "Time is Up, You Lose!!";
	for (int i = 0; message[i] != '\0'; ++i) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, message[i]);
	}

	glFlush();
}
void updateTimer() {
	int currentTime = time(nullptr);
	int elapsed = currentTime - startTime;
	countdown = 50 - elapsed;
	if (countdown <= 0) {
		timesup = true;
	}
}
void drawTimer() {
	updateTimer();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, windowWidth, windowHeight, 0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(1.0f, 1.0f, 1.0f);

	std::string timerText = "Time: " + std::to_string(countdown) + " s";
	glRasterPos2i(10, 20);
	for (const char& c : timerText) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}
void displayYouWinDelayed(int value) {
	displayYouWin = true;
}


void drawCylinderPlate() {
	glColor3f(0.8f, 0.0f, 0.8f);
	glPushMatrix();
	glRotated(-90, 1.0, 0.0, 0.0);

	// cylindrical plate
	GLUquadricObj* plate = gluNewQuadric();
	gluCylinder(plate, 1.0, 1.0, 0.1, 20, 20);

	// top and bottom 
	gluDisk(plate, 0.0, 1.0, 20, 1);
	glTranslatef(0.0, 0.0, 0.1);
	gluDisk(plate, 0.0, 1.0, 20, 1);

	gluDeleteQuadric(plate);
	glPopMatrix();
}

void drawTeacupRide() {
	glPushMatrix();
	glTranslated(4, 0, 3);
	//the center teapot
	glPushMatrix();
	glColor3f(0.4f, 0.3f, 1.0f);
	glTranslatef(0.0f, 0.5f, 0.0f);
	glutSolidTeapot(0.5);
	glPopMatrix();

	//cylindrical plate with legs
	glPushMatrix();
	glScaled(1.3, 1.3, 1.3);
	drawCylinderPlate();
	glPopMatrix();

	// cups
	glPushMatrix();
	glColor3f(0.0f, 1.0f, 0.8f);
	glRotatef(rotationAngle1, 0.0, 1.0, 0.0);
	glRotatef(0 * 90, 0.0, 1.0, 0.0);
	glTranslatef(0.9f, 0.3f, 0.0f);
	glutSolidTeapot(0.3);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.9f, 0.0f, 0.6f);
	glRotatef(rotationAngle1, 0.0, 1.0, 0.0);
	glRotatef(1 * 90, 0.0, 1.0, 0.0);
	glTranslatef(0.9f, 0.3f, 0.0f);
	glutSolidTeapot(0.3);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 0.6f, 0.0f); // orange
	glRotatef(rotationAngle1, 0.0, 1.0, 0.0);
	glRotatef(2 * 90, 0.0, 1.0, 0.0);
	glTranslatef(0.9f, 0.3f, 0.0f);
	glutSolidTeapot(0.3);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.9f, 0.3f, 0.3f); // Red 
	glRotatef(rotationAngle1, 0.0, 1.0, 0.0);
	glRotatef(3 * 90, 0.0, 1.0, 0.0);
	glTranslatef(0.9f, 0.3f, 0.0f);
	glutSolidTeapot(0.3);
	glPopMatrix();
	if (rotateTeapots)
		rotationAngle1 += 1.0;

	glPopMatrix();
}

void animateTeapots() {
	if (rotateTeapots) {
		glRotatef(1.0, 0.0, 1.0, 0.0);
	}
}

void drawFerrisWheel() {
	glColor3f(0.5f, 0.5f, 0.5f); // Gray 
	glPushMatrix();
	glTranslated(1.4, 1.5, 7.0);
	glScaled(0.3, 0.3, 0.3);
	glRotatef(45.0, 0.0, 1.0, 0.0);
	//central wheel
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 1.0f); // Blue
	glutSolidSphere(0.3, 20, 20);
	glPopMatrix();

	// lines connecting cabins to the center
	glPushMatrix();
	glLineWidth(3.0);
	glRotatef(wheelAngle, 0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
	glColor3f(0.5f, 0.5f, 0.5f); // Gray
	for (int i = 0; i < numCabins; ++i) {
		float theta = 2.0f * 3.1416f * float(i) / float(numCabins);
		glVertex3f(0.3f * cosf(theta), 0.3f * sinf(theta), 0.0f);
		glVertex3f(3.0f * cosf(theta), 3.0f * sinf(theta), 0.0f);
	}
	glEnd();
	glPopMatrix();


	// wheel 1
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.24f);
	glRotatef(wheelAngle, 0.0f, 0.0f, 1.0f);
	glLineWidth(2.5);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < numCabins; ++i) {
		float theta = 2.0f * 3.1416f * float(i) / float(numCabins);
		glVertex3f(3.0f * cosf(theta), 3.0f * sinf(theta), 0.0f);
	}
	glEnd();
	glPopMatrix();
	// wheel 2
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -0.24f);
	glRotatef(wheelAngle, 0.0f, 0.0f, 1.0f);
	glLineWidth(2.5);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < numCabins; ++i) {
		float theta = 2.0f * 3.1416f * float(i) / float(numCabins);
		glVertex3f(3.0f * cosf(theta), 3.0f * sinf(theta), 0.0f);
	}
	glEnd();
	glPopMatrix();

	//cabins
	glColor3f(0.0f, 0.0f, 1.0f); // Blue
	for (int i = 0; i < numCabins; ++i) {
		float theta = 2.0f * 3.1416f * float(i) / float(numCabins);
		glPushMatrix();
		glRotatef(wheelAngle, 0.0f, 0.0f, 1.0f);
		glTranslatef(3.0f * cosf(theta), 3.0f * sinf(theta), 0.0f);
		glutSolidCube(0.5);
		glPopMatrix();
	}
	// stand
	glPushMatrix();
	glColor3f(0.5f, 0.5f, 0.5f); // Gray 
	glLineWidth(10.0);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-3.0f, -4.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(3.0f, -4.0f, 0.0f);
	glEnd();
	glPopMatrix();

	//ground rectangle
	glColor3f(0.3f, 0.3f, 0.3f); // Dark gray 
	glRotated(-90.0, 1.0, 0.0, 0.0);
	GLfloat groundVertices[4][3] = {
		{-4.0f, -1.0f, -4.0f},
		{-4.0f, 1.0f, -4.0f},
		{4.0f, 1.0f, -4.0f},
		{4.0f, -1.0f, -4.0f}
	};
	glBegin(GL_QUADS);
	for (int i = 0; i < 4; ++i) {
		glVertex3fv(groundVertices[i]);
	}
	glEnd();

	glPopMatrix();
}
void animateWheel() {
	if (rotateWheel) {
		wheelAngle += 0.5f;

		if (wheelAngle >= 360.0f) {
			wheelAngle -= 360.0f;
		}

		glutPostRedisplay();
	}
}

void drawTree() {
	glPushMatrix();
	glScaled(0.7, 0.7, 0.7);
	//trunk
	glColor3f(0.5f, 0.35f, 0.05f);
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glScalef(0.2f, 2.1f, 0.2f);
	glutSolidCube(1.5);
	glPopMatrix();

	//tree top 
	glColor3f(0.0f, 0.5f, 0.0f);
	glPushMatrix();
	glTranslatef(0.0f, 1.5f, 0.0f);

	const int leavesCount = 8;
	const float radius = 0.5f;
	const float height = 1.0f;
	const float slice = 360.0f / leavesCount;

	for (int i = 0; i < leavesCount; ++i) {
		glPushMatrix();
		glRotatef(slice * i, 0.0f, 1.0f, 0.0f);
		glTranslatef(0.0f, 0.0f, 0.2f);
		glutSolidCone(radius, height, 8, 8);
		glPopMatrix();
	}

	glPopMatrix();
	glPopMatrix();

}

void drawLampPost() {
	glPushMatrix();
	glTranslated(0, 0.5, 0);
	glScaled(0.5, 0.5, 0.5);

	//  pole
	glColor3f(0.5f, 0.5f, 0.5f);
	glPushMatrix();
	glTranslatef(0.0f, -1.0f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	GLUquadricObj* quadric = gluNewQuadric();
	gluCylinder(quadric, 0.1, 0.1, 3.0, 20, 20);
	gluDeleteQuadric(quadric);
	glPopMatrix();

	// Dlamp head
	glColor3f(0.8f, 0.8f, 0.0f);
	glPushMatrix();
	glTranslatef(0.0f, 2.0f, 0.0f);
	glutSolidSphere(0.4, 20, 20);
	glPopMatrix();

	// lamp cover
	glColor4f(0.8f, 0.8f, 1.0f, 0.4f);
	glPushMatrix();
	glTranslatef(0.0f, 2.0f, 0.0f);
	glRotatef(lampCoverRotation, 0.0f, 1.0f, 0.0f);
	glutSolidTorus(0.42, 0.5, 10, 30);
	glPopMatrix();

	// light bulb
	glColor3f(1.0f, 1.0f, 0.8f);
	glPushMatrix();
	glTranslatef(0.0f, 2.2f, 0.0f);
	glutSolidSphere(0.1, 10, 10);
	glPopMatrix();


	glPopMatrix();

}
void animateLampRotation() {
	if (rotateLampCover) {
		lampCoverRotation += 0.3f;
		if (lampCoverRotation >= 360.0f) {
			lampCoverRotation -= 360.0f;
		}
	}
}

void drawSeesaw() {
	glPushMatrix();
	glScaled(3.7, 3.7, 3.7);
	//seesaw plank
	glPushMatrix();
	glColor3f(0.9f, 0.6f, 0.3f);
	glTranslatef(-0.8f, -0.5f, 0.0f);
	glScalef(0.5f, 0.02f, 0.1f);
	glutSolidCube(1.0);
	glPopMatrix();

	// Center cone
	glColor3f(0.3f, 0.3f, 0.9f);
	glPushMatrix();
	glTranslatef(-0.8f, -0.60f, 0.0f);
	glRotated(-90.0, 1.0, 0.0, 0.0);
	glutSolidCone(0.1, 0.1, 20, 1);
	glPopMatrix();


	glPushMatrix();
	glColor3f(0.9f, 0.6f, 0.3f);
	glTranslatef(-0.6f, -0.46f, 0.0f);
	glutSolidCube(0.07);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.9f, 0.6f, 0.3f);
	glTranslatef(-1.0f, -0.46f, 0.0f);
	glutSolidCube(0.07);
	glPopMatrix();
	glPopMatrix();
}

void drawBench() {
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, benchPosition);
	glScaled(0.8, 0.8, 0.8);

	glRotatef(-90.0, 0.0, 1.0, 0.0);

	//bench top
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glScalef(1.5f, 0.1f, 0.5f);
	glutSolidCube(1.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.6f, -0.3f, 0.2f);
	glScalef(0.1f, 0.6f, 0.1f);
	glutSolidCube(1.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.7f, -0.3f, 0.2f);
	glScalef(0.1f, 0.6f, 0.1f);
	glutSolidCube(1.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.6f, -0.3f, -0.2f);
	glScalef(0.1f, 0.6f, 0.1f);
	glutSolidCube(1.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.6f, -0.3f, -0.2f);
	glScalef(0.1f, 0.6f, 0.1f);
	glutSolidCube(1.0f);
	glPopMatrix();
	glPopMatrix();

}

void updateB(int value) {
	if (moveForward) {
		benchPosition += benchSpeed;

		// Check for boundaries to reverse direction
		if (benchPosition > 0.1f || benchPosition < -0.1f)
			benchSpeed = -benchSpeed;
	}

	glutPostRedisplay();
	glutTimerFunc(15, updateB, 0);
}

void Display() {
	setupCamera();
	setupLights();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Ground
	glPushMatrix();
	glScaled(8.0, 1.0, 8.0);
	drawGround(0.02);
	glPopMatrix();

	// Walls 
	// Left Wall
	glPushMatrix();
	glTranslated(0.0, 0.0, 0.0);
	glScaled(4.0, 1.0, 8.0);
	glRotated(90, 0, 0, 1.0);
	drawWall(0.02);
	glPopMatrix();

	// Back Wall
	glPushMatrix();
	glTranslated(0.0, 0.0, 0.0);
	glScaled(8.0, 1.0, 8.0);
	glRotated(-90, 1.0, 0.0, 0.0);
	drawWall(0.02);
	glPopMatrix();

	// Right Wall
	glPushMatrix();
	glTranslated(8.0, 0.0, 0.0);
	glScaled(4.0, 1.0, 8.0);
	glRotated(90, 0, 0, 1.0);
	drawWall(0.02);
	glPopMatrix();

	drawPlayer();

	//collision w/goal
	if (checkCollision(playerX, playerZ, kioskX, kioskZ) && gameRunning) {
		// Player collided with the kiosk
		kioskVisible = false;
		gameRunning = false; // Stop the game
		glutTimerFunc(500, displayYouWinDelayed, 0);

	}

	//Kiosk
	if (kioskVisible) {
		drawKiosk();
	}
	//timer
	drawTimer();

	if (displayYouWin) {
		int newWindow = glutCreateWindow("You Win!");
		glutDisplayFunc(YouWinDisplay);
	}
	if (timesup) {
		int newWindow = glutCreateWindow("You Lose!");
		glutDisplayFunc(YouLoseDisplay);

	}
	animateTeapots();
	drawTeacupRide();
	animateWheel();
	drawFerrisWheel();

	glPushMatrix();
	glTranslatef(2.0f, 0.5f + treePosition, 1.0f);
	drawTree();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(5.0f, 0.5f + treePosition, 1.0f);
	drawTree();
	glPopMatrix();

	animateLampRotation();


	glPushMatrix();
	glTranslatef(0.1f, 0.0f, 0.1f);
	drawLampPost();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.5f, 0.0f, 5.0f);
	drawLampPost();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(7.52f, 0.0f, 5.0f);
	drawLampPost();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(7.8f, 0.0f, 0.5f);
	drawLampPost();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(8.5f, 2.5f, 9.0f);
	glRotatef(-35.0, 0.0, 1.0, 0.0);
	drawSeesaw();
	glPopMatrix();

	glPushMatrix();
	glTranslated(7.5, 0.5, 3.5);
	drawBench();
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.5, 0.5, 3.5);
	drawBench();
	glPopMatrix();

	glutSwapBuffers();
	glFlush();
}

void update(int value) {
	glutPostRedisplay();
	glutTimerFunc(15, update, 0);

	if (animateTrees) {
		if (treePosition < 0.1f && !movingDown)
			treePosition += 0.003f; // up
		else {
			movingDown = true;
			treePosition -= 0.003f; //down
			if (treePosition <= 0.0f)
				movingDown = false;
		}
	}
}


void Keyboard(unsigned char key, int x, int y) {
	float d = 0.4;

	switch (key) {
	case 'w':
		camera.moveY(d);
		break;
	case 's':
		camera.moveY(-d);
		break;
	case 'a':
		camera.moveX(d);
		break;
	case 'd':
		camera.moveX(-d);
		break;
	case 'q':
		camera.moveZ(d);
		break;
	case 'e':
		camera.moveZ(-d);
		break;
	case '1':
		currentView = TOP;  // Switch to top view
		initializeCamera();
		break;
	case '2':
		currentView = FRONT;  // Switch to front view
		initializeCamera();
		break;
	case '3':
		currentView = SIDE;  // Switch to side view
		initializeCamera();
		break;
	case 'k': // Move backwards USING 2 (50)
		if (playerZ - movementSpeed >= minZ) {
			playerZ -= movementSpeed;
			playerRotation = 180.0f;
		}
		break;
	case 'i': // Move forwards USING 5 (53)
		if (playerZ + movementSpeed <= maxZ) {
			playerZ += movementSpeed;
			playerRotation = 0.0f;
		}
		break;
	case 'l': // Move right USING 3 (51)
		if (playerX + movementSpeed <= maxX) {
			playerX += movementSpeed;
			playerRotation = 90.0f;
		}
		break;
	case 'j': // Move left USING 1 (49)
		if (playerX - movementSpeed >= minX) {
			playerX -= movementSpeed;
			playerRotation = -90.0f;
		}
		break;
	case 13: // Enter key to START ANIMATION
		rotateTeapots = true; // Start teapot rotation
		rotateWheel = true;
		animateTrees = !animateTrees; // Toggle animation state
		if (animateTrees)
			glutTimerFunc(0, update, 0);
		rotateLampCover = true;
		moveForward = true;
		break;
	case 'x': // x key to STOP ANIMATION
		rotateTeapots = false; // Stop teapot rotation
		rotateWheel = false;
		rotateLampCover = false;
		moveForward = false;
		break;

	case GLUT_KEY_ESCAPE:
		exit(EXIT_SUCCESS);
	}

	glutPostRedisplay();
}

void Special(int key, int x, int y) {
	float a = 1.0;

	switch (key) {
	case GLUT_KEY_UP:
		camera.rotateX(a);
		break;
	case GLUT_KEY_DOWN:
		camera.rotateX(-a);
		break;
	case GLUT_KEY_LEFT:
		camera.rotateY(a);
		break;
	case GLUT_KEY_RIGHT:
		camera.rotateY(-a);
		break;
	}

	glutPostRedisplay();
}


void main(int argc, char** argv) {
	glutInit(&argc, argv);

	glutInitWindowSize(1000, 600);
	glutInitWindowPosition(50, 50);

	glutCreateWindow("Amusement Park");
	glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(Special);



	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	camera = Camera(1.2f, 0.6f, 3.0f, 1.0f, 0.5f, 0.6f, 0.0f, 1.0f, 0.0f);
	camera.eye = Vector3f(4.0f, 3.0f, 12.0f);  //cam position
	camera.center = Vector3f(4.0f, 0.5f, 0.0f); //cam looking @

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	//Set up the idle function to continuously animate colors
	glutIdleFunc(Idle);
	glutTimerFunc(0, animateTriangle, 0);
	glutTimerFunc(25, updateB, 0);
	startTime = time(nullptr);
	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}
