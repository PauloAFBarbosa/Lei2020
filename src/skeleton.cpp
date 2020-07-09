#include "skeleton.h"
#include "Bone.h"
#include <GL/glut.h>
#include <../glm/glm.hpp>
#include <../glm/gtx/rotate_vector.hpp>

bool inwards = true;
bool outwards = true;
bool restrictions = true;




void skeleton::changeinwards() {
	inwards = !inwards;
}

void skeleton::changeoutwards() {
	outwards = !outwards;
}

void skeleton::changerestrictions() {
	restrictions = !restrictions;
}


void skeleton::addChildren(float in_end[3],float angle_in,float angle_vector_in[3], float angle_out, float angle_vector_out[3])
{
	skeleton* s = new skeleton(this->me->end, in_end,angle_in,angle_vector_in, angle_out, angle_vector_out);

	s->setParent(this);
	this->children.push_back(s);

}

void skeleton::addChildren(skeleton* child)
{
	child->setParent(this);

	this->children.push_back(child);

}

void cross_skel(float* a, float* b, float* res) {

	res[0] = a[1] * b[2] - a[2] * b[1];
	res[1] = a[2] * b[0] - a[0] * b[2];
	res[2] = a[0] * b[1] - a[1] * b[0];
}

void buildRotMatrix(float* x, float* y, float* z, float* m) {

	m[0] = x[0]; m[1] = x[1]; m[2] = x[2]; m[3] = 0;
	m[4] = y[0]; m[5] = y[1]; m[6] = y[2]; m[7] = 0;
	m[8] = z[0]; m[9] = z[1]; m[10] = z[2]; m[11] = 0;
	m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
}

void multMatrixVector(float* m, float* v, float* res) {

	for (int j = 0; j < 4; ++j) {
		res[j] = 0;
		for (int k = 0; k < 4; ++k) {
			res[j] += v[k] * m[j * 4 + k];
		}
	}

}

void skeleton::setParent(skeleton* s)
{
	this->parent = s;
}

void normalize_skel(float* a) {

	float l = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
	a[0] = a[0] / l;
	a[1] = a[1] / l;
	a[2] = a[2] / l;
}

void skeleton::draw()
{

	this->me->draw();
	for each (skeleton * s in this->children)
	{
		s->draw();
	}
}

void skeleton::setTarget(float x, float y, float z)
{
	this->target[0] = x;
	this->target[1] = y;
	this->target[2] = z;
}

void skeleton::getTarget(float* target[3])
{
	if (this->children.size() == 1)
		this->children.at(0)->getTarget(target);
	if (this->children.size() == 0)
		*target = this->target;
	if (this->children.size() > 1)
	{
		*(target) = this->target;
	}
}

//nesta funcao basta seguir um dos ramos, porque depois ele vai partir do end effort e vai recuar at� ao pai e vai correr todos os filhos
void skeleton::getEndPoints(std::vector<skeleton*>* endpoints)
{
	if (this->children.empty() != true) {
		this->children.at(0)->getEndPoints(endpoints);
	}

	if (this->children.size() == 0) {

		endpoints->push_back(this);

	}

}

void skeleton::backward() {

}

void skeleton::forward() {

}

//O return effector vai devolver a root da sub tree para ser utilizado em arvores com varios ramos talvez mude depois
void skeleton::update(float target[3], float* return_effector)
{
	std::vector<skeleton*> all;
	this->getAllSkeleton(&all);

	float r = distance(target, this->me->start);
	if (r > this->totalSize()) {
		for (int i = 0; i < all.size(); i++)
		{
			float r = distance(target, all.at(i)->me->start);
			float y = (all.at(i)->me->size) / r;

			float newx, newy, newz;
			newx = (1 - y) * all.at(i)->me->start[0] + y * target[0];
			newy = (1 - y) * all.at(i)->me->start[1] + y * target[1];
			newz = (1 - y) * all.at(i)->me->start[2] + y * target[2];
			if (i < all.size() - 1) {
				all.at(i + 1)->me->start[0] = newx;
				all.at(i + 1)->me->start[1] = newy;
				all.at(i + 1)->me->start[2] = newz;
			}

			all.at(i)->me->end[0] = newx;
			all.at(i)->me->end[1] = newy;
			all.at(i)->me->end[2] = newz;
		}
	}
	else {

		skeleton* p = this;
		float* end;
		float b[3] = { 0,0,0 };
		b[0] = all.at(0)->me->start[0];
		b[1] = all.at(0)->me->start[1];
		b[2] = all.at(0)->me->start[2];

		end = all.at(all.size() - 1)->me->end;

		float dist = distance(target, end);

		while (dist > 0.001) {
			skeleton* s_aux = all.at(all.size() - 1);
			//pn=target
			s_aux->me->end[0] = target[0];
			s_aux->me->end[1] = target[1];
			s_aux->me->end[2] = target[2];

			//Forward
			int i = all.size() - 1;

			all.at(i)->me->end[0] = target[0];
			all.at(i)->me->end[1] = target[1];
			all.at(i)->me->end[2] = target[2];

			for (i; i >= 0; i--)
			{
				float r = distance(all.at(i)->me->end, all.at(i)->me->start);
				float y = all.at(i)->me->size / r;

				float temp1, temp2, temp3;
				temp1 = (1 - y) * all.at(i)->me->end[0] + y * all.at(i)->me->start[0];
				temp2 = (1 - y) * all.at(i)->me->end[1] + y * all.at(i)->me->start[1];
				temp3 = (1 - y) * all.at(i)->me->end[2] + y * all.at(i)->me->start[2];
				all.at(i)->me->start[0] = temp1;
				all.at(i)->me->start[1] = temp2;
				all.at(i)->me->start[2] = temp3;


				if (i != 0) {
					all.at(i - 1)->me->end[0] = temp1;
					all.at(i - 1)->me->end[1] = temp2;
					all.at(i - 1)->me->end[2] = temp3;
				}
				else if (return_effector != NULL) {
					return_effector[0] = all.at(i)->me->start[0];
					return_effector[1] = all.at(i)->me->start[1];
					return_effector[2] = all.at(i)->me->start[2];
				}

			}

			//backward

			all.at(0)->me->start[0] = b[0];
			all.at(0)->me->start[1] = b[1];
			all.at(0)->me->start[2] = b[2];

			for (i = 0; i < all.size() - 1; i++)
			{
				float r = distance(all.at(i)->me->start, all.at(i)->me->end);
				float y = all.at(i)->me->size / r;

				float temp1, temp2, temp3;

				temp1 = (1 - y) * all.at(i)->me->start[0] + y * all.at(i)->me->end[0];
				temp2 = (1 - y) * all.at(i)->me->start[1] + y * all.at(i)->me->end[1];
				temp3 = (1 - y) * all.at(i)->me->start[2] + y * all.at(i)->me->end[2];

				all.at(i)->me->end[0] = temp1;
				all.at(i)->me->end[1] = temp2;
				all.at(i)->me->end[2] = temp3;

				all.at(i + 1)->me->start[0] = temp1;
				all.at(i + 1)->me->start[1] = temp2;
				all.at(i + 1)->me->start[2] = temp3;

			}

			end = (all.at(all.size() - 1))->me->end;

			dist = distance(target, end);

		}


	}

}



void skeleton::applyRestrictions(float firstVecX, float firstVecY, float firstVecZ) {

}

void rotate(float original[3], float target_vector[3],float angle ,float res[3]) {

	glm::dvec3 v(original[0], original[1], original[2]);
	
	float perp[3];
	cross_skel(original, target_vector, perp);
	
	glm::dvec3 k(perp[0], perp[1], perp[2]);

	double theta = angle;

	glm::dvec3 rotated = glm::rotate(v, theta, k);

	res[0] = rotated.x;
	res[1] = rotated.y;
	res[2] = rotated.z;

}

bool skeleton::multiUpdateIn(float* originalX, float* originalY, float* originalZ) {
	//get all sub-roots
	std::vector<skeleton*> subRoots;
	this->getSubRoots(&subRoots);

	//Vai ser usado para guardar a posi��o do centroid
	float finalx, finaly, finalz;

	//vai guardar a posi��o original do skeleton
	float b[3];
	*(originalX) = this->me->start[0];
	*(originalY) = this->me->start[1];
	*(originalZ) = this->me->start[2];

	for each (skeleton * sub in subRoots)
	{

		std::vector<float> positions;
		for each (skeleton * children in sub->children)
		{
			//Para cada filho vai buscar a sua chain at� chegar ao fim ou at� encontrar outra sub root
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

			//------restri��es
			//vai calcular o vetor para as restri��es seguintes
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

			//------restri��es

			for (i; i >= 0; i--)
			{

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



				//-------restri�oes
				//Vai fazer a restri��o para todos menos o primeiro
				//vai verificar se o chain.at(i).start esta no sitio certo
				//vector[3] � o vetor que esta no centro do cone
				
				
				if (i < chain.size() - 1) {
					//Vai ver se o ponto esta dentro ou fora do cone
					float point[3];

					point[0] = chain.at(i)->me->start[0];
					point[1] = chain.at(i)->me->start[1];
					point[2] = chain.at(i)->me->start[2];

					float original_point[3];
					original_point[0] = chain.at(i + 1)->me->start[0];
					original_point[1] = chain.at(i + 1)->me->start[1];
					original_point[2] = chain.at(i + 1)->me->start[2];

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

						printf("entrou no if no multiIN ponto : %f %f %f angulo %f max %f \n", point[0], point[1], point[2],acos,angle);
							
						float resultado[3];
							
						rotate(vector, vector_teste, angle ,resultado);

						normalize_skel(resultado);
						printf("Vector %f %f %f  vectorTeste %f %f %f resultado %f %f %f\n", vector[0], vector[1], vector[2], vector_teste[0], vector_teste[1], vector_teste[2], resultado[0], resultado[1], resultado[2]);
						double dist;
						dist = chain.at(i)->me->size;

						resultado[0] = resultado[0] *dist + original_point[0];
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

					//preparar o vetor para a prox itera��o
					//nao precisa fazer nada no ultimo
						


					if (chain.at(i)->me->angle_vector_in[0] == 0 && chain.at(i)->me->angle_vector_in[1] == 0 && chain.at(i)->me->angle_vector_in[2] == 0) {
						vector[0] = chain.at(i)->me->start[0] - chain.at(i)->me->end[0];
						vector[1] = chain.at(i)->me->start[1] - chain.at(i)->me->end[1];
						vector[2] = chain.at(i)->me->start[2] - chain.at(i)->me->end[2];
					}
					else {
						vector[0] = chain.at(i-1)->me->angle_vector_in[0];
						vector[1] = chain.at(i-1)->me->angle_vector_in[1];
						vector[2] = chain.at(i-1)->me->angle_vector_in[2];
					}

					//vector[0] = chain.at(i - 1)->me->start[0] - chain.at(i - 1)->me->end[0];
					//vector[1] = chain.at(i - 1)->me->start[1] - chain.at(i - 1)->me->end[1];
					//vector[2] = chain.at(i - 1)->me->start[2] - chain.at(i - 1)->me->end[2];

					normalize_skel(vector);
						
					//so posso adicionar aqui � lista porque tenho de esperar que as corre��es sejam feitas pelas restri��es
					if (i == 0) {
						positions.push_back(chain.at(i)->me->start[0]);
						positions.push_back(chain.at(i)->me->start[1]);
						positions.push_back(chain.at(i)->me->start[2]);
					}
				}
				
				



				//------restri��es
			}
		}
		//Quando acabar todos os filhos vai calcular o centroid
		float sumx = 0;
		float sumy = 0;
		float sumz = 0;
		float i = 0;

		for (i = 0; i < positions.size(); i += 3)
		{

			sumx += positions.at(i);
			sumy += positions.at(i + 1);
			sumz += positions.at(i + 2);
		}


		finalx = sumx / (i / 3);
		finaly = sumy / (i / 3);
		finalz = sumz / (i / 3);
		//printf("Final %f %f %f\n", finalx,finaly,finalz);


		//Agora tenho de escolher o ponto que fica na linha entre o start da bifurca��o e o centroid. e que respeite o size
		float vector[3];
		vector[0] = finalx - sub->me->start[0];
		vector[1] = finaly - sub->me->start[1];
		vector[2] = finalz - sub->me->start[2];
		float magnitude = sqrtf(powf(vector[0], 2) + powf(vector[1], 2) + powf(vector[2], 2));

		vector[0] = vector[0] / magnitude;
		vector[1] = vector[1] / magnitude;
		vector[2] = vector[2] / magnitude;

		//Aqui posso por o ponto finalx,y,z ou posso por o ponto real, nao faz diferen�a
		sub->me->end[0] = finalx;
		sub->me->end[1] = finaly;
		sub->me->end[2] = finalz;

		sub->setTarget(finalx, finaly, finalz);


		//Agora que temos o centroid temos de por como end to pai e start de todos os filhos esse centroid
		//Aqui nao sei se ponho so os filhos, ou so o pai, ou todos
		//vou testar com todos para ja a ver
		for each (skeleton * children in sub->children)
		{
			children->me->start[0] = sub->me->start[0] + vector[0] * sub->me->size;
			children->me->start[1] = sub->me->start[1] + vector[1] * sub->me->size;
			children->me->start[2] = sub->me->start[2] + vector[2] * sub->me->size;
		}

	}
	//Ja temos todos as sub roots processadas, falta agora correr o algoritmo normal da primeira sub base at� � root principal
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

	//------restri��es
			//vai calcular o vetor para as restri��es seguintes
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

	//vector[0] = chain.at(i)->me->start[0] - chain.at(i)->me->end[0];
	//vector[1] = chain.at(i)->me->start[1] - chain.at(i)->me->end[1];
	//vector[2] = chain.at(i)->me->start[2] - chain.at(i)->me->end[2];

	normalize_skel(vector);

	//------restri��es

	for (i; i >= 0; i--)
	{
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

		
		if (i < chain.size() - 1) {
			//Vai ver se o ponto esta dentro ou fora do cone
			float point[3];

			point[0] = chain.at(i)->me->start[0];
			point[1] = chain.at(i)->me->start[1];
			point[2] = chain.at(i)->me->start[2];

			float original_point[3];
			original_point[0] = chain.at(i + 1)->me->start[0];
			original_point[1] = chain.at(i + 1)->me->start[1];
			original_point[2] = chain.at(i + 1)->me->start[2];

			//vai buscar o angulo do osso
			float angle = chain.at(i)->me->angle_in;

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

				rotate(vector, vector_teste, angle, resultado);

				normalize_skel(resultado);

				double dist;
				dist = sqrt(pow((double)point[0] - (double)chain.at(i)->me->end[0], 2) + pow((double)point[2] - (double)chain.at(i)->me->end[2], 2) + pow((double)point[2] - (double)chain.at(i)->me->end[2], 2));

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
			//preparar o vetor para a prox itera��o
			//nao precisa fazer nada no ultimo
			if (i != 0) {

				if (chain.at(i)->me->angle_vector_in[0] == 0 && chain.at(i)->me->angle_vector_in[1] == 0 && chain.at(i)->me->angle_vector_in[2] == 0) {
					vector[0] = chain.at(i - 1)->me->start[0] - chain.at(i)->me->end[0];
					vector[1] = chain.at(i - 1)->me->start[1] - chain.at(i)->me->end[1];
					vector[2] = chain.at(i - 1)->me->start[2] - chain.at(i)->me->end[2];
				}
				else {
					vector[0] = chain.at(i - 1)->me->angle_vector_in[0];
					vector[1] = chain.at(i - 1)->me->angle_vector_in[1];
					vector[2] = chain.at(i - 1)->me->angle_vector_in[2];
				}

				normalize_skel(vector);
			}

		}


	}
	return true;
}

void skeleton::multiUpdateOut(float originalX, float originalY, float originalZ) {
	//Acabou agora a parte 1 do algoritmo
		//vai come�ar na raiz principal e subir


	std::vector<skeleton*> chain_root;
	this->getAllSkeleton(&chain_root);

	chain_root.at(0)->me->start[0] = originalX;
	chain_root.at(0)->me->start[1] = originalY;
	chain_root.at(0)->me->start[2] = originalZ;

	//------restri��es
			//vai calcular o vetor para as restri��es seguintes
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

	//------restri��es

	for (int i = 0; i <= chain_root.size() - 1; i++)
	{
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
		
		
		if (i>0) {
			float point[3];

			point[0] = chain_root.at(i)->me->end[0];
			point[1] = chain_root.at(i)->me->end[1];
			point[2] = chain_root.at(i)->me->end[2];

			float original_point[3];
			original_point[0] = chain_root.at(i)->me->start[0];
			original_point[1] = chain_root.at(i)->me->start[1];
			original_point[2] = chain_root.at(i)->me->start[2];

			float angle = chain_root.at(i)->me->angle_out;

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
				rotate(vector, vector_teste, angle, resultado);

				normalize_skel(resultado);

				double dist;
				dist = chain_root.at(i)->me->size;

				resultado[0] = resultado[0] * dist + original_point[0];
				resultado[1] = resultado[1] * dist + original_point[1];
				resultado[2] = resultado[2] * dist + original_point[2];

				chain_root.at(i)->me->end[0] = resultado[0];
				chain_root.at(i)->me->end[1] = resultado[1];
				chain_root.at(i)->me->end[2] = resultado[2];

				if (i < chain_root.size() - 1) {
					chain_root.at(i + 1)->me->start[0] = resultado[0];
					chain_root.at(i + 1)->me->start[1] = resultado[1];
					chain_root.at(i + 1)->me->start[2] = resultado[2];
				}
				

				if (chain_root.at(i)->me->angle_vector_out[0] == 0 && chain_root.at(i)->me->angle_vector_out[1] == 0 && chain_root.at(i)->me->angle_vector_out[2] == 0) {
					vector[0] = chain_root.at(i)->me->end[0] - chain_root.at(i)->me->start[0];
					vector[1] = chain_root.at(i)->me->end[1] - chain_root.at(i)->me->start[1];
					vector[2] = chain_root.at(i)->me->end[2] - chain_root.at(i)->me->start[2];
				}
				else {
					vector[0] = chain_root.at(i)->me->angle_vector_out[0];
					vector[1] = chain_root.at(i)->me->angle_vector_out[1];
					vector[2] = chain_root.at(i)->me->angle_vector_out[2];
				}

				

				normalize_skel(vector);


			}
		}
		
	}
	//Agora vai ter de aplicar esta algoritmo para todas as sub chains que aparecem
	//vai guardar os sub_roots a processar
	std::vector<skeleton*> processar;
	//Vai por dentro do processar o primeiro sub root
	processar.push_back(chain_root.at(chain_root.size() - 1));


	while (processar.empty() != true)
	{
		//printf("Processar tem %d elementos\n", processar.size());
		//vai buscar o primeiro elemento a processar e tira-o da lista
		skeleton* a_processar = processar.at(0);
		processar.erase(processar.begin());


		for each (skeleton * children in a_processar->children)
		{
			std::vector<skeleton*> chain_2;
			children->getAllSkeleton(&chain_2);

			//vou testar aplicar agora coisas aqui e ver se a parte de cima da arvore funciona bem 

			//------restri��es
			//vai calcular o vetor para as restri��es seguintes
			float vector[3];

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

			//------restri��es

			for (int i = 0; i <= chain_2.size() - 1; i++)
			{
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
				
				if (i>0) {
					float point[3];

					point[0] = chain_2.at(i)->me->end[0];
					point[1] = chain_2.at(i)->me->end[1];
					point[2] = chain_2.at(i)->me->end[2];

					float original_point[3];
					original_point[0] = chain_2.at(i)->me->start[0];
					original_point[1] = chain_2.at(i)->me->start[1];
					original_point[2] = chain_2.at(i)->me->start[2];

					float angle = chain_2.at(i)->me->angle_out;

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
						printf("entrou no if no multiOUT\n");
						float resultado[3];
						rotate(vector, vector_teste, angle, resultado);

						normalize_skel(resultado);

						double dist;
						dist = chain_2.at(i)->me->size;

						resultado[0] = resultado[0] * dist + original_point[0];
						resultado[1] = resultado[1] * dist + original_point[1];
						resultado[2] = resultado[2] * dist + original_point[2];

						chain_2.at(i)->me->end[0] = resultado[0];
						chain_2.at(i)->me->end[1] = resultado[1];
						chain_2.at(i)->me->end[2] = resultado[2];
						if (i < chain_2.size() - 1) {
							chain_2.at(i + 1)->me->start[0] = resultado[0];
							chain_2.at(i + 1)->me->start[1] = resultado[1];
							chain_2.at(i + 1)->me->start[2] = resultado[2];
						}
						

						if (chain_2.at(i)->me->angle_vector_out[0] == 0 && chain_2.at(i)->me->angle_vector_out[1] == 0 && chain_2.at(i)->me->angle_vector_out[2] == 0) {
							vector[0] = chain_2.at(i)->me->end[0] - chain_2.at(i)->me->start[0];
							vector[1] = chain_2.at(i)->me->end[1] - chain_2.at(i)->me->start[1];
							vector[2] = chain_2.at(i)->me->end[2] - chain_2.at(i)->me->start[2];
						}
						else {
							vector[0] = chain_2.at(i)->me->angle_vector_out[0];
							vector[1] = chain_2.at(i)->me->angle_vector_out[1];
							vector[2] = chain_2.at(i)->me->angle_vector_out[2];
						}
						

						normalize_skel(vector);


					}
				
				}
				

			}
			if (chain_2.at(chain_2.size() - 1)->children.size() > 1) {
				processar.push_back(chain_2.at(chain_2.size() - 1));
			}
		}

	}
}

void skeleton::multiUpdate(skeleton* targets[4])
{
	float vec1[3] = {0,1,0};
	float vec2[3] = { 1,0,0 };
	float res[3];

	rotate(vec1, vec2, 0.7, res);

	printf("O resultado deu %f %f %f \n", res[0], res[1], res[2]);

	float originalx, originaly, originalz;
	for (int k = 0; k < 1; k++)
	{

		bool ret = this->multiUpdateIn(&originalx, &originaly, &originalz);

		this->multiUpdateOut(originalx, originaly, originalz);

		

	}



}


void skeleton::getSubRoots(std::vector<skeleton*>* subRoots)
{
	if (this->children.size() > 1)
	{
		for each (skeleton * children in this->children)
		{

			children->getSubRoots(subRoots);
		}

		subRoots->push_back(this);
	}
	else if (this->children.size() == 1) {
		this->children.at(0)->getSubRoots(subRoots);
	}



}


float skeleton::distance(float p1[3], float p2[3])
{
	return sqrtf(pow((p1[0] - p2[0]), 2) + pow((p1[1] - p2[1]), 2) + pow((p1[2] - p2[2]), 2));
}

float skeleton::totalSize()
{
	float result = this->me->size;
	if (this->children.size() == 1)
		result += this->children.at(0)->totalSize();

	return result;
}

float* skeleton::getEndEffector()
{
	if (this->children.empty()) {
		return this->me->end;
	}
	else
	{
		this->children.at(0)->getEndEffector();
	}

}

void skeleton::getAllSkeleton(std::vector<skeleton*>* ret)
{
	ret->push_back(this);
	if (this->children.empty())
		return;
	if (this->children.size() == 1) {
		this->children.at(0)->getAllSkeleton(ret);
	}

}

skeleton::skeleton(float in_start[3], float in_end[3],float angle_in, float angle_vector_in[3], float angle_out, float angle_vector_out[3])
{
	Bone* b = new Bone(in_start, in_end,angle_in, angle_vector_in, angle_out, angle_vector_out);
	this->me = b;
	this->parent = NULL;
	std::vector<skeleton*> children;
	this->children = children;

}

skeleton::~skeleton()
{
	//delete [] this->target;
	//delete [] this->me;
	//delete [] this->parent;

	for each (skeleton * s in this->children)
	{
		s->~skeleton();
		delete s;
	}
}
