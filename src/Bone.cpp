/**
 * @file Bone.cpp
 * @author Paulo Barbosa, Francisco Lira and Joel Gama
 * @brief Bone class is the definition of a bone for the skeleton
 * @version 0.1
 * @date 2020-07-20
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "Bone.h"
#include <GL/glut.h>
#include <math.h>

#include <stdio.h>

bool debug = false;

/**
 * @brief Construct a new Bone:: Bone object
 * 
 */
Bone::Bone() {
  this->start[0] = 0;
  this->start[1] = 0;
  this->start[2] = 0;

  this->end[0] = 0;
  this->end[1] = 1;
  this->end[2] = 0;

  this->size = 1;
}

/**
 * @brief Construct a new Bone:: Bone object. A Bone is defined by 2 points, start and end point.
 * 
 * @param in_start Array with x,y,z coordinates defining position of start point
 * @param in_end Array with x,y,z coordinates defining position of end point
 * @param angle_in Limit of movement for the bone, in rad
 * @param angle_vector_in Vetor that defines the center of movement for the bone
 * @param angle_out Same as angle_in but for when is doing the out algorith
 * @param angle_vector_out Same as angle_vector_in but for doing the out algorith
 * @param fixed_in Define if you what that the vetor rotate with the bone or stay the same
 * @param fixed_out Same as fixed_in but for doing the out algorithm
 */
Bone::Bone(float in_start[3], float in_end[3], float angle_in, float angle_vector_in[3], float angle_out, float angle_vector_out[3], bool fixed_in, bool fixed_out) {
    this->start[0] = in_start[0];
    this->start[1] = in_start[1];
    this->start[2] = in_start[2];

    this->end[0] = in_end[0];
    this->end[1] = in_end[1];
    this->end[2] = in_end[2];

    this->original_vec[0] = this->end[0] - this->start[0];
    this->original_vec[1] = this->end[1] - this->start[1];
    this->original_vec[2] = this->end[2] - this->start[2];

    this->size = sqrtf(powf(original_vec[0], 2) + powf(original_vec[1], 2) + powf(original_vec[2], 2));

    this->angle_in = angle_in;

    this->angle_vector_in[0] = angle_vector_in[0];
    this->angle_vector_in[1] = angle_vector_in[1];
    this->angle_vector_in[2] = angle_vector_in[2];

    this->original_in[0] = angle_vector_in[0];
    this->original_in[1] = angle_vector_in[1];
    this->original_in[2] = angle_vector_in[2];

    this->angle_out = angle_out;

    this->angle_vector_out[0] = angle_vector_out[0];
    this->angle_vector_out[1] = angle_vector_out[1];
    this->angle_vector_out[2] = angle_vector_out[2];

    this->original_out[0] = angle_vector_out[0];
    this->original_out[1] = angle_vector_out[1];
    this->original_out[2] = angle_vector_out[2];

    this->fixed_in = fixed_in;
    this->fixed_out = fixed_out;
}

/**
 * @brief Cross Multiplication
 * 
 * @param a a[3] to be multiplied
 * @param b b[3] to be multiplied
 * @param res result of multiplication
 */
void cross(float *a, float *b, float *res) {
  res[0] = a[1] * b[2] - a[2] * b[1];
  res[1] = a[2] * b[0] - a[0] * b[2];
  res[2] = a[0] * b[1] - a[1] * b[0];
}

/**
 * @brief Normalization of a vector
 * 
 * @param a vector to be normalized
 */
void normalize(float *a) {
  float l = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
  a[0] = a[0] / l;
  a[1] = a[1] / l;
  a[2] = a[2] / l;
}

/**
 * @brief return the length of v
 * 
 * @param v array[3] with the x,y,z dimensions
 * @return float length of v
 */
float length(float *v) {
  float res = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  return res;
}

/**
 * @brief Draw vector along with the bones, helps for visualisation
 * 
 */
void Bone::draw_vecs() {
  // Roxo =in

  // amarelo = out

  // in
  {
    glPushMatrix();

    GLUquadric *qobj;
    qobj = gluNewQuadric();
    gluQuadricNormals(qobj, GLU_SMOOTH);

    float vector[3];

    vector[0] = this->angle_vector_in[0];
    vector[1] = this->angle_vector_in[1];
    vector[2] = this->angle_vector_in[2];

    float original[3] = {0, 0, 1};
    float res[3];

    normalize(vector);

    cross(vector, original, res);

    float dot = vector[0] * original[0] + vector[1] * original[1] +
                vector[2] * original[2];
    float lenSq1 =
        vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2];
    float lenSq2 = original[0] * original[0] + original[1] * original[1] +
                   original[2] * original[2];
    float angle = acos(dot / sqrt(lenSq1 * lenSq2));

    glTranslatef(end[0], end[1], end[2]);
    //printf("Dentro do draw %f %f %f", start[0], start[1], start[2]);

    if (angle != 0 && res[0] == 0 && res[1] == 0 && res[2] == 0) {
      glRotatef(angle * (-57.3), 1, 0, 0);
    } else
      glRotatef(angle * (-57.3), res[0], res[1], res[2]);
    glColor3f(1.0f, 0.0f, 1.0f);
    glutWireCone(0.1, 2, 5, 5);
    glColor3f(0.0f, 0.0f, 1.0f);
    glPopMatrix();
  }

  // out
  {
    glPushMatrix();

    GLUquadric *qobj;
    qobj = gluNewQuadric();
    gluQuadricNormals(qobj, GLU_SMOOTH);

    float vector[3];

    vector[0] = this->angle_vector_out[0];
    vector[1] = this->angle_vector_out[1];
    vector[2] = this->angle_vector_out[2];

    float original[3] = {0, 0, 1};
    float res[3];

    normalize(vector);

    cross(vector, original, res);

    float dot = vector[0] * original[0] + vector[1] * original[1] +
                vector[2] * original[2];
    float lenSq1 =
        vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2];
    float lenSq2 = original[0] * original[0] + original[1] * original[1] +
                   original[2] * original[2];
    float angle = acos(dot / sqrt(lenSq1 * lenSq2));

    glTranslatef(start[0], start[1], start[2]);
    // printf("Dentro do draw %f %f %f", start[0], start[1], start[2]);

    if (angle != 0 && res[0] == 0 && res[1] == 0 && res[2] == 0) {
      glRotatef(angle * (-57.3), 1, 0, 0);
    } else
      glRotatef(angle * (-57.3), res[0], res[1], res[2]);
    glColor3f(1.0f, 1.0f, 0.0f);
    glutWireCone(0.1, 2, 5, 5);
    glColor3f(0.0f, 0.0f, 1.0f);
    glPopMatrix();
  }
}

/**
 * @brief Draw the bone
 * 
 */
void Bone::draw() {
  glPushMatrix();

  GLUquadric *qobj;
  qobj = gluNewQuadric();
  gluQuadricNormals(qobj, GLU_SMOOTH);

  float vector[3];
  float original[3] = {0, 0, 1};
  float res[3];

  // vetor entre start e end
  vector[0] = end[0] - start[0];
  vector[1] = end[1] - start[1];
  vector[2] = end[2] - start[2];

  normalize(vector);

  cross(vector, original, res);
  if (debug) { printf("res %f %f %f\n", res[0], res[1], res[2]); }

  // determina o angulo entre origem e o destino
  float dot = vector[0] * original[0] + vector[1] * original[1] +
              vector[2] * original[2];
  float lenSq1 =
      vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2];
  float lenSq2 = original[0] * original[0] + original[1] * original[1] +
                 original[2] * original[2];
  float angle = acos(dot / sqrt(lenSq1 * lenSq2));
  if (debug) { printf("Angulo %f\n", angle * (-57.3)); }

  glTranslatef(start[0], start[1], start[2]);
  if (debug) { printf("Dentro do draw %f %f %f", start[0], start[1], start[2]); }

  if (angle != 0 && res[0] == 0 && res[1] == 0 && res[2] == 0) {
    glRotatef(angle * (-57.3), 1, 0, 0);
  } else
    glRotatef(angle * (-57.3), res[0], res[1], res[2]);

  gluCylinder(qobj, 0.2, 0.2, this->size, 10, 10);

  glPopMatrix();
}

/**
 * @brief Destroy the Bone:: Bone object
 * 
 */
Bone::~Bone() { //TODO
  // delete [] this->start;
  // delete [] this->end;
  // delete this->size;
}
