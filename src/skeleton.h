#pragma once

#include "Bone.h"
#include <vector>

class skeleton
{
public:
	std::vector<skeleton*> children;
	float target[3];
	Bone* me;
	skeleton* parent;

	skeleton* copy();

	void changeinwards();

	void changeoutwards();

	void changerestrictions();

	void addChildren(float in_end[3]);
	void addChildren(skeleton* child);
	void setParent(skeleton* s);
	void draw();
	void setTarget(float x, float y, float z);
	void getTarget(float* target[3]);
	void getEndPoints(std::vector<skeleton*>* endpoints);
	void backward();
	void forward();
	void update(float target[3], float* return_effector);
	void tempSkeleton(int path, skeleton* ret);

	void applyRestrictions(float firstVecX, float firstVecY, float firstVecZ);
	bool multiUpdateIn(float* originalX, float* originalY, float* originalZ);
	void multiUpdateOut(float originalX, float originalY, float originalZ);

	void multiUpdate(skeleton* targets[4]);

	//void multiUpdate(skeleton* targets[4], skeleton* placeholder);

	//void multiUpdate(skeleton* targets[4]);

	void save_state(skeleton* b);

	void reverte_state(skeleton* b);

	//void multiUpdate();
	void getSubRoots(std::vector<skeleton*>* subRoots);
	float distance(float p1[3], float p2[3]);
	float totalSize();
	float* getEndEffector();
	void getAllSkeleton(std::vector<skeleton*>* ret);

	skeleton(float in_start[3], float in_end[3]);
	skeleton(const skeleton& s);
	~skeleton();
};

