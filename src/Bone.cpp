#include "Bone.h"
#include <math.h>
#include<GL/glut.h>

#include <stdio.h>



Bone::Bone()
{
	this->start[0] = 0;
	this->start[1] = 0;
	this->start[2] = 0;

	this->end[0] = 0;
	this->end[1] = 1;
	this->end[2] = 0;

	this->size = 1;
}

Bone::Bone(float in_start[3], float in_end[3],float angle_in, float angle_vector_in[3], float angle_out, float angle_vector_out[3])
{
	this->start[0] = in_start[0];
	this->start[1] = in_start[1];
	this->start[2] = in_start[2];

	this->end[0] = in_end[0];
	this->end[1] = in_end[1];
	this->end[2] = in_end[2];

	this->angle_in = angle_in;

	this->angle_vector_in[0] = angle_vector_in[0];
	this->angle_vector_in[1] = angle_vector_in[1];
	this->angle_vector_in[2] = angle_vector_in[2];

	this->angle_out = angle_out;

	this->angle_vector_out[0] = angle_vector_out[0];
	this->angle_vector_out[1] = angle_vector_out[1];
	this->angle_vector_out[2] = angle_vector_out[2];

	this->size = sqrtf(powf(end[0] - start[0], 2) + powf(end[1] - start[1], 2) + powf(end[2] - start[2], 2));


}



void cross(float* a, float* b, float* res) {

	res[0] = a[1] * b[2] - a[2] * b[1];
	res[1] = a[2] * b[0] - a[0] * b[2];
	res[2] = a[0] * b[1] - a[1] * b[0];
}


void normalize(float* a) {

	float l = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
	a[0] = a[0] / l;
	a[1] = a[1] / l;
	a[2] = a[2] / l;
}


float length(float* v) {

	float res = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	return res;

}


void Bone::draw()
{
	glPushMatrix();

	GLUquadric* qobj;
	qobj = gluNewQuadric();
	gluQuadricNormals(qobj, GLU_SMOOTH);

	float vector[3];
	float original[3] = { 0,0,1 };
	float res[3];
	//vetor entre start e end
	vector[0] = end[0] - start[0];
	vector[1] = end[1] - start[1];
	vector[2] = end[2] - start[2];

	normalize(vector);

	cross(vector, original, res);
	//printf("res %f %f %f\n", res[0], res[1], res[2]);
	//determina o angulo entre origem e o destino
	float dot = vector[0] * original[0] + vector[1] * original[1] + vector[2] * original[2];
	float lenSq1 = vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2];
	float lenSq2 = original[0] * original[0] + original[1] * original[1] + original[2] * original[2];
	float angle = acos(dot / sqrt(lenSq1 * lenSq2));
	//printf("Angulo %f\n", angle*(-57.3));


	glTranslatef(start[0], start[1], start[2]);
	//printf("Dentro do draw %f %f %f", start[0], start[1], start[2]);

	if (angle != 0 && res[0] == 0 && res[1] == 0 && res[2] == 0) {
		glRotatef(angle * (-57.3), 1, 0, 0);
	}
	else
		glRotatef(angle * (-57.3), res[0], res[1], res[2]);

	//gluSphere(qobj, 2, 10, 10);
	gluCylinder(qobj, 0.2, 0.2, this->size, 10, 10);

	glPopMatrix();
}

Bone::~Bone()
{
	//delete [] this->start;
	//delete [] this->end;
	//delete this->size;
}
