#include "Platform.h"
#include <iostream>

using namespace std;

void Platform::init(unsigned int geomVAO, unsigned int indexVBO, int eleBufSize, glm::vec3 scale)
{
	WorldObject::init(geomVAO, indexVBO, eleBufSize);

	scaleMatrices[0] = glm::scale(glm::mat4(1.0), scale);
}

void Platform::draw(const std::shared_ptr<Program> prog)
{
	modelMatrices[0] = glm::translate(glm::mat4(1.0), position);
	WorldObject::draw(prog);
}