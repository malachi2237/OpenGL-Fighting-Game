


#pragma once

#ifndef HUMAN_H
#define HUMAN_H

#include "WorldObject.h"
#include "Platform.h"
#include "sound.h"

#define PLAT_ABOVE 1
#define PLAT_BELOW -1

#define HUMAN_NO_ATTACK 0
#define HUMAN_PUNCH 1
#define HUMAN_KICK 2

#define HUMAN_STAND 10
#define HUMAN_WALK 20
#define HUMAN_JUMP 40
#define HUMAN_WALL_JUMP 50
#define HUMAN_FALL 60


class Program;

class Human : public WorldObject
{
public:
	void init(unsigned int geomVAO, unsigned int indexVBO, int eleBufSize);
	void draw(const std::shared_ptr<Program> prog, double frametime);
	
	bool move(double multiplier);
	void takeDamage(int damage);

	void setMovementMode(unsigned char newMode);
	void setAttackMode(unsigned char newMode);

	//bool punching;
	//bool kicking;
	int health = 150;

	Human *enemy[3];
	Platform *platforms[6];

protected:
	bool onPlatform();

	unsigned char movementMode = HUMAN_STAND;
	unsigned char attackMode = HUMAN_STAND;
	

	float jumpVel = 10;
	float yVel = 0.0;
	float runVel = 10;

	float walkCycle = 0.0f;
	double moveActionTime = 0.0;
	double attackActionTime = 10.0;

	bool platformStat = FALSE;
	bool obstacle = FALSE;

	Collider leftArm = { glm::vec4(1.0, -1.0, 0.0, 1.0), glm::vec4(1.0, 1.0, 0.0, 1.0) };
	Collider rightArm = { glm::vec4(-1.0, -1.0, -1.0, 1.0), glm::vec4(1.0, 1.0, 1.0, 1.0) };

	Collider leftLeg = { glm::vec4(1.0, -1.0, 0.0, 1.0), glm::vec4(1.0, 1.0, 0.0, 1.0) };
	Collider rightLeg = { glm::vec4(-1.0, -1.0, -1.0, 1.0), glm::vec4(1.0, 1.0, 1.0, 1.0) };

	Collider top = { glm::vec4(-1.0, 1.0, 0.0, 1.0), glm::vec4(1.0, 1.0, 0.0, 1.0) };
	Collider bottom = { glm::vec4(-0.5, 1.0, 0.0, 1.0), glm::vec4(0.5, -1.0, 0.0, 1.0) };
	Collider body = { glm::vec4(-1.00, 0.95, 0.0, 1.0), glm::vec4(1.00, -0.90, 0.0, 1.0) };

	void resolveJump(double frametime);

	int inObstacle();
	int inObstacle(unsigned char* platformStat);

	music_ music;
};

#endif HUMAN_H
