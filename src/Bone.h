/**
 * @file Bone.h
 * @author Paulo Barbosa, Francisco Lira and Joel Gama
 * @brief Header for Bone class is the definition of a bone for the skeleton
 * @version 0.1
 * @date 2020-07-20
 *
 * @copyright Copyright (c) 2020
 *
 */

#pragma once

class Bone {
public:
	/** Array with x,y,z coordinates defining position of start point */
	float start[3];
	/** Array with x,y,z coordinates defining position of end point */
	float end[3];
	/** Lenght of the Bone*/
	float size;
	
	/** First end-start Vector */
	float original_vec[3]; 
	/** First in Vector */
	float original_in[3];  
	/** First out Vector */
	float original_out[3]; 

	/** Limit of movement for the bone, in rad */
	float angle_in; 
	/** Vetor that defines the center of movement for the bone */
	float angle_vector_in[3]; 

	/** Same as angle_in but for when is doing the out algorith */
	float angle_out; 
	/** Same as angle_vector_in but for doing the out algorith */
	float angle_vector_out[3]; 

	/** Define if you what that the vetor rotate with the bone or stay the same */
	bool fixed_in; 
	/** Same as fixed_in but for doing the out algorithm */
	bool fixed_out; 

	Bone();
	Bone(float start[3], float end[3],float angle_in, float angle_vector_in[3], float angle_out, float angle_vector_out[3],bool fixed_in, bool fixed_out);

	void draw_vecs();

	void draw();
	~Bone();
};

