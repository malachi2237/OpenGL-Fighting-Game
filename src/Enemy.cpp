


#include "Enemy.h"

#include <iostream>

#define PLAYER_ID 0
using namespace std;

void Enemy::init(unsigned int geomVAO, unsigned int indexVBO, int eleBufSize)
{
	//std::vector<Human*> tempPlayer = { player };
	Human::init(geomVAO, indexVBO, eleBufSize);
	rotation = glm::rotate(glm::mat4(1.0), 3.14159f, glm::vec3(0.0, 1.0, 0.0));
}

void Enemy::update(double frametime)
{
	int obs = inObstacle();
	bool plat = onPlatform();

	glm::vec4 testPos = (glm::translate(glm::mat4(1.0), position) * rotation * glm::translate(glm::mat4(1.0), glm::vec3(1.5, 0.0, 0.0))) * glm::vec4(0.0, 0.0, 0.0, 1.0);
	glm::mat4 M = glm::translate(glm::mat4(1.0), glm::vec3(testPos.x, testPos.y, testPos.z)) * scaleMatrices[BASE_MODEL];
	Collider testCollider = { M * baseCollider.topLeft, M * baseCollider.bottomRight };

	//glm::vec3 testPoint = testCollider.bottomRight;
	//testPoint.y = testPoint.y - 1.0;
	////testPoint.x += direction;

	///*if (plat)
	//{*/
	//	bool tempPlat = FALSE;
	//	for (int i = 0; i < 6; i++)
	//	{
	//		if (platforms[i]->pointInCollider(testCollider.bottomRight))
	//		{
	//			tempPlat = TRUE;
	//			break;
	//		}
	//		
	//	}
	//	if (tempPlat && walkTimer > 0.1)
	//	{
	//		direction = -direction;
	//		walkTimer = 0.0;
	//	}
	//}

	if (enemy[PLAYER_ID]->colliderInCollider(testCollider))
	{
		setMovementMode(HUMAN_STAND);
		if (attackActionTime > 0.5)
			setAttackMode(HUMAN_PUNCH);
	}
	//else if (plat)// && walkTimer > 0.1)
	//{
	//	move(frametime * direction);
	//	setMovementMode(HUMAN_WALK);
	//	//walkTimer = 0.0;
	//}

	//walkTimer += frametime;
}
