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
		*(target)= this->me->end;
	}
}

void skeleton::getEndPoints(std::vector<skeleton*> * endpoints)
{	
	
	for each (skeleton * s in this->children)
	{
		s->getEndPoints(endpoints);
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
				else {
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
	//get all endpoints
	std::vector<skeleton *> endPoints;
	//printf("Vai buscar endpoints\n");
	this->getEndPoints(&endPoints);
	//printf("Tem %d endpoints\n", endPoints.size());
	float endeffector[3] = {-999,-999,-999};
	

	//No caso se ser um skeleton apenas com um endpoint nao precisa fazer o while
	if (endPoints.size() == 1) {
		float * target[3];
		this->getTarget(target);
		
		this->update(*target, endeffector);
		return;
	}

	while (!endPoints.empty())
	{
		
		//vai buscar o primeiro elemento e apaga-o
		skeleton * s_aux = endPoints.at(0);
		skeleton * last=NULL;
		endPoints.erase(endPoints.begin());
		
		while (s_aux->parent!=NULL && s_aux->children.size()<=1)
		{
			
			last = s_aux;
			s_aux = s_aux->parent;
			
		}
		
		//varias posições para a root (vai ser usado para fazer a media depois)
		std::vector<float> positions;
		
		for each (skeleton * s in s_aux->children)
		{
			//printf("Vai procurar os filhos\n");
			//vai apenas chamar o update no children pelo qual ele veio comparando os entereços de memoria
			std::vector<skeleton *> all;
			s->getAllSkeleton(&all);

			float *target[3];
			s->getTarget(target);
			
			skeleton * s_aux = all.at(all.size() - 1);
			//pn=target
			s_aux->me->end[0] = (*target)[0];
			s_aux->me->end[1] = (*target)[1];
			s_aux->me->end[2] = (*target)[2];

			//Forward
			int i = all.size() - 1;

			all.at(i)->me->end[0] = (*target)[0];
			all.at(i)->me->end[1] = (*target)[1];
			all.at(i)->me->end[2] = (*target)[2];

			for (i; i >= 0; i--)
			{
				float r = distance(all.at(i)->me->end, all.at(i)->me->start);
				float y = all.at(i)->me->size / r;

				float temp1, temp2, temp3;
				temp1 = (1 - y)*all.at(i)->me->end[0] + y * all.at(i)->me->start[0];
				temp2 = (1 - y)*all.at(i)->me->end[1] + y * all.at(i)->me->start[1];
				temp3 = (1 - y)*all.at(i)->me->end[2] + y * all.at(i)->me->start[2];
				all.at(i)->me->start[0] = temp1;
				all.at(i)->me->start[1] = temp2;
				all.at(i)->me->start[2] = temp3;


				if (i != 0) {
					all.at(i - 1)->me->end[0] = temp1;
					all.at(i - 1)->me->end[1] = temp2;
					all.at(i - 1)->me->end[2] = temp3;
				}
				else {
					positions.push_back(all.at(i)->me->start[0]);
					positions.push_back(all.at(i)->me->start[1]);
					positions.push_back(all.at(i)->me->start[2]);
					

				}

			}
			
		}
		//calcular o centroid
		float sumx=0;
		float sumy=0;
		float sumz=0;
		float i = 0;
		for (i = 0; i < positions.size(); i+=3)
		{
			
			sumx +=positions.at(i);
			sumy += positions.at(i+1);
			sumz += positions.at(i+2);
		}
		float finalx = sumx / (i / 3);
		float finaly = sumy / (i / 3);
		float finalz = sumz / (i / 3);

		
		//Agora tenho de escolher o ponto que fica na linha entre o start da bifurcação e o centroid. e que respeite o size
		float vector[3];
		vector[0] = finalx - s_aux->me->start[0];
		vector[1] = finaly - s_aux->me->start[1];
		vector[2] = finalz - s_aux->me->start[2];
		float magnitude = sqrtf(powf(vector[0], 2) + powf(vector[1], 2) + powf(vector[2], 2));
		
		vector[0] = vector[0] / magnitude;
		vector[1] = vector[1] / magnitude;
		vector[2] = vector[2] / magnitude;
		

		s_aux->me->end[0] = s_aux->me->start[0]+vector[0]*s_aux->me->size;
		s_aux->me->end[1] = s_aux->me->start[1] + vector[1] * s_aux->me->size;
		s_aux->me->end[2] = s_aux->me->start[2] + vector[2] * s_aux->me->size;
		//por todos os filhos com a mesma posição
		for each (skeleton * s in s_aux->children)
		{
			s->me->start[0] = s_aux->me->end[0];
			s->me->start[0] = s_aux->me->end[1];
			s->me->start[0] = s_aux->me->end[2];
		}

		for each (skeleton * s in s_aux->children)
		{
			
			//backward
			std::vector<skeleton *> all;
			s->getAllSkeleton(&all);

			float *target[3];
			s->getTarget(target);

			

			all.at(0)->me->start[0] = s_aux->me->end[0];
			all.at(0)->me->start[1] = s_aux->me->end[1];
			all.at(0)->me->start[2] = s_aux->me->end[2];

			for (i = 0; i < all.size() - 1; i++)
			{
				float r = distance(all.at(i)->me->start, all.at(i)->me->end);
				float y = all.at(i)->me->size / r;

				float temp1, temp2, temp3;

				temp1 = (1 - y)*all.at(i)->me->start[0] + y * all.at(i)->me->end[0];
				temp2 = (1 - y)*all.at(i)->me->start[1] + y * all.at(i)->me->end[1];
				temp3 = (1 - y)*all.at(i)->me->start[2] + y * all.at(i)->me->end[2];

				all.at(i)->me->end[0] = temp1;
				all.at(i)->me->end[1] = temp2;
				all.at(i)->me->end[2] = temp3;

				all.at(i + 1)->me->start[0] = temp1;
				all.at(i + 1)->me->start[1] = temp2;
				all.at(i + 1)->me->start[2] = temp3;

			}
		}

	}
	float *target[3];
	this->getTarget(target);
	this->update(*target,NULL);
	
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
