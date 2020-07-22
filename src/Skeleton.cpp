/**
 * @file Skeleton.cpp
 * @author Paulo Barbosa, Francisco Lira and Joel Gama
 * @brief Skeleton class
 * @version 0.1
 * @date 2020-07-20
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "skeleton.h"
#include "Bone.h"
#include <GL/glut.h>
#include <../glm/glm.hpp>
#include <../glm/gtx/rotate_vector.hpp>

bool inwards = true;
bool outwards = true;
bool restrictions = true;
bool vecs = false;
bool debugs = false;

float obj_center_global[3];
float obj_size_global;


/**
 * @brief Adds a new children to skeleton
 * 
 * @param in_end Array with x, y, z coordinates defining position of end point
* @param angle_in Limit of movement for the bone, in rad
* @param angle_vector_in Vetor that defines the center of movement fot the bone
* @param angle_out Same as angle_in but for when is doing the out algorith
* @param angle_vector_out Same as angle_vector_in but for doing the out algorith
* @param fixed_in Define if you what that the vetor rotate with the bone or stay the same
* @param fixed_out Same as fixed_in but for doing the out algorithm
 */
void skeleton::addChildren(float in_end[3],float angle_in,float angle_vector_in[3], float angle_out, float angle_vector_out[3], bool fixed_in, bool fixed_out) {
	skeleton* s = new skeleton(this->me->end, in_end,angle_in,angle_vector_in, angle_out, angle_vector_out,fixed_in,fixed_out);

	s->setParent(this);
	this->children.push_back(s);
}

/**
 * @brief Adds a new children to skeleton
 * 
 * @param child 
 */
void skeleton::addChildren(skeleton* child) {
	child->setParent(this);

	this->children.push_back(child);
}

/**
 * @brief Cross Multiplication
 * 
 * @param a a[3] to be multiplied
 * @param b b[3] to be multiplied
 * @param res result of multiplication
 */
void cross_skel(float* a, float* b, float* res) {

	res[0] = a[1] * b[2] - a[2] * b[1];
	res[1] = a[2] * b[0] - a[0] * b[2];
	res[2] = a[0] * b[1] - a[1] * b[0];
}

/**
 * @brief Set s as parent skeleton
 * 
 * @param s *skeleton is the parent of this skeleton
 */
void skeleton::setParent(skeleton* s) {
	this->parent = s;
}

/**
 * @brief Normalization of a vector
 * 
 * @param a vector to be normalized
 */
void normalize_skel(float* a) {
	float l = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
	a[0] = a[0] / l;
	a[1] = a[1] / l;
	a[2] = a[2] / l;
}

/**
 * @brief This method draws the skeleton, if vecs is true even draw the vector of each bone
 * 
 */
void skeleton::draw() {
	this->me->draw();

	if(vecs) this->me->draw_vecs();
	for each (skeleton * s in this->children) {
		s->draw();
	}
}

/**
 * @brief Set the target point for the x,y,z coordinates
 * 
 * @param x 
 * @param y 
 * @param z 
 */
void skeleton::setTarget(float x, float y, float z) {
	this->target[0] = x;
	this->target[1] = y;
	this->target[2] = z;
}

/**
 * @brief Fill *target with the target coordinates
 * 
 * @param target Array to fill [x,y,z] coordinates
 */
void skeleton::getTarget(float* target[3]) {
	if (this->children.size() == 1)
		this->children.at(0)->getTarget(target);
	if (this->children.size() == 0)
		*target = this->target;
	if (this->children.size() > 1){
		*(target) = this->target;
	}
}

/**
 * @brief 
 * 
 * @param b 
 */
void skeleton::update_vec(Bone* b) {
	float vec1[3], vec2[3];

	vec1[0] = b->original_vec[0];
	vec1[1] = b->original_vec[1];
	vec1[2] = b->original_vec[2];

	normalize_skel(vec1);

	vec2[0] = b->end[0] - b->start[0];
	vec2[1] = b->end[1] - b->start[1];
	vec2[2] = b->end[2] - b->start[2];
	normalize_skel(vec2);

	float dot = vec1[0] * vec2[0] + vec1[1] * vec2[1] + vec1[2] * vec2[2];
	
	float angle = acosf(dot);

	float res_in[3];
	float res_out[3];

	rotate(b->original_in,vec1, vec2, angle, res_in);
	rotate(b->original_out, vec1,vec2, angle, res_out);

	normalize_skel(res_in);
	normalize_skel(res_out);

	if (debugs) {
		printf("vec1 %f %f %f \n", vec1[0], vec1[1], vec1[2]);
		printf("vec2 %f %f %f \n", vec2[0], vec2[1], vec2[2]);

		printf("Angle: %f\n", angle);

		printf("Res in %f %f %f \n", res_in[0], res_in[1], res_in[2]);
		printf("Res out %f %f %f \n", res_out[0], res_out[1], res_out[2]);

		printf("start %f %f %f \n", b->start[0], b->start[1], b->start[2]);
		printf("end   %f %f %f \n", b->end[0], b->end[1], b->end[2]);

		printf("------\n");
	}
	
	if (b->fixed_in == false) {
		b->angle_vector_in[0] = res_in[0];
		b->angle_vector_in[1] = res_in[1];
		b->angle_vector_in[2] = res_in[2];
	}
	if (b->fixed_out == false) {
		b->angle_vector_out[0] = res_out[0];
		b->angle_vector_out[1] = res_out[1];
		b->angle_vector_out[2] = res_out[2];
	}
}

/**
 * @brief Rotates a vector associated to a Bone
 * 
 * @param original Vector components [x,y,z]
 * @param from Orientation of Bone before rotation
 * @param to Orientation of Bone after rotation
 * @param angle Angle of rotation between from and to to rotate (can be smaller or bigger that the angle between them)
 * @param res Result of rotation of original components with from, to and angle information
 */
void skeleton::rotate(float original[3],float from[3], float to[3],float angle ,float res[3]) {
	
	glm::dvec3 v(original[0], original[1], original[2]);
	normalize_skel(from);
	normalize_skel(to);
	float perp[3];
	cross_skel(from, to, perp);
	normalize_skel(perp);
	glm::dvec3 k(perp[0], perp[1], perp[2]);

	double theta = angle;
	
	if ((perp[0] != 0 || perp[1] != 0 || perp[2] != 0)&& angle >0.00001) {
		glm::dvec3 rotated = glm::rotate(v, theta, k);

		res[0] = rotated.x;
		res[1] = rotated.y;
		res[2] = rotated.z;

		//printf("Rotate angulo %f vec original %f %f %f eixo %f %f %f resultado %f %f %f\n", theta, original[0], original[1], original[2], perp[0], perp[1], perp[2], res[0], res[1], res[2]);
	}
	else {
		res[0] = original[0];
		res[1] = original[1];
		res[2] = original[2];
	}
}

/**
 * @brief Define the restrictions in the in part of the algorithm
 * 
 * @param vector Same angle_vec_in vector as in the creation of Bone
 * @param chain All parts of the skeleton from the end until a bifurcation or a junction, or between two of this
 * @param i Size of the chain -1
 * @param positions Positions to calculate the centroid
 */
void skeleton::restrictions_in(float vector[3], std::vector<skeleton*> chain, int i, std::vector<float> * positions) {
	
		//Vai ver se o ponto esta dentro ou fora do cone
		float point[3];

		point[0] = chain.at(i)->me->start[0];
		point[1] = chain.at(i)->me->start[1];
		point[2] = chain.at(i)->me->start[2];

		float original_point[3];
		original_point[0] = chain.at(i)->me->end[0];
		original_point[1] = chain.at(i)->me->end[1];
		original_point[2] = chain.at(i)->me->end[2];

		float angle = chain.at(i)->me->angle_in;

		float vector_teste[3];
		vector_teste[0] = point[0] - original_point[0];
		vector_teste[1] = point[1] - original_point[1];
		vector_teste[2] = point[2] - original_point[2];

		normalize_skel(vector_teste);

		float dot = vector[0] * vector_teste[0] + vector[1] * vector_teste[1] + vector[2] * vector_teste[2];
		//dot = (dot < -1.0 ? -1.0 : (dot > 1.0 ? 1.0 : dot));
		float acos = acosf(dot);

		//printf("Acos %f \n", acos);
		//0,78 sao 45 graus, se tiver maior que 45 graus esta fora e vai ter de fazer alguma coisa
		if (acos > angle) {

			float resultado[3];

			rotate(vector, vector, vector_teste, angle, resultado);

			normalize_skel(resultado);

			double dist;
			dist = chain.at(i)->me->size;

			resultado[0] = resultado[0] * dist + original_point[0];
			resultado[1] = resultado[1] * dist + original_point[1];
			resultado[2] = resultado[2] * dist + original_point[2];

			chain.at(i)->me->start[0] = resultado[0];
			chain.at(i)->me->start[1] = resultado[1];
			chain.at(i)->me->start[2] = resultado[2];
			if (i != 0) {
				chain.at(i - 1)->me->end[0] = resultado[0];
				chain.at(i - 1)->me->end[1] = resultado[1];
				chain.at(i - 1)->me->end[2] = resultado[2];
			}
		}
		//preparar o vetor para a prox iteração
		//nao precisa fazer nada no ultimo

		if (chain.at(i)->me->angle_vector_in[0] == 0 && chain.at(i)->me->angle_vector_in[1] == 0 && chain.at(i)->me->angle_vector_in[2] == 0) {
			vector[0] = chain.at(i)->me->start[0] - chain.at(i)->me->end[0];
			vector[1] = chain.at(i)->me->start[1] - chain.at(i)->me->end[1];
			vector[2] = chain.at(i)->me->start[2] - chain.at(i)->me->end[2];
		}
		else {
			// este if esta aqui para ele nao tentar preparar o vector na ultima iteração --- se nao da erro
			if (i > 0) {
				vector[0] = chain.at(i - 1)->me->angle_vector_in[0];
				vector[1] = chain.at(i - 1)->me->angle_vector_in[1];
				vector[2] = chain.at(i - 1)->me->angle_vector_in[2];
			}
		}
		normalize_skel(vector);

		//so posso adicionar aqui à lista porque tenho de esperar que as correções sejam feitas pelas restrições
		if (i == 0) {
			positions->push_back(chain.at(i)->me->start[0]);
			positions->push_back(chain.at(i)->me->start[1]);
			positions->push_back(chain.at(i)->me->start[2]);
		}
}

/**
 * @brief Does the in part of the Algorithm
 * 
 * @param originalX 
 * @param originalY 
 * @param originalZ 
 */
bool skeleton::multiUpdateIn(float* originalX, float* originalY, float* originalZ) {
	//get all sub-roots
	std::vector<skeleton*> subRoots;
	this->getSubRoots(&subRoots);

	//Vai ser usado para guardar a posição do centroid
	float finalx, finaly, finalz;

	//vai guardar a posição original do skeleton
	float b[3];
	*(originalX) = this->me->start[0];
	*(originalY) = this->me->start[1];
	*(originalZ) = this->me->start[2];

	for each (skeleton * sub in subRoots) {

		std::vector<float> positions;
		for each (skeleton * children in sub->children) {
			//Para cada filho vai buscar a sua chain até chegar ao fim ou até encontrar outra sub root
			std::vector<skeleton*> chain;
			children->getAllSkeleton(&chain);
			//Agora para cada filho vai aplicar o algoritmo descendente
			float* target[3];
			children->getTarget(target);

			skeleton* s_aux = chain.at(chain.size() - 1);
			//pn=target

			//Forward
			int i = chain.size() - 1;
			skeleton* recover = chain.at(i);
			
			chain.at(i)->me->end[0] = (*target)[0];
			chain.at(i)->me->end[1] = (*target)[1];
			chain.at(i)->me->end[2] = (*target)[2];

			//------restrições
			//vai calcular o vetor para as restrições seguintes
			float vector[3];
			if (chain.at(i)->me->angle_vector_in[0] == 0 && chain.at(i)->me->angle_vector_in[1] == 0 && chain.at(i)->me->angle_vector_in[2] == 0) {
				vector[0] = chain.at(i)->me->start[0] - chain.at(i)->me->end[0];
				vector[1] = chain.at(i)->me->start[1] - chain.at(i)->me->end[1];
				vector[2] = chain.at(i)->me->start[2] - chain.at(i)->me->end[2];
			}
			else {
				vector[0] = chain.at(i)->me->angle_vector_in[0];
				vector[1] = chain.at(i)->me->angle_vector_in[1];
				vector[2] = chain.at(i)->me->angle_vector_in[2];
			}
			normalize_skel(vector);

			for (i; i >= 0; i--) {
				float r = distance(chain.at(i)->me->end, chain.at(i)->me->start);
				float y = chain.at(i)->me->size / r;

				float temp1, temp2, temp3;
				temp1 = (1 - y) * chain.at(i)->me->end[0] + y * chain.at(i)->me->start[0];
				temp2 = (1 - y) * chain.at(i)->me->end[1] + y * chain.at(i)->me->start[1];
				temp3 = (1 - y) * chain.at(i)->me->end[2] + y * chain.at(i)->me->start[2];
				
				chain.at(i)->me->start[0] = temp1;
				chain.at(i)->me->start[1] = temp2;
				chain.at(i)->me->start[2] = temp3;

				if (i != 0) {
					chain.at(i - 1)->me->end[0] = temp1;
					chain.at(i - 1)->me->end[1] = temp2;
					chain.at(i - 1)->me->end[2] = temp3;
				}
				
				restrictions_in(vector, chain, i, &positions);
			}
		}
		//Quando acabar todos os filhos vai calcular o centroid
		float sumx = 0;
		float sumy = 0;
		float sumz = 0;
		float i = 0;

		for (i = 0; i < positions.size(); i += 3) {

			sumx += positions.at(i);
			sumy += positions.at(i + 1);
			sumz += positions.at(i + 2);
		}

		finalx = sumx / (i / 3);
		finaly = sumy / (i / 3);
		finalz = sumz / (i / 3);
		if (debugs) { printf("Final %f %f %f\n", finalx, finaly, finalz); }


		//Agora tenho de escolher o ponto que fica na linha entre o start da bifurcação e o centroid. e que respeite o size
		float vector[3];
		vector[0] = finalx - sub->me->start[0];
		vector[1] = finaly - sub->me->start[1];
		vector[2] = finalz - sub->me->start[2];
		float magnitude = sqrtf(powf(vector[0], 2) + powf(vector[1], 2) + powf(vector[2], 2));

		vector[0] = vector[0] / magnitude;
		vector[1] = vector[1] / magnitude;
		vector[2] = vector[2] / magnitude;

		//Aqui posso por o ponto finalx,y,z ou posso por o ponto real, nao faz diferença
		sub->me->end[0] = finalx;
		sub->me->end[1] = finaly;
		sub->me->end[2] = finalz;

		sub->setTarget(finalx, finaly, finalz);


		//Agora que temos o centroid temos de por como end to pai e start de todos os filhos esse centroid
		//Aqui nao sei se ponho so os filhos, ou so o pai, ou todos
		//vou testar com todos para ja a ver
		for each (skeleton * children in sub->children) {
			children->me->start[0] = sub->me->start[0] + vector[0] * sub->me->size;
			children->me->start[1] = sub->me->start[1] + vector[1] * sub->me->size;
			children->me->start[2] = sub->me->start[2] + vector[2] * sub->me->size;
		}
	}
	//Ja temos todos as sub roots processadas, falta agora correr o algoritmo normal da primeira sub base até á root principal
	//Vai buscar a chain da raiz principal
	std::vector<skeleton*> chain;
	this->getAllSkeleton(&chain);

	float* target[3];

	this->getTarget(target);

	skeleton* s_aux = chain.at(chain.size() - 1);

	int i = chain.size() - 1;

	chain.at(i)->me->end[0] = (*target)[0];
	chain.at(i)->me->end[1] = (*target)[1];
	chain.at(i)->me->end[2] = (*target)[2];

	//------restrições
			//vai calcular o vetor para as restrições seguintes
	float vector[3];

	if (chain.at(i)->me->angle_vector_in[0] == 0 && chain.at(i)->me->angle_vector_in[1] == 0 && chain.at(i)->me->angle_vector_in[2] == 0) {
		vector[0] = chain.at(i)->me->start[0] - chain.at(i)->me->end[0];
		vector[1] = chain.at(i)->me->start[1] - chain.at(i)->me->end[1];
		vector[2] = chain.at(i)->me->start[2] - chain.at(i)->me->end[2];
	}
	else {
		vector[0] = chain.at(i)->me->angle_vector_in[0];
		vector[1] = chain.at(i)->me->angle_vector_in[1];
		vector[2] = chain.at(i)->me->angle_vector_in[2];
	}

	normalize_skel(vector);

	//------restrições

	for (i; i >= 0; i--) {

		float r = distance(chain.at(i)->me->end, chain.at(i)->me->start);
		float y = chain.at(i)->me->size / r;

		float temp1, temp2, temp3;
		temp1 = (1 - y) * chain.at(i)->me->end[0] + y * chain.at(i)->me->start[0];
		temp2 = (1 - y) * chain.at(i)->me->end[1] + y * chain.at(i)->me->start[1];
		temp3 = (1 - y) * chain.at(i)->me->end[2] + y * chain.at(i)->me->start[2];

		chain.at(i)->me->start[0] = temp1;
		chain.at(i)->me->start[1] = temp2;
		chain.at(i)->me->start[2] = temp3;

		if (i != 0) {
			chain.at(i - 1)->me->end[0] = temp1;
			chain.at(i - 1)->me->end[1] = temp2;
			chain.at(i - 1)->me->end[2] = temp3;
		}
		std::vector<float> does_nothing;
		restrictions_in(vector, chain, i, &does_nothing);
	}
	return true;
}

/**
 * @brief Verifies if a element of a chain is coliding with restrictions
 * 
 * @param chain A chain of the skeleton to be checked for colisions
 * @param i Identifies the element of the chain to be checked
 */
void skeleton::checkColision(std::vector<skeleton*> chain,int i) {
	float dist_center_bone = sqrtf(powf((obj_center_global[0] - chain.at(i)->me->end[0]), 2) + powf((obj_center_global[1] - chain.at(i)->me->end[1]), 2) + powf((obj_center_global[2] - chain.at(i)->me->end[2]), 2));
	if (dist_center_bone < obj_size_global) {
		
		float vector_center_bone[3];

		vector_center_bone[0] = chain.at(i)->me->end[0] - obj_center_global[0];
		vector_center_bone[1] = chain.at(i)->me->end[1] - obj_center_global[1];
		vector_center_bone[2] = chain.at(i)->me->end[2] - obj_center_global[2];

		normalize_skel(vector_center_bone);
		//calcular o ponto mais prox na superficie da esfera

		float point[3];
		point[0] = obj_center_global[0] + vector_center_bone[0] * obj_size_global;
		point[1] = obj_center_global[1] + vector_center_bone[1] * obj_size_global;
		point[2] = obj_center_global[2] + vector_center_bone[2] * obj_size_global;

		//vetor que vai do ultimo osso estavel ate a posição na superfice da esfera
		float stable_bone_sphere[3];
		stable_bone_sphere[0] = point[0] - chain.at(i)->me->start[0];
		stable_bone_sphere[1] = point[1] - chain.at(i)->me->start[1];
		stable_bone_sphere[2] = point[2] - chain.at(i)->me->start[2];
		normalize_skel(stable_bone_sphere);
		//com esse novo vetor vai calcular a nova posição para o osso respeitando o size do osso

		chain.at(i)->me->end[0] = chain.at(i)->me->start[0] + stable_bone_sphere[0] * chain.at(i)->me->size;
		chain.at(i)->me->end[1] = chain.at(i)->me->start[1] + stable_bone_sphere[1] * chain.at(i)->me->size;
		chain.at(i)->me->end[2] = chain.at(i)->me->start[2] + stable_bone_sphere[2] * chain.at(i)->me->size;

		if (i < chain.size() - 1) {
			chain.at(i + 1)->me->start[0] = chain.at(i)->me->end[0];
			chain.at(i+1)->me->start[1] = chain.at(i)->me->end[1];
			chain.at(i+1)->me->start[2] = chain.at(i)->me->end[2];
		}
	}
}

/**
 * @brief Define the restrictions in the out part of the algorithm
 * 
 * @param vector Same angle_vec_out vector as in the creation of Bone
 * @param chain All parts of the skeleton from the end until a bifurcation or a junction, or between two of this
 * @param i Size of the chain -1
 */
void skeleton::restrictions_out(float vector[3], std::vector<skeleton*> chain,int i) {
	
		float point[3];

		point[0] = chain.at(i)->me->end[0];
		point[1] = chain.at(i)->me->end[1];
		point[2] = chain.at(i)->me->end[2];

		float original_point[3];
		original_point[0] = chain.at(i)->me->start[0];
		original_point[1] = chain.at(i)->me->start[1];
		original_point[2] = chain.at(i)->me->start[2];

		float angle = chain.at(i)->me->angle_out;

		float vector_teste[3];
		vector_teste[0] = point[0] - original_point[0];
		vector_teste[1] = point[1] - original_point[1];
		vector_teste[2] = point[2] - original_point[2];

		normalize_skel(vector_teste);

		float dot = vector[0] * vector_teste[0] + vector[1] * vector_teste[1] + vector[2] * vector_teste[2];

		float acos = acosf(dot);

		//printf("Acos %f \n", acos);
		//0,78 sao 45 graus, se tiver maior que 45 graus esta fora e vai ter de fazer alguma coisa
		if (acos > angle) {

			float resultado[3];
			rotate(vector, vector, vector_teste, angle, resultado);

			normalize_skel(resultado);

			double dist;
			dist = chain.at(i)->me->size;

			resultado[0] = resultado[0] * dist + original_point[0];
			resultado[1] = resultado[1] * dist + original_point[1];
			resultado[2] = resultado[2] * dist + original_point[2];

			chain.at(i)->me->end[0] = resultado[0];
			chain.at(i)->me->end[1] = resultado[1];
			chain.at(i)->me->end[2] = resultado[2];

			if (i < chain.size() - 1) {
				chain.at(i + 1)->me->start[0] = resultado[0];
				chain.at(i + 1)->me->start[1] = resultado[1];
				chain.at(i + 1)->me->start[2] = resultado[2];
			}
		}

		if (chain.at(i)->me->angle_vector_out[0] == 0 && chain.at(i)->me->angle_vector_out[1] == 0 && chain.at(i)->me->angle_vector_out[2] == 0) {
			vector[0] = chain.at(i)->me->end[0] - chain.at(i)->me->start[0];
			vector[1] = chain.at(i)->me->end[1] - chain.at(i)->me->start[1];
			vector[2] = chain.at(i)->me->end[2] - chain.at(i)->me->start[2];
		}
		else {
			if (i < chain.size() - 1) {

				vector[0] = chain.at(i + 1)->me->angle_vector_out[0];
				vector[1] = chain.at(i + 1)->me->angle_vector_out[1];
				vector[2] = chain.at(i + 1)->me->angle_vector_out[2];
			}
		}
		normalize_skel(vector);
}

/**
 * @brief Does the out part of the Algorithm
 * 
 * @param originalX 
 * @param originalY 
 * @param originalZ 
 */
void skeleton::multiUpdateOut(float originalX, float originalY, float originalZ) {
	//Acabou agora a parte 1 do algoritmo
		//vai começar na raiz principal e subir

 
	std::vector<skeleton*> chain_root;
	this->getAllSkeleton(&chain_root);

	chain_root.at(0)->me->start[0] = originalX;
	chain_root.at(0)->me->start[1] = originalY;
	chain_root.at(0)->me->start[2] = originalZ;

	//------restrições
			//vai calcular o vetor para as restrições seguintes
	float vector[3];

	if (chain_root.at(0)->me->angle_vector_out[0] == 0 && chain_root.at(0)->me->angle_vector_out[1] == 0 && chain_root.at(0)->me->angle_vector_out[2] == 0) {
		vector[0] = chain_root.at(0)->me->end[0] - chain_root.at(0)->me->start[0];
		vector[1] = chain_root.at(0)->me->end[1] - chain_root.at(0)->me->start[1];
		vector[2] = chain_root.at(0)->me->end[2] - chain_root.at(0)->me->start[2];
	}
	else {
		
		vector[0] = chain_root.at(0)->me->angle_vector_out[0];
		vector[1] = chain_root.at(0)->me->angle_vector_out[1];
		vector[2] = chain_root.at(0)->me->angle_vector_out[2];
	}

	normalize_skel(vector);

	//------restrições

	for (int i = 0; i <= chain_root.size() - 1; i++) {
		float r = distance(chain_root.at(i)->me->start, chain_root.at(i)->me->end);
		float y = chain_root.at(i)->me->size / r;

		float temp1, temp2, temp3;

		temp1 = (1 - y) * chain_root.at(i)->me->start[0] + y * chain_root.at(i)->me->end[0];
		temp2 = (1 - y) * chain_root.at(i)->me->start[1] + y * chain_root.at(i)->me->end[1];
		temp3 = (1 - y) * chain_root.at(i)->me->start[2] + y * chain_root.at(i)->me->end[2];

		chain_root.at(i)->me->end[0] = temp1;
		chain_root.at(i)->me->end[1] = temp2;
		chain_root.at(i)->me->end[2] = temp3;
		if (chain_root.size() - 1 != i) {
			chain_root.at(i + 1)->me->start[0] = temp1;
			chain_root.at(i + 1)->me->start[1] = temp2;
			chain_root.at(i + 1)->me->start[2] = temp3;
		}
		
		//restrições
		restrictions_out(vector, chain_root, i);
		
		//vai verificar se o ponto esta dentro do objecto
		checkColision(chain_root,i);

		//update_vec
		update_vec(chain_root.at(i)->me);
	}
	//Agora vai ter de aplicar esta algoritmo para todas as sub chains que aparecem
	//vai guardar os sub_roots a processar
	std::vector<skeleton*> processar;
	//Vai por dentro do processar o primeiro sub root
	processar.push_back(chain_root.at(chain_root.size() - 1));


	while (processar.empty() != true) {
		//printf("Processar tem %d elementos\n", processar.size());
		//vai buscar o primeiro elemento a processar e tira-o da lista
		skeleton* a_processar = processar.at(0);
		processar.erase(processar.begin());


		for each (skeleton * children in a_processar->children) {
			std::vector<skeleton*> chain_2;
			children->getAllSkeleton(&chain_2);
			//------restrições
			//vai calcular o vetor para as restrições seguintes
			float vector[3];
			chain_2.at(0)->me->start[0] = chain_root.at(chain_root.size()-1)->me->end[0];
			chain_2.at(0)->me->start[1] = chain_root.at(chain_root.size() - 1)->me->end[1];
			chain_2.at(0)->me->start[2] = chain_root.at(chain_root.size() - 1)->me->end[2];

			if (chain_2.at(0)->me->angle_vector_out[0] == 0 && chain_2.at(0)->me->angle_vector_out[1] == 0 && chain_2.at(0)->me->angle_vector_out[2] == 0) {
				vector[0] = chain_2.at(0)->me->end[0] - chain_2.at(0)->me->start[0];
				vector[1] = chain_2.at(0)->me->end[1] - chain_2.at(0)->me->start[1];
				vector[2] = chain_2.at(0)->me->end[2] - chain_2.at(0)->me->start[2];
			}
			else {
				
				vector[0] = chain_2.at(0)->me->angle_vector_out[0];
				vector[1] = chain_2.at(0)->me->angle_vector_out[1];
				vector[2] = chain_2.at(0)->me->angle_vector_out[2];
			}
			normalize_skel(vector);

			//------restrições

			for (int i = 0; i <= chain_2.size() - 1; i++) {
				float r = distance(chain_2.at(i)->me->start, chain_2.at(i)->me->end);
				float y = chain_2.at(i)->me->size / r;

				float temp1, temp2, temp3;

				temp1 = (1 - y) * chain_2.at(i)->me->start[0] + y * chain_2.at(i)->me->end[0];
				temp2 = (1 - y) * chain_2.at(i)->me->start[1] + y * chain_2.at(i)->me->end[1];
				temp3 = (1 - y) * chain_2.at(i)->me->start[2] + y * chain_2.at(i)->me->end[2];

				chain_2.at(i)->me->end[0] = temp1;
				chain_2.at(i)->me->end[1] = temp2;
				chain_2.at(i)->me->end[2] = temp3;

				if (chain_2.size() - 1 != i) {
					chain_2.at(i + 1)->me->start[0] = temp1;
					chain_2.at(i + 1)->me->start[1] = temp2;
					chain_2.at(i + 1)->me->start[2] = temp3;
				}
				
				restrictions_out(vector, chain_2, i);
				
				checkColision(chain_2, i);

				update_vec(chain_2.at(i)->me);
			}
			if (chain_2.at(chain_2.size() - 1)->children.size() > 1) {
				processar.push_back(chain_2.at(chain_2.size() - 1));
			}
		}
	}
}

/**
 * @brief Controls the algorithm, the in and out part 
 * 
 * @param targets 
 * @param obj_center 
 * @param obj_size 
 */
void skeleton::multiUpdate(skeleton* targets[4],float obj_center[3],float obj_size)
{	
	obj_center_global[0]= obj_center[0];
	obj_center_global[1] = obj_center[1];
	obj_center_global[2] = obj_center[2];
	obj_size_global= obj_size;
	
	float originalx, originaly, originalz;
	for (int k = 0; k < 1; k++) {

		this->multiUpdateIn(&originalx, &originaly, &originalz);

		this->multiUpdateOut(originalx, originaly, originalz);
	}
}

/**
 * @brief Identifies and puts in subRoots all the chains in a skeleton
 * 
 * @param subRoots Vector to populate with the chains
 */
void skeleton::getSubRoots(std::vector<skeleton*>* subRoots) {
	if (this->children.size() > 1) {
		for each (skeleton * children in this->children) {
			children->getSubRoots(subRoots);
		}
		subRoots->push_back(this);
	}
	else if (this->children.size() == 1) {
		this->children.at(0)->getSubRoots(subRoots);
	}
}

/**
 * @brief Gets the distance between p1 and p1
 * 
 * @param p1 array with coordinates of point 1
 * @param p2 array with coordinates of point 2
 * @return float distance between point 1 and point 2
 */
float skeleton::distance(float p1[3], float p2[3]) {
	return sqrtf(pow((p1[0] - p2[0]), 2) + pow((p1[1] - p2[1]), 2) + pow((p1[2] - p2[2]), 2));
}

/**
 * @brief Gets the chains to use in multiupdatein and multiupdateout
 * 
 * @param ret Vector to populate with chains
 */
void skeleton::getAllSkeleton(std::vector<skeleton*>* ret) {
	ret->push_back(this);
	if (this->children.empty())
		return;
	if (this->children.size() == 1) {
		this->children.at(0)->getAllSkeleton(ret);
	}
}

/**
 * @brief Construct a new skeleton::skeleton object, all this variables are the same as in the creation of a Bone
 * 
 * @param in_start Array with x,y,z coordinates defining position of start point
 * @param in_end Array with x,y,z coordinates defining position of end point
 * @param angle_in Limit of movement for the bone, in rad
 * @param angle_vector_in Vetor that defines the center of movement fot the bone
 * @param angle_out Same as angle_in but for when is doing the out algorith
 * @param angle_vector_out Same as angle_vector_in but for doing the out algorith
 * @param fixed_in Define if you what that the vetor rotate with the bone or stay the same
 * @param fixed_out Same as fixed_in but for doing the out algorithm
 */
skeleton::skeleton(float in_start[3], float in_end[3],float angle_in, float angle_vector_in[3], float angle_out, float angle_vector_out[3], bool fixed_in, bool fixed_out) {
	Bone* b = new Bone(in_start, in_end,angle_in, angle_vector_in, angle_out, angle_vector_out, fixed_in, fixed_out);
	this->me = b;
	this->parent = NULL;
	std::vector<skeleton*> children;
	this->children = children;
}


/**
 * @brief Destroy the skeleton::skeleton object
 * 
 */
skeleton::~skeleton() {
	//delete [] this->target;
	//delete [] this->me;
	//delete [] this->parent;

	for each (skeleton * s in this->children) {
		s->~skeleton();
		delete s;
	}
}
