


#pragma once

#include "Human.h"

class Program;

class Enemy : public Human
{
public:
	void update(double frametime);
	void init(unsigned int geomVAO, unsigned int indexVBO, int eleBufSize);

private:
	int direction = 1;

	//float walkTimer = 0.0;
};