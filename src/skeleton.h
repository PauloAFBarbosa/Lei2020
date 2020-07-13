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

	void addChildren(float in_end[3],float angle_in,float angle_vector_in[3], float angle_out, float angle_vector_out[3], bool fixed_in, bool fixed_out);
	void addChildren(skeleton* child);
	
	void setParent(skeleton* s);
	void draw();
	void setTarget(float x, float y, float z);
	void getTarget(float* target[3]);
	void getEndPoints(std::vector<skeleton*>* endpoints);
	void backward();
	void forward();
	void update_vec(Bone* b);
	void update(float target[3], float* return_effector);
	

	void applyRestrictions(float firstVecX, float firstVecY, float firstVecZ);
	void rotate(float original[3], float from[3], float to[3], float angle, float res[3]);
	//void rotate(float original[3], float target_vector[3], float angle, float res[3]);
	bool multiUpdateIn(float* originalX, float* originalY, float* originalZ);
	void multiUpdateOut(float originalX, float originalY, float originalZ);

	void multiUpdate(skeleton* targets[4]);



	//void multiUpdate();
	void getSubRoots(std::vector<skeleton*>* subRoots);
	float distance(float p1[3], float p2[3]);
	float totalSize();
	float* getEndEffector();
	void getAllSkeleton(std::vector<skeleton*>* ret);

	skeleton(float in_start[3], float in_end[3],float angle_in, float angle_vector_in[3], float angle_out, float angle_vector_out[3],bool fixed_in,bool fixed_out);
	
	~skeleton();
};

