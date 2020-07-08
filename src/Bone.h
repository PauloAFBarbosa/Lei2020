#pragma once
class Bone
{
public:
	float start[3];
	float end[3];
	float size;
	float angle_in;
	float angle_vector_in[3];

	float angle_out;
	float angle_vector_out[3];

	Bone();
	Bone(float start[3], float end[3],float angle_in, float angle_vector_in[3], float angle_out, float angle_vector_out[3]);

	Bone* copy();

	void draw();
	~Bone();
};

