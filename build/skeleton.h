#pragma once

#include<../build/Bone.h>
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
	void update(float target[3]);
	void multiUpdate();
	float distance(float p1[3], float p2[3]);
	float totalSize();
	float * getEndEffector();
	void getAllSkeleton(std::vector<skeleton *> * ret);

	skeleton(float in_start[3], float in_end[3]);
	~skeleton();
};

