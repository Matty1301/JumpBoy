//#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include "Platform.h"
#include "MainGame.h"
#include "Coin.h"
#include "Obstacle.h"

void UpdatePlatforms()
{
	//For each platform level check if a new platform should be spawned
	int platformLength;
	for (int platformLevel = 1; platformLevel <= gameState.timeToSpawnPlatforms.size(); platformLevel++)
	{
		float& timeToSpawnPlatform = gameState.timeToSpawnPlatforms[platformLevel - 1];
		timeToSpawnPlatform -= constElapsedTime;

		if (gameState.playerState != STATE_DEAD && timeToSpawnPlatform <= 0)
		{
			platformLength = SpawnPlatform(platformLevel);
			//Update time to spawn outisde of SpawnPlatform function for better visibility
			timeToSpawnPlatform = platformLength * 0.2f + Play::RandomRoll(8) * 0.2f;
		}
	}

	std::vector<int> vPlatforms = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);
	for (int id_platform : vPlatforms)
	{
		GameObject& obj_platform = Play::GetGameObject(id_platform);

		obj_platform.velocity = gameState.currentVelocity;
		Play::UpdateGameObject(obj_platform);
	}
}

int SpawnPlatform(int platformLevel)
{
	const float groundPosY = Play::GetGameObjectByType(TYPE_GROUND).pos.y;

	float platformPosX = DISPLAY_WIDTH + (consts::spriteWidth_platform / 2);
	float platformPosY = groundPosY - (platformLevel * platformHeight);;
	const char* spriteName;

	int platformLength = Play::RandomRollRange(3, 7);
	for (int i = 1; i <= platformLength; i++)
	{
		if (i == 1)
			spriteName = "left";
		else if (i == platformLength)
			spriteName = "right";
		else
			spriteName = "middle";

		Vector2f platformPiecePos = { platformPosX + consts::spriteWidth_platform * (i - 1), platformPosY };

		SpawnPlatformPiece(platformPiecePos, spriteName);	//Separate function to avoid loop within a loop

		int rand10 = Play::RandomRoll(10);

		if (rand10 == 1)
		{
			Coin coin{ platformPiecePos };
			vCoins.push_back(coin);
		}
		else if (rand10 == 2)
			SpawnObstacle(platformPiecePos);
	}
	return platformLength;
}

void SpawnPlatformPiece(Vector2f platformPos, const char* spriteName)
{
	std::vector<int> vPlatforms = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);

	//Object pooling
	//Search for a currently unused platform piece with the correct sprite
	for (int id_platform : vPlatforms)
	{
		GameObject& obj_platform = Play::GetGameObject(id_platform);

		if (obj_platform.pos.x <= -consts::spriteWidth_platform / 2
			&& Play::GetSpriteName(obj_platform.spriteId) == spriteName)
		{
			obj_platform.pos = platformPos;	//If one is found reuse it
			return;	//Then return out of the function
		}
	}

	//If none are found create a new platform piece
	int id_platform = Play::CreateGameObject(TYPE_PLATFORM, platformPos, 0, spriteName);
}