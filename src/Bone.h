#pragma once
class Bone
{
public:
	float start[3];
	float end[3];
	float size;
	
	float original_vec[3];//vetor end-start original
	float original_in[3];	//vetor in original
	float original_out[3];	//vetor out original
							

	float angle_in;
	float angle_vector_in[3];

	float angle_out;
	float angle_vector_out[3];

	bool fixed_in; // diz se o vector in vai ser fixo ou se vai ser atualizado em relação ao osso, ou se fica sempre fixo
	bool fixed_out; // diz se o vector in vai ser fixo ou se vai ser atualizado

	Bone();
	Bone(float start[3], float end[3],float angle_in, float angle_vector_in[3], float angle_out, float angle_vector_out[3],bool fixed_in, bool fixed_out);

	void draw_vecs();

	void draw();
	~Bone();
};

