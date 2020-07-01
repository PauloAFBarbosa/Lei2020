#pragma once

#include "Bone.h"
#include <vector>

class skeleton
{
public:
	std::vector<skeleton *> children;
	float target[3];
	Bone* me;
	skeleton* parent;

	void addChildren(float in_end[3]);
	void setParent(skeleton * s);
	void draw();
	void setTarget(float x,float y,float z);
	void getTarget(float * target[3]);
	void getEndPoints(std::vector<skeleton *> * endpoints);
	void backward();
	void forward();
	void update(float target[3], float * return_effector);
	void tempSkeleton(int path, skeleton * ret);
	void multiUpdate();
	void getSubRoots(std::vector<skeleton *> * subRoots);
	float distance(float p1[3], float p2[3]);
	float totalSize();
	float * getEndEffector();
	void getAllSkeleton(std::vector<skeleton *> * ret);

	skeleton(float in_start[3], float in_end[3]);
	~skeleton();
};

