//#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include "Obstacle.h"
#include "MainGame.h"

void UpdateObstacles()
{
	GameObject& obj_player = Play::GetGameObjectByType(TYPE_PLAYER);

	std::vector<int> vObstacles = Play::CollectGameObjectIDsByType(TYPE_OBSTACLE);
	for (int id_obstacle : vObstacles)
	{
		GameObject& obj_obstacle = Play::GetGameObject(id_obstacle);

		obj_obstacle.velocity = gameState.currentVelocity;
		Play::UpdateGameObject(obj_obstacle);

		if (gameState.playerState != STATE_DEAD && Play::IsColliding(obj_player, obj_obstacle))
		{
			PlayerDeath();
		}
	}
}

void SpawnObstacle(Vector2f platformPiecePos)
{
	const char* spriteName;

	int rand3 = Play::RandomRoll(3);
	if (rand3 == 1)
		spriteName = "1obstacle";
	else if (rand3 == 2)
		spriteName = "2obstacle";
	else
		spriteName = "3obstacle";

	Vector2f obtsaclePos = { platformPiecePos.x, platformPiecePos.y - 37.5f };

	std::vector<int> vObstacles = Play::CollectGameObjectIDsByType(TYPE_OBSTACLE);

	//Object pooling
	for (int id_obstacle : vObstacles)
	{
		GameObject& obj_obstacle = Play::GetGameObject(id_obstacle);

		if (obj_obstacle.pos.x <= -consts::spriteWidth_obstacle / 2 && Play::GetSpriteName(obj_obstacle.spriteId) == spriteName)
		{
			obj_obstacle.pos = obtsaclePos;
			return;
		}
	}

	Play::CreateGameObject(TYPE_OBSTACLE, obtsaclePos, consts::spriteWidth_obstacle / 3, spriteName);
}