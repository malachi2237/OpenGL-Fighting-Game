


#pragma once

#ifndef WORLD_OBJECT_H
#define WORLD_OBJECT_H

#include "GLSL.h"
#include "Program.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include <memory>

#define BASE_MODEL 0

struct Collider
{
	glm::vec4 topLeft;
	glm::vec4 bottomRight;
};

class WorldObject
{
public:
	glm::vec3 getPos();
	void translate(glm::vec3 translation);
	void init(unsigned int geomVAO, unsigned int indexVBO, int eleBufSize);// std::vector<GLushort> *elements, std::vector<GLfloat> *positions);
	void draw(const std::shared_ptr<Program> prog);
	Collider getCollider();
	bool pointInCollider(glm::vec3 point);
	bool colliderInCollider(Collider otherCollider, double* xDiff = NULL, double*yDiff = NULL);

protected:
	unsigned int vaoID = 0;
	unsigned int eleBufID = 0;

	float gravity = -20;

	int numElements;
	//std::vector<GLushort> *eleBuf;
	//std::vector<GLfloat> *posBuf;

	glm::mat4 rotation = glm::mat4(1.0);

	std::vector<glm::mat4> modelMatrices;
	std::vector<glm::mat4> scaleMatrices;
	std::vector<glm::mat4> tRotateMatrices;

	glm::vec3 position{ 0.0, 0.0, 0.0 };

	Collider baseCollider = { glm::vec4(-1.0, 1.0, -1.0, 1.0), glm::vec4(1.0, -1.0, 1.0, 1.0) };
};


#endif WORLD_OBJECT_H