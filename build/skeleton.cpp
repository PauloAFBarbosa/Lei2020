#include "skeleton.h"
#include <../build/Bone.h>





void skeleton::addChildren(float in_end[3])
{
	skeleton * s = new skeleton(this->me->end, in_end);

	s->setParent(this);
	this->children.push_back(s);
	
}

void skeleton::setParent(skeleton * s)
{
	this->parent = s;
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

void skeleton::getTarget(float * target[3])
{
	if (this->children.size() == 1)
		this->children.at(0)->getTarget(target);
	if (this->children.size() == 0)
		*target=this->target;
	if (this->children.size()>1)
	{
		*(target)= this->target;
	}
}

//nesta funcao basta seguir um dos ramos, porque depois ele vai partir do end effort e vai recuar até ao pai e vai correr todos os filhos
void skeleton::getEndPoints(std::vector<skeleton*> * endpoints)
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
void skeleton::update(float target[3],float * return_effector)
{
	std::vector<skeleton *> all;
	this->getAllSkeleton(&all);

	float r = distance(target, this->me->start);
	if (r > this->totalSize()) {
		for (int i = 0; i < all.size(); i++)
		{
			float r = distance(target, all.at(i)->me->start);
			float y = (all.at(i)->me->size) / r;

			float newx, newy, newz;
			newx = (1 - y)*all.at(i)->me->start[0]+ y * target[0];
			newy = (1 - y)*all.at(i)->me->start[1] + y * target[1];
			newz = (1 - y)*all.at(i)->me->start[2] + y * target[2];
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
		
		skeleton * p = this;
		float * end;
		float b[3] = {0,0,0};
		b[0] = all.at(0)->me->start[0];
		b[1] = all.at(0)->me->start[1];
		b[2] = all.at(0)->me->start[2];
		
		end=all.at(all.size()-1)->me->end;

		float dist = distance(target, end);

		while (dist>0.001) {
			skeleton * s_aux=all.at(all.size() - 1);
			//pn=target
			s_aux->me->end[0] = target[0];
			s_aux->me->end[1] = target[1];
			s_aux->me->end[2] = target[2];

			//Forward
			int i = all.size() - 1;

			all.at(i)->me->end[0] = target[0];
			all.at(i)->me->end[1] = target[1];
			all.at(i)->me->end[2] = target[2];

			for (i; i>=0; i--)
			{
				float r = distance(all.at(i)->me->end, all.at(i)->me->start);
				float y = all.at(i)->me->size/ r;

				float temp1, temp2, temp3;
				temp1 = (1 - y)*all.at(i)->me->end[0] + y * all.at(i)->me->start[0];
				temp2= (1 - y)*all.at(i)->me->end[1] + y * all.at(i)->me->start[1];
				temp3= (1 - y)*all.at(i)->me->end[2] + y * all.at(i)->me->start[2];
				all.at(i)->me->start[0] = temp1;
				all.at(i)->me->start[1] = temp2;
				all.at(i)->me->start[2] = temp3;

				
				if (i != 0) {
					all.at(i - 1)->me->end[0] = temp1;
					all.at(i - 1)->me->end[1] = temp2;
					all.at(i - 1)->me->end[2] = temp3;
				}
				else if(return_effector!=NULL){
					return_effector[0] = all.at(i)->me->start[0];
					return_effector[1] = all.at(i)->me->start[1];
					return_effector[2] = all.at(i)->me->start[2];
				}
				
			}

			//backward
			
			all.at(0)->me->start[0] = b[0];
			all.at(0)->me->start[1] = b[1];
			all.at(0)->me->start[2] = b[2];

			for  ( i= 0;  i < all.size()-1; i++)
			{
				float r = distance(all.at(i)->me->start, all.at(i)->me->end);
				float y = all.at(i)->me->size / r;

				float temp1, temp2, temp3;

				temp1=(1 - y)*all.at(i)->me->start[0] +y* all.at(i)->me->end[0];
				temp2= (1 - y)*all.at(i)->me->start[1] + y * all.at(i)->me->end[1];
				temp3= (1 - y)*all.at(i)->me->start[2] + y * all.at(i)->me->end[2];

				all.at(i)->me->end[0] = temp1;
				all.at(i)->me->end[1] = temp2;
				all.at(i)->me->end[2] = temp3;
				
				all.at(i+1)->me->start[0] = temp1;
				all.at(i+1)->me->start[1] = temp2;
				all.at(i+1)->me->start[2] = temp3;
				
			}

			end =(all.at(all.size()-1))->me->end;

			dist = distance(target, end);

		}
		
		
}
	
}

void skeleton::tempSkeleton(int path, skeleton * ret) {
	
	skeleton * aux = this;
	while (aux->children.empty()==false)
	{
		if (aux->children.size() == 1) {
			ret->addChildren(aux->children.at(0)->me->end);
			ret->setTarget(aux->children.at(0)->target[0], aux->children.at(0)->target[1], aux->children.at(0)->target[2]);
			aux = aux->children.at(0);
		}
		else {
			ret->addChildren(aux->children.at(path)->me->end);
			ret->setTarget(aux->children.at(path)->target[0], aux->children.at(path)->target[1], aux->children.at(path)->target[2]);
			aux = aux->children.at(path);
			
		}
	}
	
}


void skeleton::multiUpdate()
{
	//get all sub-roots
	std::vector<skeleton *> subRoots;
	this->getSubRoots(&subRoots);

	//Vai ser usado para guardar a posição do centroid
	float finalx, finaly, finalz;

	//vai guardar a posição original do skeleton
	float b[3];
	b[0] = this->me->start[0];
	b[1] = this->me->start[1];
	b[2] = this->me->start[2];
	
	

	for (int k = 0; k < 1; k++)
	{
		for each (skeleton * sub in subRoots)
		{

			std::vector<float> positions;
			for each (skeleton * children in sub->children)
			{
				//Para cada filho vai buscar a sua chain até chegar ao fim ou até encontrar outra sub root
				std::vector<skeleton *> chain;
				children->getAllSkeleton(&chain);
				//Agora para cada filho vai aplicar o algoritmo descendente
				float *target[3];
				children->getTarget(target);

				skeleton * s_aux = chain.at(chain.size() - 1);
				//pn=target

				//Forward
				int i = chain.size() - 1;

				chain.at(i)->me->end[0] = (*target)[0];
				chain.at(i)->me->end[1] = (*target)[1];
				chain.at(i)->me->end[2] = (*target)[2];

				for (i; i >= 0; i--)
				{
					float r = distance(chain.at(i)->me->end, chain.at(i)->me->start);
					float y = chain.at(i)->me->size / r;

					float temp1, temp2, temp3;
					temp1 = (1 - y)*chain.at(i)->me->end[0] + y * chain.at(i)->me->start[0];
					temp2 = (1 - y)*chain.at(i)->me->end[1] + y * chain.at(i)->me->start[1];
					temp3 = (1 - y)*chain.at(i)->me->end[2] + y * chain.at(i)->me->start[2];
					chain.at(i)->me->start[0] = temp1;
					chain.at(i)->me->start[1] = temp2;
					chain.at(i)->me->start[2] = temp3;

					if (i != 0) {
						chain.at(i - 1)->me->end[0] = temp1;
						chain.at(i - 1)->me->end[1] = temp2;
						chain.at(i - 1)->me->end[2] = temp3;
					}
					else {
						positions.push_back(chain.at(i)->me->start[0]);
						positions.push_back(chain.at(i)->me->start[1]);
						positions.push_back(chain.at(i)->me->start[2]);

						//printf("Position %f %f %f\n", chain.at(i)->me->start[0], chain.at(i)->me->start[1], chain.at(i)->me->start[2]);

					}
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
			for each (skeleton * children in sub->children)
			{
				children->me->start[0] = sub->me->start[0] + vector[0] * sub->me->size;
				children->me->start[1] = sub->me->start[1] + vector[1] * sub->me->size;
				children->me->start[2] = sub->me->start[2] + vector[2] * sub->me->size;
			}

		}
		//Ja temos todos as sub roots processadas, falta agora correr o algoritmo normal da primeira sub base até á root principal
		//Vai buscar a chain da raiz principal
		std::vector<skeleton *> chain;
		this->getAllSkeleton(&chain);

		float *target[3];

		this->getTarget(target);



		skeleton * s_aux = chain.at(chain.size() - 1);
		//Forward
		int i = chain.size() - 1;

		chain.at(i)->me->end[0] = (*target)[0];
		chain.at(i)->me->end[1] = (*target)[1];
		chain.at(i)->me->end[2] = (*target)[2];

		for (i; i >= 0; i--)
		{
			float r = distance(chain.at(i)->me->end, chain.at(i)->me->start);
			float y = chain.at(i)->me->size / r;

			float temp1, temp2, temp3;
			temp1 = (1 - y)*chain.at(i)->me->end[0] + y * chain.at(i)->me->start[0];
			temp2 = (1 - y)*chain.at(i)->me->end[1] + y * chain.at(i)->me->start[1];
			temp3 = (1 - y)*chain.at(i)->me->end[2] + y * chain.at(i)->me->start[2];
			chain.at(i)->me->start[0] = temp1;
			chain.at(i)->me->start[1] = temp2;
			chain.at(i)->me->start[2] = temp3;


			if (i != 0) {
				chain.at(i - 1)->me->end[0] = temp1;
				chain.at(i - 1)->me->end[1] = temp2;
				chain.at(i - 1)->me->end[2] = temp3;
			}
		}

		//Acabou agora a parte 1 do algoritmo
		//vai começar na raiz principal e subir

		std::vector<skeleton *> chain_root;
		this->getAllSkeleton(&chain_root);

		chain_root.at(0)->me->start[0] = b[0];
		chain_root.at(0)->me->start[1] = b[1];
		chain_root.at(0)->me->start[2] = b[2];

		for (i = 0; i < chain_root.size() - 1; i++)
		{
			float r = distance(chain_root.at(i)->me->start, chain_root.at(i)->me->end);
			float y = chain_root.at(i)->me->size / r;

			float temp1, temp2, temp3;

			temp1 = (1 - y)*chain_root.at(i)->me->start[0] + y * chain_root.at(i)->me->end[0];
			temp2 = (1 - y)*chain_root.at(i)->me->start[1] + y * chain_root.at(i)->me->end[1];
			temp3 = (1 - y)*chain_root.at(i)->me->start[2] + y * chain_root.at(i)->me->end[2];

			chain_root.at(i)->me->end[0] = temp1;
			chain_root.at(i)->me->end[1] = temp2;
			chain_root.at(i)->me->end[2] = temp3;

			chain_root.at(i + 1)->me->start[0] = temp1;
			chain_root.at(i + 1)->me->start[1] = temp2;
			chain_root.at(i + 1)->me->start[2] = temp3;
		}
		//Agora vai ter de aplicar esta algoritmo para todas as sub chains que aparecem
		//vai guardar os sub_roots a processar
		std::vector<skeleton *> processar;
		//Vai por dentro do processar o primeiro sub root
		processar.push_back(chain_root.at(chain_root.size() - 1));


		while (processar.empty() != true)
		{
			//printf("Processar tem %d elementos\n", processar.size());
			//vai buscar o primeiro elemento a processar e tira-o da lista
			skeleton * a_processar = processar.at(0);
			processar.erase(processar.begin());


			for each (skeleton * children in a_processar->children)
			{
				std::vector<skeleton *> chain_2;
				children->getAllSkeleton(&chain_2);



				for (i = 0; i < chain_2.size() - 1; i++)
				{
					float r = distance(chain_2.at(i)->me->start, chain_2.at(i)->me->end);
					float y = chain_2.at(i)->me->size / r;

					float temp1, temp2, temp3;

					temp1 = (1 - y)*chain_2.at(i)->me->start[0] + y * chain_2.at(i)->me->end[0];
					temp2 = (1 - y)*chain_2.at(i)->me->start[1] + y * chain_2.at(i)->me->end[1];
					temp3 = (1 - y)*chain_2.at(i)->me->start[2] + y * chain_2.at(i)->me->end[2];

					chain_2.at(i)->me->end[0] = temp1;
					chain_2.at(i)->me->end[1] = temp2;
					chain_2.at(i)->me->end[2] = temp3;

					chain_2.at(i + 1)->me->start[0] = temp1;
					chain_2.at(i + 1)->me->start[1] = temp2;
					chain_2.at(i + 1)->me->start[2] = temp3;
				}
				if (chain_2.at(chain_2.size() - 1)->children.size() > 1) {
					processar.push_back(chain_2.at(chain_2.size() - 1));
				}
			}

		}
	}

	

}

void skeleton::getSubRoots(std::vector<skeleton*>* subRoots)
{
	if (this->children.size()>1)
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
	if(this->children.size()==1)
		result += this->children.at(0)->totalSize();
	
	return result;
}

float * skeleton::getEndEffector()
{
	if (this->children.empty()) {
		return this->me->end;
	}
	else
	{
		this->children.at(0)->getEndEffector();
	}
	
}

void skeleton::getAllSkeleton(std::vector<skeleton*> * ret)
{
	ret->push_back(this);
	if (this->children.empty())
		return;
	if (this->children.size() == 1) {
		this->children.at(0)->getAllSkeleton(ret);
	}
	
}

skeleton::skeleton(float in_start[3], float in_end[3])
{
	Bone* b = new Bone(in_start, in_end);
	this->me = b;
	this->parent = NULL;
	std::vector<skeleton *> children;
	this->children = children;
	
}

skeleton::~skeleton()
{
}
