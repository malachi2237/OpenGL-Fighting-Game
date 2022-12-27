#include "WorldObject.h"
#include <iostream>

#include "minmax.h"
using namespace std;


void WorldObject::init(unsigned int geomVAO, unsigned int indexVBO, int eleBufSize)//, std::vector<GLushort> *elements, std::vector<GLfloat> *positions)
{
	vaoID = geomVAO;
	eleBufID = indexVBO;

	numElements = eleBufSize;
	/*eleBuf = elements;
	posBuf = positions;*/

	modelMatrices.push_back(glm::mat4(1.0));
	scaleMatrices.push_back(glm::mat4(1.0));
	tRotateMatrices.push_back(glm::mat4(1.0));
}

void WorldObject::draw(const std::shared_ptr<Program> prog)
{
	glm::mat4 M = modelMatrices[0] * scaleMatrices[0];
	
	glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);

	glBindVertexArray(vaoID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);

	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, (void*)0);

	glBindVertexArray(0);
}

Collider WorldObject::getCollider()
{
	glm::mat4 M = (glm::translate(glm::mat4(1.0), position) * rotation) * scaleMatrices[0];
	return Collider{ M * baseCollider.topLeft,  M * baseCollider.bottomRight };
}

bool WorldObject::pointInCollider(glm::vec3 point)
{
	glm::mat4 M = glm::translate(glm::mat4(1.0), position) * rotation;
	M = M * scaleMatrices[0];
	glm::vec4 tempMin = M * baseCollider.topLeft;
	glm::vec4 tempMax = M * baseCollider.bottomRight;


	if (point.x < tempMin.x || point.y < tempMin.y ||
		point.x > tempMax.x || point.y > tempMax.y)
		return false;

	return true;
}

bool WorldObject::colliderInCollider(Collider other, double*xDiff, double*yDiff)
{
	glm::mat4 M = glm::translate(glm::mat4(1.0), position);// *rotation;
	M = M * scaleMatrices[BASE_MODEL];

	glm::vec4 tempLeft = M * baseCollider.topLeft;
	glm::vec4 tempRight = M * baseCollider.bottomRight;

	double otherLeft = min(other.topLeft.x, other.bottomRight.x);
	double otherRight = max(other.topLeft.x, other.bottomRight.x);

	bool xCondition = otherRight >= tempLeft.x && otherLeft <= tempRight.x;
	bool yCondition = other.topLeft.y >= tempRight.y && other.bottomRight.y <= tempLeft.y;

	if (yDiff != NULL)
		*yDiff = (double)tempLeft.y - (double)other.bottomRight.y;

	if (xDiff != NULL)
	{
		if (tempRight.x < otherRight)
			*xDiff = tempRight.x - otherLeft;
			//*xDiff = .01;
		else
			*xDiff = tempLeft.x - otherRight;
	}
		

	return xCondition && yCondition;
}

void WorldObject::translate(glm::vec3 translation) { position += translation; }

glm::vec3 WorldObject::getPos() { return position; }