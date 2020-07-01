#pragma once
class Bone
{
public:
	float start[3];
	float end[3];
	float size;
	Bone();
	Bone(float start[3], float end[3]);

	void draw();
	~Bone();
};

