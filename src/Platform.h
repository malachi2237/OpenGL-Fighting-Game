


#pragma once

#ifndef PLATFORM_H
#define PLATFORM_H

#include "WorldObject.H"

class Platform : public WorldObject
{
public:
	void init(unsigned int geomVAO, unsigned int indexVBO, int eleBufSize, glm::vec3 scale);
	void draw(const std::shared_ptr<Program> prog);
};

#endif PLATFORM_H