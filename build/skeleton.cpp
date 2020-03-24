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
	printf("Tenho %d filhos \n", this->children.size());
	for each (skeleton * s in this->children)
	{
		s->getEndPoints(endpoints);
	}
	if (this->children.size() == 0) {
		printf("WEEEEEEEEEEEEEEEEEE\n");
		endpoints->push_back(this);
		
	}

}

void skeleton::update(float target[3])
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
			int i = all.size() - 2;

			for (i; i>=0; i--)
			{
				float r = distance(all.at(i+1)->me->end, all.at(i)->me->end);
				float y = all.at(i+1)->me->size/ r;

				float temp1, temp2, temp3;
				temp1 = (1 - y)*all.at(i + 1)->me->end[0] + y * all.at(i)->me->end[0];
				temp2= (1 - y)*all.at(i + 1)->me->end[1] + y * all.at(i)->me->end[1];
				temp3= (1 - y)*all.at(i + 1)->me->end[2] + y * all.at(i)->me->end[2];
				all.at(i)->me->end[0] = temp1;
				all.at(i)->me->end[1] = temp2;
				all.at(i)->me->end[2] = temp3;

				all.at(i+1)->me->start[0]= temp1;
				all.at(i + 1)->me->start[1] = temp2;
				all.at(i + 1)->me->start[2] = temp3;
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

void skeleton::multiUpdate()
{
	//get all endpoints
	std::vector<skeleton *> endPoints;
	//printf("Vai buscar endpoints\n");
	this->getEndPoints(&endPoints);
	//printf("Tem %d endpoints\n", endPoints.size());
	

	//No caso se ser um skeleton apenas com um endpoint nao precisa fazer o while
	if (endPoints.size() == 1) {
		float * target[3];
		this->getTarget(target);
		this->update(*target);
		return;
	}

	while (!endPoints.empty())
	{
		printf("Vai fazer coisas no while :D\n");
		//vai buscar o primeiro elemento e apaga-o
		skeleton * s_aux = endPoints.at(0);
		skeleton * last=NULL;
		endPoints.erase(endPoints.begin());
		
		while (s_aux->parent!=NULL && s_aux->children.size()<=1)
		{
			printf("Dentro do while interior \n");
			last = s_aux;
			s_aux = s_aux->parent;
			
		}
		
		//varias posições para a root (vai ser usado para fazer a media depois)
		std::vector<float> positions;

		for each (skeleton * s in s_aux->children)
		{
			//printf("Vai procurar os filhos\n");
			//vai apenas chamar o update no children pelo qual ele veio comparando os entereços de memoria
			if (s == last) {
				printf("Vai fazer um update ()\n");
				float * target[3];
				s->getTarget(target);
				//printf("O target vai ser %f %f %f \n", (*target)[0], (*target)[1], (*target)[2]);
				s->update(*target);
				endPoints.push_back(s_aux);
				
			}
		}


	}
	printf("Acabou o multiupdate\n");
}


float skeleton::distance(float p1[3], float p2[3])
{
	return sqrtf(pow((p1[0] - p2[0]), 2) + pow((p1[1] - p2[1]), 2) + pow((p1[2] - p2[2]), 2));
}

float skeleton::totalSize()
{
	float result = this->me->size;
	for each (skeleton * s in this->children)
	{
		result += s->totalSize();
	}
	
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
	this->children.at(0)->getAllSkeleton(ret);
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
