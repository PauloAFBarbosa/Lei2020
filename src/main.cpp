#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include"Bone.h"
#include"Skeleton.h"
#include<GL/glut.h>

#endif

#define _USE_MATH_DEFINES
#include <math.h>

#define POINT_COUNT 5
// Points that make up the loop for catmull-rom interpolation
//bra�os
float p[POINT_COUNT][3] = { {0,-1,-2},{0,-2,-2},{0,-2,2} ,{0,-1,2},{0,0,0} };

float p2[POINT_COUNT][3] = { {0,-1,2} ,{0,0,0} ,{0,-1,-2},{0,-2,-2},{0,-2,2}  };

//pernas
float p4[POINT_COUNT][3] = { {0,-1,-2},{0,-2,-2},{0,-2,2} ,{0,-1,2},{0,0,0} };

float p3[POINT_COUNT][3] = { {0,-1,2} ,{0,0,0} ,{0,-1,-2},{0,-2,-2},{0,-2,2} };

//cabe�a
float p5[POINT_COUNT][3] = { {0,5,-1},{2,0,3} ,{2,-0.5,3} ,{-2,-0.5,3},{-2,0,3} };

int working = 0;

float oldy[4] = { 1,0,0,0 };

float obj_center[3] = { 0,-4,0 };
float obj_size = 2;

int time;
int timebase;
int maxTime = 10000;
float stepT = 100.0f / maxTime;

skeleton* up;
skeleton* down;
float alfa = 0.0f, beta = 0.5f, radius = 20.0f;
float camX = 0, camY = 5, camZ = 20;
float rot = 0;
int winid = 0;

int controlling;
int qtargets;

bool catdebug = true;
bool catmullControlling = false;

skeleton* targets[5];

/**
 * @brief return the length of v
 * 
 * @param v array[3] with the x,y,z dimensions
 * @return float length of v
 */
float length_main(float* v) {
	float res = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	return res;
}

/**
 * @brief calculates the cam values (used in GluLookAt function) from the alteration of alfa, beta and radius
 * 
 */
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
		if (controlling >= qtargets)
			controlling = qtargets - 1;
		break;
	case 'k':
		controlling--;
		if (controlling < 0)
			controlling = 0;
		break;
	case 'c':
		catmullControlling = !catmullControlling;
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

/**
 * @brief draw an x,y,z axis for better visualization of scenario
 * 
 */
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

/**
 * @brief Multiply vector 1 and 2
 * 
 * @param v1 Vector 1
 * @param v2 Vector 2
 * @return float Result of multiplication
 */
float multvec(float* v1, float* v2) {
	float res = 0;
	for (int i = 0; i < 4; i++)
	{
		res += v1[i] * v2[i];
	}
	return res;
}

/**
 * @brief Multiplication between a vector and a matrix
 * 
 * @param m matrix with 4x4 dimentions
 * @param v Vector with size 4
 * @param res Vecttor with result of the multiplication
 */
void multMatrixVector(float* m, float* v, float* res) {
	for (int j = 0; j < 4; ++j) {
		res[j] = 0;
		for (int k = 0; k < 4; ++k) {
			res[j] += v[k] * m[j * 4 + k];
		}
	}
}

/**
 * @brief Get the Catmull Rom Point object
 * 
 * @param t Value between [0,1]
 * @param p0 
 * @param p1 
 * @param p2 
 * @param p3 
 * @param pos 
 * @param deriv 
 */
void getCatmullRomPoint(float t, float* p0, float* p1, float* p2, float* p3, float* pos, float* deriv) {

	// catmull-rom matrix
	float m[4][4] = { {-0.5f,  1.5f, -1.5f,  0.5f},
					  { 1.0f, -2.5f,  2.0f, -0.5f},
					  {-0.5f,  0.0f,  0.5f,  0.0f},
					  { 0.0f,  1.0f,  0.0f,  0.0f} };
	float vt[4] = { t * t * t,t * t,t,1 };
	float dvt[4] = { 3 * t * t,2 * t,1,0 };
	float p[4];
	float x = 0, y = 0, z = 0, dx = 0, dy = 0, dz = 0;
	float auxvec[4] = { p0[0], p1[0], p2[0], p3[0] };

	multMatrixVector((float*)m, auxvec, p);
	x = multvec(vt, p);
	dx = multvec(dvt, p);

	auxvec[0] = p0[1];
	auxvec[1] = p1[1];
	auxvec[2] = p2[1];
	auxvec[3] = p3[1];

	multMatrixVector((float*)m, auxvec, p);
	y = multvec(vt, p);
	dy = multvec(dvt, p);

	auxvec[0] = p0[2];
	auxvec[1] = p1[2];
	auxvec[2] = p2[2];
	auxvec[3] = p3[2];

	multMatrixVector((float*)m, auxvec, p);
	z = multvec(vt, p);
	dz = multvec(dvt, p);

	pos[0] = x;
	pos[1] = y;
	pos[2] = z;
	deriv[0] = dx;
	deriv[1] = dy;
	deriv[2] = dz;
}

/**
 * @brief Get the Global Catmull Rom Point object based on global t
 * 
 * @param gt 
 * @param pos 
 * @param deriv 
 */
void getGlobalCatmullRomPoint(float gt, float* pos, float* deriv) {

	float t = gt * POINT_COUNT; // this is the real global t
	int index = floor(t);  // which segment
	t = t - index; // where within  the segment

	// indices store the points
	int indices[4];
	indices[0] = (index + POINT_COUNT - 1) % POINT_COUNT;
	indices[1] = (indices[0] + 1) % POINT_COUNT;
	indices[2] = (indices[1] + 1) % POINT_COUNT;
	indices[3] = (indices[2] + 1) % POINT_COUNT;
	if(working==0)
		getCatmullRomPoint(t, p[indices[0]], p[indices[1]], p[indices[2]], p[indices[3]], pos, deriv);
	else if(working==1)
		getCatmullRomPoint(t, p2[indices[0]], p2[indices[1]], p2[indices[2]], p2[indices[3]], pos, deriv);
	else if (working == 2)
		getCatmullRomPoint(t, p3[indices[0]], p3[indices[1]], p3[indices[2]], p3[indices[3]], pos, deriv);
	else if (working == 3)
		getCatmullRomPoint(t, p4[indices[0]], p4[indices[1]], p4[indices[2]], p4[indices[3]], pos, deriv);
	else if (working == 4)
		getCatmullRomPoint(t, p5[indices[0]], p5[indices[1]], p5[indices[2]], p5[indices[3]], pos, deriv);
}

/**
 * @brief Draw the Catmull curve, helps with visualisation
 * 
 */
void renderCatmullRomCurve() {
	glPushMatrix();
	if(working==0)
		glTranslatef(-1, -5, 0);
	else if(working==1)
		glTranslatef(1, -5, 0);
	else if (working == 2)
		glTranslatef(-1.2, 0, 0);
	else if (working == 3)
		glTranslatef(1.2, 0, 0);
	else if (working == 4)
		glTranslatef(0, 4.5, 0);
	// draw curve using line segments with GL_LINE_LOOP
	float pos[3], der[3];
	float t;
	int size = 100;
	glBegin(GL_LINE_LOOP);

	for (int i = 0; i < size; i++)
	{
		t = (1.0 / (float)size) * (float)i;
		getGlobalCatmullRomPoint(t, pos, der);
		glVertex3f(pos[0], pos[1], pos[2]);

	}
	glEnd();
	glPopMatrix();
}

/**
 * @brief Cross product between vector a and b to calculate the res vector wich is perpendicular to the first two
 * 
 * @param a Vector to be multiplied
 * @param b Vector to be multiplied
 * @param res Perpendicular Vector to Vector 1 and 2
 */
void cross_main(float* a, float* b, float* res) {
	res[0] = a[1] * b[2] - a[2] * b[1];
	res[1] = a[2] * b[0] - a[0] * b[2];
	res[2] = a[0] * b[1] - a[1] * b[0];
}

/**
 * @brief Normalized Vetor
 * 
 * @param a 
 */
void normalize_main(float* a) {

	float l = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
	a[0] = a[0] / l;
	a[1] = a[1] / l;
	a[2] = a[2] / l;
}

void renderScene(void) {
	static float t = 0;
	float pos[3], der[3];
	float m[4][4];

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
	glutWireSphere(0.5, 5, 5);
	glColor3f(0.0f, 0.0f, 1.0f);
	glPopMatrix();

	glPushMatrix();
	if (controlling == 1)
		glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(targets[1]->target[0], targets[1]->target[1], targets[1]->target[2]);
	glutWireSphere(0.5, 5, 5);
	glColor3f(0.0f, 0.0f, 1.0f);
	glPopMatrix();

	glPushMatrix();
	if (controlling == 2)
		glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(targets[2]->target[0], targets[2]->target[1], targets[2]->target[2]);
	glutWireSphere(0.5, 5, 5);
	glColor3f(0.0f, 0.0f, 1.0f);
	glPopMatrix();
	
	glPushMatrix();
	if (controlling == 3)
		glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(targets[3]->target[0], targets[3]->target[1], targets[3]->target[2]);
	glutWireSphere(0.5, 5, 5);
	glColor3f(0.0f, 0.0f, 1.0f);
	glPopMatrix();

	glPushMatrix();
	if (controlling == 4)
		glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(targets[4]->target[0], targets[4]->target[1], targets[4]->target[2]);
	glutWireSphere(0.5, 5, 5);
	glColor3f(0.0f, 0.0f, 1.0f);
	glPopMatrix();

	glColor3f(0.0f, 0.0f, 1.0f);

	if (catmullControlling == false) {
		//carmull
		working = 0;
		{
			glPushMatrix();
			if(catdebug) renderCatmullRomCurve();
			getGlobalCatmullRomPoint(t, pos, der);

			glTranslatef(pos[0], pos[1], pos[2]);

			targets[3]->target[0] = pos[0] - 1;
			targets[3]->target[1] = pos[1] - 5;
			targets[3]->target[2] = pos[2];

			glPopMatrix();
		}
		working = 1;
		{
			glPushMatrix();
			if (catdebug) renderCatmullRomCurve();
			getGlobalCatmullRomPoint(t, pos, der);

			glTranslatef(pos[0], pos[1], pos[2]);

			targets[4]->target[0] = pos[0] + 1;
			targets[4]->target[1] = pos[1] - 5;
			targets[4]->target[2] = pos[2];

			glPopMatrix();
		}
		working = 2;
		{
			glPushMatrix();
			if (catdebug) renderCatmullRomCurve();
			getGlobalCatmullRomPoint(t, pos, der);

			glTranslatef(pos[0], pos[1], pos[2]);

			targets[1]->target[0] = pos[0] - 1.2;
			targets[1]->target[1] = pos[1];
			targets[1]->target[2] = pos[2];

			glPopMatrix();
		}
		working = 3;
		{
			glPushMatrix();
			if (catdebug) renderCatmullRomCurve();
			getGlobalCatmullRomPoint(t, pos, der);

			glTranslatef(pos[0], pos[1], pos[2]);

			targets[2]->target[0] = pos[0] + 1.2;
			targets[2]->target[1] = pos[1];
			targets[2]->target[2] = pos[2];

			glPopMatrix();
		}
		working = 4;

		/*
		{
			glPushMatrix();
			//renderCatmullRomCurve();
			getGlobalCatmullRomPoint(t, pos, der);

			glTranslatef(pos[0], pos[1], pos[2]);


			targets[0]->target[0] = pos[0] ;
			targets[0]->target[1] = pos[1]+4.5;
			targets[0]->target[2] = pos[2];

			glPopMatrix();
		}
		*/
		time = glutGet(GLUT_ELAPSED_TIME);
		if (time - timebase > 10) {
			t -= stepT / length_main(der);
			timebase = time;
		}
		if (t < 0) {

			t = 1;
		}
		//-----------catmull
	}
	
	
	up->draw();
	down->draw();
	
	up->multiUpdate(targets,obj_center,obj_size);
	down->multiUpdate(targets, obj_center, obj_size);
   
	drawAxis();

	glPushMatrix();
	
	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslatef(obj_center[0], obj_center[1], obj_center[2]);
	glutWireSphere(obj_size, 7, 7);
	glColor3f(0.0f, 0.0f, 1.0f);
	glPopMatrix();

	glColor3f(0.0f, 0.0f, 1.0f);

	// End of frame
	glutSwapBuffers();
}

int main(int argc, char** argv) {

	controlling = 0;
	qtargets = 5;

	//skeleton points
	float start[3] = { 0,0,0 };

	float up1[3] = { 0,2,0 };
	float up2[3] = { 0,3,0 };
	float up3[3] = { 0,4,0 };
	float up4[3] = { 0,4.5,0 };
	float up5[3] = { -1,3,0 };
	float up6[3] = { -1.2,1,0 };
	float up7[3] = { -1.2,-1,0 };
	float up8[3] = { 1,3,0 };
	float up9[3] = { 1.2,1,0 };
	float up10[3] = { 1.2,-1,0 };
	float up11[3] = { 0,4.5,1 };
	
	float down1[3] = { 0,-2,0 };
	float down2[3] = { -1,-2,0 };
	float down3[3] = { -1,-4,0 };
	float down4[3] = { -1,-6,0 };
	float down5[3] = { 1,-2,0 };
	float down6[3] = { 1,-4,0 };
	float down7[3] = { 1,-6,0 };

	//Parte inferior
	float angle_pernas_down[3] = { 0,-1,0 };
	float angle_pernas_up[3] = { 0,1,0 };
	float angle_pernas_left[3] = { -1,0,0 };
	float angle_pernas_right[3] = { 1,0,0 };
	float angle_pernas_front[3] = { 0,0,1 };
	float angle_pernas_back[3] = { 0,0,-1 };

	float angle_vector_null[3] = { 0,0,0 };

	//Parte superior
	up = new skeleton(start, up1,0.01, angle_pernas_down, 0.01, angle_pernas_up,true,true);
	//Tronco
	up->addChildren(up2, 0.1, angle_pernas_down, 0.1, angle_pernas_up, true, true);
	//Cabe�a
	up->children.at(0)->addChildren(up3, 0.1, angle_pernas_down, 0.1, angle_pernas_up, true, true);
	up->children.at(0)->children.at(0)->addChildren(up4, 0.4, angle_pernas_down, 0.4, angle_pernas_up, true, true);
	up->children.at(0)->children.at(0)->children.at(0)->addChildren(up11, 1.58, angle_pernas_back, 1.58, angle_pernas_front, true, true);
	//bra�o 1
	up->children.at(0)->addChildren(up5, 0.01, angle_pernas_right, 0.2, angle_pernas_left, true, true);
	up->children.at(0)->children.at(1)->addChildren(up6, 1.80, angle_pernas_front, 1.80, angle_pernas_back, true, true);
	up->children.at(0)->children.at(1)->children.at(0)->addChildren(up7, 1.58, angle_pernas_back, 1.58, angle_pernas_front, false, false);
	//bra�o 2
	up->children.at(0)->addChildren(up8, 0.2, angle_pernas_left, 0.2, angle_pernas_right, true, true);
	up->children.at(0)->children.at(2)->addChildren(up9, 1.80, angle_pernas_front, 1.80, angle_pernas_back, true, true);
	up->children.at(0)->children.at(2)->children.at(0)->addChildren(up10, 1.58, angle_pernas_back, 1.58, angle_pernas_front, false, false);

	down = new skeleton(start, down1, 0.05, angle_pernas_up, 0.05, angle_pernas_down, true, true);
	//Perna 1

	down->addChildren(down2, 0.01, angle_pernas_right, 0.01, angle_pernas_left, true, true);
	down->children.at(0)->addChildren(down3, 1.58, angle_pernas_back, 1.58, angle_pernas_front, false, true);
	down->children.at(0)->children.at(0)->addChildren(down4, 1.58, angle_pernas_front, 1.58, angle_pernas_back, false, false);
	//Perna 2
	down->addChildren(down5, 0.01, angle_pernas_left, 0.01, angle_pernas_right, true, true);
	down->children.at(1)->addChildren(down6, 1.58, angle_pernas_back, 1.58, angle_pernas_front, false, true);
	down->children.at(1)->children.at(0)->addChildren(down7, 1.58, angle_pernas_front, 1.58, angle_pernas_back, false, false);

	//Targets
	up->children.at(0)->children.at(0)->children.at(0)->children.at(0)->setTarget(0, 4.5, 1);//Cabe�a
	up->children.at(0)->children.at(1)->children.at(0)->children.at(0)->setTarget(-1.2, -1, 0);//Bra�o 1
	up->children.at(0)->children.at(2)->children.at(0)->children.at(0)->setTarget(1.2, -1, 0);//Bra�o 2

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