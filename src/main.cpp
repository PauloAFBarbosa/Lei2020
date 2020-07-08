#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include"Bone.h"
#include"skeleton.h"
#include<GL/glut.h>

#endif

#define _USE_MATH_DEFINES
#include <math.h>

skeleton* up;
skeleton* down;
float alfa = 0.0f, beta = 0.5f, radius = 20.0f;
float camX = 0, camY = 5, camZ = 20;
float rot = 0;
int winid = 0;

int controlling;

skeleton* targets[5];

void spherical2Cartesian() {
	camX = radius * cos(beta) * sin(alfa);
	camY = radius * sin(beta);
	camZ = radius * cos(beta) * cos(alfa);
}

void processKeys(unsigned char c, int xx, int yy) {
	float step = 0.1;
	switch (c)
	{
	case 'q':
		targets[controlling]->target[1] += 0.5;
		break;
	case 'e':
		targets[controlling]->target[1] -= 0.5;
		break;
	case 'w':
		targets[controlling]->target[0] += 0.5;
		break;
	case 'd':
		targets[controlling]->target[2] -= 0.5;
		break;
	case 's':
		targets[controlling]->target[0] -= 0.5;
		break;
	case 'a':
		targets[controlling]->target[2] += 0.5;
		break;
	case 'j':
		controlling++;
		break;
	case 'k':
		controlling--;
		if (controlling < 0)
			controlling = 0;
		break;
	case 'r':
		up->changerestrictions();
		down->changerestrictions();
		break;
	case 'o':
		up->changeoutwards();
		down->changeoutwards();
		break;
	case 27:
		glutDestroyWindow(winid);
		exit(0);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void processSpecialKeys(int key, int xx, int yy) { 
	switch (key) {

	case GLUT_KEY_RIGHT:
		alfa -= 0.1; break;

	case GLUT_KEY_LEFT:
		alfa += 0.1; break;

	case GLUT_KEY_UP:
		beta += 0.1f;
		if (beta > 1.5f)
			beta = 1.5f;
		break;

	case GLUT_KEY_DOWN:
		beta -= 0.1f;
		if (beta < -1.5f)
			beta = -1.5f;
		break;

	case GLUT_KEY_PAGE_DOWN: radius -= 2.0f;
		if (radius < 1.0f)
			radius = 1.0f;
		break;

	case GLUT_KEY_PAGE_UP: radius += 2.0f; break;
	}
	spherical2Cartesian();
	glutPostRedisplay();
}


void changeSize(int w, int h) {
	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if (h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(45.0f, ratio, 1.0f, 1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

void drawAxis() {
	// red x
	glColor3f(1.0, 0.0, 0.0); 
	glBegin(GL_LINES);
	glVertex3f(-4.0, 0.0f, 0.0f);
	glVertex3f(4.0, 0.0f, 0.0f);

	// arrow
	glVertex3f(4.0, 0.0f, 0.0f);
	glVertex3f(3.0, 1.0f, 0.0f);

	glVertex3f(4.0, 0.0f, 0.0f);
	glVertex3f(3.0, -1.0f, 0.0f);

	glEnd();
	glFlush();

	// green y
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(0.0, -4.0f, 0.0f);
	glVertex3f(0.0, 4.0f, 0.0f);

	// arrow
	glVertex3f(0.0, 4.0f, 0.0f);
	glVertex3f(1.0, 3.0f, 0.0f);

	glVertex3f(0.0, 4.0f, 0.0f);
	glVertex3f(-1.0, 3.0f, 0.0f);

	glEnd();
	glFlush();

	// blue z
	glColor3f(0.0, 0.0, 1.0); 
	glBegin(GL_LINES);
	glVertex3f(0.0, 0.0f, -4.0f);
	glVertex3f(0.0, 0.0f, 4.0f);

	// arrow
	glVertex3f(0.0, 0.0f, 4.0f);
	glVertex3f(0.0, 1.0f, 3.0f);

	glVertex3f(0.0, 0.0f, 4.0f);
	glVertex3f(0.0, -1.0f, 3.0f);

	glEnd();
	glFlush();
}

void renderScene(void) {
	static float size = 1.5;
	static bool grow = true;
	static float speed = 0.01;
	static float maxsize = 2;
	static float minsize = 0.1;
	static float g_RotateS = 0.1;
	static float g_RotateX = 0.0;

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(camX, camY, camZ,
		0.0, 0.0, 0.0,
		0.0f, 1.0f, 0.0f);

	
	glPushMatrix();
	if (controlling == 0)
		glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(targets[0]->target[0], targets[0]->target[1], targets[0]->target[2]);
	glutWireSphere(0.5, 10, 10);
	glColor3f(0.0f, 0.0f, 1.0f);
	glPopMatrix();

	glPushMatrix();
	if (controlling == 1)
		glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(targets[1]->target[0], targets[1]->target[1], targets[1]->target[2]);
	glutWireSphere(0.5, 10, 10);
	glColor3f(0.0f, 0.0f, 1.0f);
	glPopMatrix();

	glPushMatrix();
	if (controlling == 2)
		glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(targets[2]->target[0], targets[2]->target[1], targets[2]->target[2]);
	glutWireSphere(0.5, 10, 10);
	glColor3f(0.0f, 0.0f, 1.0f);
	glPopMatrix();
	
	glPushMatrix();
	if (controlling == 3)
		glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(targets[3]->target[0], targets[3]->target[1], targets[3]->target[2]);
	glutWireSphere(0.5, 10, 10);
	glColor3f(0.0f, 0.0f, 1.0f);
	glPopMatrix();

	glPushMatrix();
	if (controlling == 4)
		glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(targets[4]->target[0], targets[4]->target[1], targets[4]->target[2]);
	glutWireSphere(0.5, 10, 10);
	glColor3f(0.0f, 0.0f, 1.0f);
	glPopMatrix();

	glColor3f(0.0f, 0.0f, 1.0f);

	up->draw();
	down->draw();
	up->multiUpdate(targets);
	down->multiUpdate(targets);
   
	drawAxis();

	// End of frame
	glutSwapBuffers();
}


int main(int argc, char** argv) {

	controlling = 0;

	//skeleton points
	float start[3] = { 0,0,0 };

	
	float up1[3] = { 0,2,0 };
	float up2[3] = { 0,3,0 };
	float up3[3] = { 0,4,0 };
	float up4[3] = { 0,4.5,0 };
	float up5[3] = { -1,3,0 };
	float up6[3] = { -2,1.5,0 };
	float up7[3] = { -2,-1,0 };
	float up8[3] = { 1,3,0 };
	float up9[3] = { 2,1.5,0 };
	float up10[3] = { 2,-1,0 };
	float up11[3] = { 0,4.5,1 };
	

	float down1[3] = { 0,-2,0 };
	float down2[3] = { -1,-2,0 };
	float down3[3] = { -1,-4,0 };
	float down4[3] = { -1,-6,0 };
	float down5[3] = { 1,-2,0 };
	float down6[3] = { 1,-4,0 };
	float down7[3] = { 1,-6,0 };

	float angle_vector_null[3] = { 0,0,0 };

	
	//Parte superior
	up = new skeleton(start, up1,0.1, angle_vector_null, 0.1, angle_vector_null);
	//Tronco
	up->addChildren(up2, 1.58, angle_vector_null, 1.58, angle_vector_null);
	//Cabeça
	up->children.at(0)->addChildren(up3, 1.58, angle_vector_null, 1.58, angle_vector_null);
	up->children.at(0)->children.at(0)->addChildren(up4, 1.58, angle_vector_null, 1.58, angle_vector_null);
	up->children.at(0)->children.at(0)->children.at(0)->addChildren(up11, 1.58, angle_vector_null, 1.58, angle_vector_null);
	//braço 1
	up->children.at(0)->addChildren(up5, 1.58, angle_vector_null, 1.58, angle_vector_null);
	float angle_bracos[3] = { 0,0,-1 };
	up->children.at(0)->children.at(1)->addChildren(up6, 1.58, angle_bracos, 1.58, angle_bracos);
	up->children.at(0)->children.at(1)->children.at(0)->addChildren(up7, 1.58, angle_vector_null, 1.58, angle_vector_null);
	//braço 2
	up->children.at(0)->addChildren(up8, 1.58, angle_vector_null, 1.58, angle_vector_null);
	up->children.at(0)->children.at(2)->addChildren(up9, 1.58, angle_bracos, 1.58, angle_bracos);
	up->children.at(0)->children.at(2)->children.at(0)->addChildren(up10, 1.58, angle_vector_null, 1.58, angle_vector_null);
	
	//Parte inferior
	float angle_pernas_down[3] = { 0,-1,0 };
	down = new skeleton(start, down1, 1.58, angle_vector_null, 1.58, angle_vector_null);
	//Perna 1
	float angle_pernas_left[3] = { 1,0,0 };
	float angle_pernas_right[3] = { -1,0,0 };
	down->addChildren(down2, 1.58, angle_vector_null, 1.58, angle_vector_null);
	down->children.at(0)->addChildren(down3, 1.58, angle_vector_null, 1.58, angle_vector_null);
	down->children.at(0)->children.at(0)->addChildren(down4, 1.58, angle_vector_null, 1.58, angle_vector_null);
	//Perna 2
	down->addChildren(down5, 1.58, angle_vector_null, 1.58, angle_vector_null);
	down->children.at(1)->addChildren(down6, 1.58, angle_vector_null, 1.58, angle_vector_null);
	down->children.at(1)->children.at(0)->addChildren(down7, 1.58, angle_vector_null, 1.58, angle_vector_null);

	//Targets
	up->children.at(0)->children.at(0)->children.at(0)->children.at(0)->setTarget(0, 4.5, 1);//Cabeça
	up->children.at(0)->children.at(1)->children.at(0)->children.at(0)->setTarget(-2, -1, 0);//Braço 1
	up->children.at(0)->children.at(2)->children.at(0)->children.at(0)->setTarget(2, -1, 0);//Braço 2

	down->children.at(0)->children.at(0)->children.at(0)->setTarget(-1, -6, 0);
	down->children.at(1)->children.at(0)->children.at(0)->setTarget(1, -6, 0);

	targets[0] = up->children.at(0)->children.at(0)->children.at(0)->children.at(0);
	targets[1] = up->children.at(0)->children.at(1)->children.at(0)->children.at(0);
	targets[2] = up->children.at(0)->children.at(2)->children.at(0)->children.at(0);

	targets[3] = down->children.at(0)->children.at(0)->children.at(0);
	targets[4] = down->children.at(1)->children.at(0)->children.at(0);


	// put GLUT init here
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(799, 599);
	winid = glutCreateWindow("CG@DI");
	// put callback registration here

	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);

	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);


	// OpenGL settings 
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// enter GLUT's main loop
	glutMainLoop();

	return 1;
}

