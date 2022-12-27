#include "Human.h"
#include <iostream>

#define HUMAN_TORSO 1
#define HUMAN_LEFT_ARM 2
#define HUMAN_RIGHT_ARM 3
#define HUMAN_LEFT_LEG 4
#define HUMAN_RIGHT_LEG 5
#define HUMAN_HEAD 6
#define HUMAN_LEFT_ARM_PUNCH 7
#define HUMAN_RIGHT_ARM_PUNCH 8
#define HUMAN_LEFT_LEG_KICK 9
#define HUMAN_RIGHT_LEG_KICK 10

#define NUM_ENEMIES 3

using namespace std;

void Human::init(unsigned int geomVAO, unsigned int indexVBO, int eleBufSize)
{
	WorldObject::init(geomVAO, indexVBO, eleBufSize);

	scaleMatrices[BASE_MODEL] = glm::scale(glm::mat4(1.0), glm::vec3(0.5, 2.0, 1.0));

	//Torso Matrices
	modelMatrices.push_back(glm::translate(glm::mat4(1.0), glm::vec3(0.0, 1.0, 0.0)));
	scaleMatrices.push_back(glm::scale(glm::mat4(1.0), glm::vec3(0.5, 1.0, 0.7)));
	tRotateMatrices.push_back(glm::mat4(1.0));

	//Left Arm
	modelMatrices.push_back(glm::translate(glm::mat4(1.0), glm::vec3(0.0, 1.75,-1.0)));
	scaleMatrices.push_back(glm::scale(glm::mat4(1.0), glm::vec3(0.3, 1.0, 0.3)));
	tRotateMatrices.push_back(glm::translate(glm::mat4(1.0), glm::vec3(0.0, -1.0, 0.0)));

	//Right Arm
	modelMatrices.push_back(glm::translate(glm::mat4(1.0), glm::vec3(0.0, 1.75, 1.0)));
	scaleMatrices.push_back(glm::scale(glm::mat4(1.0), glm::vec3(0.3, 1.0, 0.3)));
	tRotateMatrices.push_back(glm::translate(glm::mat4(1.0), glm::vec3(0.0, -1.0, 0.0)));

	//Left Leg
	modelMatrices.push_back(glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, -0.4)));
	scaleMatrices.push_back(glm::scale(glm::mat4(1.0), glm::vec3(0.3, 1.0, 0.3)));
	tRotateMatrices.push_back(glm::translate(glm::mat4(1.0), glm::vec3(0.0, -1.0, 0.0)));

	//Right Leg
	modelMatrices.push_back(glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 0.4)));
	scaleMatrices.push_back(glm::scale(glm::mat4(1.0), glm::vec3(0.3, 1.0, 0.3)));
	tRotateMatrices.push_back(glm::translate(glm::mat4(1.0), glm::vec3(0.0, -1.0, 0.0)));

	//Head
	modelMatrices.push_back(glm::translate(glm::mat4(1.0), glm::vec3(0.0, 2.5, 0.0)));
	scaleMatrices.push_back(glm::scale(glm::mat4(1.0), glm::vec3(0.5, 0.5, 0.5)));
	tRotateMatrices.push_back(glm::mat4(1.0));

	//Left Arm Punch
	modelMatrices.push_back(glm::translate(glm::mat4(1.0), glm::vec3(0.0, 1.75, -1.0)));
	scaleMatrices.push_back(glm::scale(glm::mat4(1.0), glm::vec3(1.0, 0.3, 0.3)));
	tRotateMatrices.push_back(glm::translate(glm::mat4(1.0), glm::vec3(1.0, 0.0, 0.0)));

	//Right Arm Punch
	modelMatrices.push_back(glm::translate(glm::mat4(1.0), glm::vec3(0.0, 1.75, 1.0)));
	scaleMatrices.push_back(glm::scale(glm::mat4(1.0), glm::vec3(1.0, 0.3, 0.3)));
	tRotateMatrices.push_back(glm::translate(glm::mat4(1.0), glm::vec3(1.0, 0.0, 0.0)));

	//Left Leg Kick
	modelMatrices.push_back(glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, -0.4)));
	scaleMatrices.push_back(glm::scale(glm::mat4(1.0), glm::vec3(1.0, 0.3, 0.3)));
	tRotateMatrices.push_back(glm::translate(glm::mat4(1.0), glm::vec3(1.0, 0.0, 0.0)));

	//Right Leg Kick
	modelMatrices.push_back(glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 0.4)));
	scaleMatrices.push_back(glm::scale(glm::mat4(1.0), glm::vec3(1.0, 0.3, 0.3)));
	tRotateMatrices.push_back(glm::translate(glm::mat4(1.0), glm::vec3(1.0, 0.0, 0.0)));
}

void Human::setAttackMode(unsigned char newMode)
{ 
	if (attackActionTime >= 0.25)
	{
		if (movementMode == HUMAN_STAND)
		{
			//music.play_fx("../resources/woosh.mp3");
			attackMode = newMode;
			attackActionTime = 0.0;
		}
		else
			attackMode = HUMAN_NO_ATTACK;
	}
}

void Human::setMovementMode(unsigned char newMode)
{
	unsigned char platformStat;

	if (onPlatform())
	{
		if (newMode == HUMAN_JUMP)
			yVel = jumpVel;

		movementMode = newMode;
	}
	else if (inObstacle(&platformStat))
	{
		if (newMode == HUMAN_JUMP)
		{
			/*if (platformStat == PLAT_ABOVE)
			{
				yVel = jumpVel;
				movementMode = newMode;
			}*/
			yVel = jumpVel;
			movementMode = newMode;
		}
		
	}
}

void Human::draw(const std::shared_ptr<Program> prog, double frametime)
{
	if (health > 0)
	{
		glm::mat4 M = glm::mat4(1.0);
		glm::vec4 attackMin, attackMax;

		resolveJump(frametime);

		modelMatrices[BASE_MODEL] = glm::translate(glm::mat4(1.0), position) * rotation;

		if (attackActionTime < 0.25)
		{
			if (attackMode == HUMAN_PUNCH)
				modelMatrices[BASE_MODEL] = modelMatrices[BASE_MODEL] * glm::rotate(glm::mat4(1.0), 3.14159f / 4.0f, glm::vec3(0.0, 1.0, 0.0));
			else if (attackMode == HUMAN_KICK)
				modelMatrices[BASE_MODEL] = modelMatrices[BASE_MODEL] * glm::rotate(glm::mat4(1.0), -3.14159f / 4.0f, glm::vec3(0.0, 1.0, 0.0));
		}

		glBindVertexArray(vaoID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);

		M = (modelMatrices[BASE_MODEL] * modelMatrices[HUMAN_TORSO]) * scaleMatrices[HUMAN_TORSO];
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, (void*)0);

		M = (modelMatrices[BASE_MODEL] * modelMatrices[HUMAN_HEAD]) * scaleMatrices[HUMAN_HEAD];
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, (void*)0);

		if (movementMode == HUMAN_WALK)
			walkCycle += 10.0f * frametime;
		else
			walkCycle = 0.0f;

		

		M = modelMatrices[BASE_MODEL] * modelMatrices[HUMAN_LEFT_ARM] * glm::rotate(glm::mat4(1.0), sin(walkCycle), glm::vec3(0.0, 0.0, 1.0)) * tRotateMatrices[HUMAN_LEFT_ARM];
		M = M * scaleMatrices[HUMAN_LEFT_ARM];
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, (void*)0);

		if (attackMode == HUMAN_PUNCH && attackActionTime < 0.25)
		{
			M = modelMatrices[BASE_MODEL] * modelMatrices[HUMAN_RIGHT_ARM_PUNCH] * glm::rotate(glm::mat4(1.0), -3.14159f / 8.0f, glm::vec3(0.0, 1.0, 0.0)) * tRotateMatrices[HUMAN_RIGHT_ARM_PUNCH];
			M = M * scaleMatrices[HUMAN_RIGHT_ARM_PUNCH];
			if (attackActionTime == 0.0)
			{
				attackMin = M * baseCollider.topLeft;
				attackMax = M * baseCollider.bottomRight;

				for (int i = 0; i < NUM_ENEMIES; i ++)
					if (enemy[i] != NULL && enemy[i]->colliderInCollider(Collider{ attackMin, attackMax }))
						enemy[i]->takeDamage(50);
			}
		}
		else
		{
			M = modelMatrices[BASE_MODEL] * modelMatrices[HUMAN_RIGHT_ARM] * glm::rotate(glm::mat4(1.0), sin(-walkCycle), glm::vec3(0.0, 0.0, 1.0)) * tRotateMatrices[HUMAN_RIGHT_ARM];
			M = M * scaleMatrices[HUMAN_RIGHT_ARM];
		}

		
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, (void*)0);

		if (attackMode == HUMAN_KICK && attackActionTime < 0.25)
		{
			M = modelMatrices[BASE_MODEL] * modelMatrices[HUMAN_LEFT_LEG_KICK] * glm::rotate(glm::mat4(1.0), 3.14159f / 8.0f, glm::vec3(0.0, 1.0, 0.0)) * tRotateMatrices[HUMAN_LEFT_LEG_KICK];
			M = M * scaleMatrices[HUMAN_LEFT_LEG_KICK];
			if (attackActionTime == 0.0)
			{
				attackMin = M * baseCollider.topLeft;
				attackMax = M * baseCollider.bottomRight;

				for (int i = 0; i < NUM_ENEMIES; i++)
					if (enemy[i] != NULL && enemy[i]->colliderInCollider(Collider{ attackMin, attackMax }))
						enemy[i]->takeDamage(50);
			}
		}
		else
		{
			M = modelMatrices[BASE_MODEL] * modelMatrices[HUMAN_LEFT_LEG] * glm::rotate(glm::mat4(1.0), sin(-walkCycle), glm::vec3(0.0, 0.0, 1.0)) * tRotateMatrices[HUMAN_LEFT_LEG];
			M = M * scaleMatrices[HUMAN_LEFT_LEG];
		}

		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, (void*)0);

		M = modelMatrices[BASE_MODEL] * modelMatrices[HUMAN_RIGHT_LEG] * glm::rotate(glm::mat4(1.0), sin(walkCycle), glm::vec3(0.0, 0.0, 1.0)) * tRotateMatrices[HUMAN_RIGHT_LEG];
		M = M * scaleMatrices[HUMAN_RIGHT_LEG];
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, (void*)0);

		glBindVertexArray(0);

		if (attackMode == HUMAN_PUNCH)
			attackActionTime += frametime;
		else if (attackMode == HUMAN_KICK)
			attackActionTime += frametime;
	}
}

bool Human::move(double multiplier)
{
	bool result = FALSE;
	int obstacleInfo = inObstacle();

	if (obstacleInfo == 0 || (obstacleInfo < 0 && multiplier > 0) || (obstacleInfo > 0 && multiplier < 0))
	{
		position.x += runVel * multiplier;
		result = TRUE;
	}

	if (multiplier > 0)
		rotation = glm::rotate(glm::mat4(1.0), 0.0f, glm::vec3(0.0, 1.0, 0.0));
	else
		rotation = glm::rotate(glm::mat4(1.0), 3.14159f, glm::vec3(0.0, 1.0, 0.0));

	return result;
}

bool Human::onPlatform()
{
	glm::mat4 P = glm::translate(glm::mat4(1.0), position) * scaleMatrices[BASE_MODEL];
	Collider tempCollider = { P * bottom.topLeft, P * bottom.bottomRight };
	double yAdjust;

	for (int i = 0; i < 6; i++)
	{
		/*if (platforms[i]->pointInCollider(glm::vec3(tempLeft.x, tempLeft.y, tempLeft.z)) || platforms[i]->pointInCollider(glm::vec3(tempRight.x, tempRight.y, tempRight.z)))
			return TRUE;*/

		if (platforms[i]->colliderInCollider(tempCollider, NULL, &yAdjust))
		{
			position.y += yAdjust;
			return TRUE;
		}
			
	}

	return FALSE;
}

int Human::inObstacle()
{
	glm::mat4 P = glm::translate(glm::mat4(1.0), position) * scaleMatrices[BASE_MODEL];
	Collider tempCollider = { P * body.topLeft, P * body.bottomRight };
	double xAdjust;

	for (int i = 0; i < 6; i++)
	{
		if (platforms[i]->colliderInCollider(tempCollider, &xAdjust))
		{
			position.x += xAdjust;
			if (position.x > platforms[i]->getPos().x)
				return -1;
			else
				return 1;
		}
	}

	return 0;
}

int Human::inObstacle(unsigned char *platformStat)
{
	glm::mat4 P = glm::translate(glm::mat4(1.0), position) * scaleMatrices[BASE_MODEL];
	Collider tempCollider = { P * body.topLeft, P * body.bottomRight };
	double xAdjust;

	for (int i = 0; i < 6; i++)
	{
		if (platforms[i]->colliderInCollider(tempCollider, &xAdjust))
		{
			position.x += xAdjust;
			/*if (xAdjust > 1)
				return -1;
			else
				return 1;*/

			if (platformStat != NULL)
			{
				Collider platCollider = platforms[i]->getCollider();
				if (platCollider.topLeft.y > position.y)
					*platformStat = PLAT_ABOVE;
				else
					*platformStat = PLAT_BELOW;
			}
			return TRUE;
		}

	}

	return 0;
}

void Human::resolveJump(double frametime)
{
	if (onPlatform() && movementMode != HUMAN_JUMP)// && moveActionTime > 0)
	{
		//position.y -= yVel * frametime;
		yVel = 0;
		//movementMode = HUMAN_STAND;
		return;
	}
	/*else if (movementMode == HUMAN_WALL_JUMP)
	{
		if (moveActionTime < .5)
		{
			if (rotation[0][0] < 0)
				move(frametime);
			else
				move(-frametime);
		}
		else
			moveActionTime = 0;
	}*/

	yVel += gravity * frametime;
	position.y += yVel * frametime;

	moveActionTime += frametime;
}

void Human::takeDamage(int damage) { health -= damage; }