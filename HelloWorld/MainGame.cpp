#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include "MainGame.h"
#include "Player.h"
#include "Platform.h"
#include "Coin.h"
#include "Obstacle.h"
#include "Ground.h"
#include "Background.h"

GameState gameState;

// The entry point for a PlayBuffer program
void MainGameEntry( PLAY_IGNORE_COMMAND_LINE )
{
	Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE );
	Play::CentreAllSpriteOrigins();
	Play::MoveMatchingSpriteOrigins("player", 15, 0);	//Move sprite centre for purpose of collision bounds

	//Initialise consts = must be done after declaration due to function call
	consts::spriteWidth_BG = Play::GetSpriteWidth("BG");
	consts::spriteHeight_BG = Play::GetSpriteHeight("BG");
	consts::spriteHeight_ground = Play::GetSpriteHeight("ground");
	consts::spriteWidth_ground = Play::GetSpriteWidth("ground");
	consts::spriteWidth_player = Play::GetSpriteWidth("player");
	consts::spriteWidth_coin = Play::GetSpriteWidth("coin");
	consts::spriteWidth_platform = Play::GetSpriteWidth("platform");
	consts::spriteHeight_platform = Play::GetSpriteHeight("platform");
	consts::spriteWidth_obstacle = Play::GetSpriteWidth("obstacle");

	//Spawn background
	int backgroundPosY = DISPLAY_HEIGHT - consts::spriteHeight_BG / 2 + backgroundHeight;
	Play::CreateGameObject(TYPE_BACKGROUND, { 0, backgroundPosY }, 0, "BG");
	Play::CreateGameObject(TYPE_BACKGROUND, { consts::spriteWidth_BG, backgroundPosY }, 0, "BG");

	//Spawn ground
	for (int i{ consts::spriteWidth_ground / 2 }; i < DISPLAY_WIDTH + (1.5f * consts::spriteWidth_ground); i += consts::spriteWidth_ground)
	{
		int groundId = Play::CreateGameObject(TYPE_GROUND, { i, DISPLAY_HEIGHT - consts::spriteHeight_ground / 2 }, 0, "ground");
		Play::GetGameObject(groundId).velocity = { playerRunSpeed, 0 };
	}

	//Spawn player
	int playerId = Play::CreateGameObject(TYPE_PLAYER, consts::spritePos_player, playerCollisionRadius, "run");
	Play::GetGameObject(playerId).animSpeed = 0.3f;
}

bool MainGameUpdate(float elpasedTime)
{
	gameState.timer += constElapsedTime;
	gameState.relativeVelocity += gameState.relativeAcceleration;

	GetActiveTile();
	UpdateCurrentVelocity();
	UpdatePlayer();
	UpdateBackground();
	UpdateGround();
	UpdatePlatforms();

	for (int i = 0; i < vCoins.size(); i++)
	{
		vCoins[i].Update();
	}

	UpdateObstacles();

	Play::ClearDrawingBuffer(Play::cBlack);
	DrawScene();
	Play::PresentDrawingBuffer();

	return Play::KeyDown( VK_ESCAPE );
}

int MainGameExit( void )
{
	Play::DestroyManager();
	return PLAY_OK;
}

void ResetGame()
{
	GameObject& obj_player = Play::GetGameObjectByType(TYPE_PLAYER);

	gameState.score = 0;
	gameState.playerState = STATE_RUN;
	Play::SetSprite(obj_player, "run", 0.3f);
	obj_player.radius = playerCollisionRadius;
	gameState.relativeVelocity = { playerRunSpeed, 0 };
	gameState.relativeAcceleration = { 0, 0 };

	//Reset the background position
	std::vector<int> vBackround = Play::CollectGameObjectIDsByType(TYPE_BACKGROUND);
	for (int id_background : vBackround)
	{
		GameObject& obj_background = Play::GetGameObject(id_background);
		obj_background.pos.y = DISPLAY_HEIGHT - consts::spriteHeight_BG / 2 + backgroundHeight;
	}

	//Reset the ground position
	std::vector<int> vGround = Play::CollectGameObjectIDsByType(TYPE_GROUND);
	for (int id_ground : vGround)
	{
		GameObject& obj_ground = Play::GetGameObject(id_ground);
		obj_ground.pos.y = DISPLAY_HEIGHT - (consts::spriteHeight_ground / 2);
	}

	Play::DestroyGameObjectsByType(TYPE_PLATFORM);
	Play::DestroyGameObjectsByType(TYPE_OBSTACLE);

	//Storage is handled by the vector so the objects in the vector don't need to be manually deleted before being cleared
	vCoins.clear();
}

//Sets gamestate.ptr_activeTile to the location of the tile currently beneath the player
void GetActiveTile()
{
	GameObject& obj_player = Play::GetGameObjectByType(TYPE_PLAYER);

	//Default the active tile to any ground tile in the case there is no platform below the player
	gameState.ptr_activeTile = &Play::GetGameObjectByType(TYPE_GROUND);

	std::vector<int> vPlatforms = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);

	//Loop through every platform to find the one that is closest to the player and is beneath the player
	//and set this as the active tile. If none are found the active tile stays as the defaulted ground tile
	for (int id_platform : vPlatforms)
	{
		GameObject& obj_platform = Play::GetGameObject(id_platform);

		if (obj_platform.pos.x - consts::spriteWidth_platform / 2 <= obj_player.pos.x
			&& obj_platform.pos.x + consts::spriteWidth_platform / 2 >= obj_player.pos.x
			&& obj_platform.pos.y - consts::spriteHeight_platform / 2 >= obj_player.pos.y + playerRunHeight
			&& obj_platform.pos.y < gameState.ptr_activeTile->pos.y)
		{
			gameState.ptr_activeTile = &obj_platform;
		}
	}
}

//Calculates the current velocity
//The current velocity is used to prevent clipping
//and also to shift the game view when the player is near the ground
void UpdateCurrentVelocity()
{
	GameObject& obj_player = Play::GetGameObjectByType(TYPE_PLAYER);

	int tileHeight = Play::GetSpriteHeight(gameState.ptr_activeTile->spriteId);

	//Reset the player velocity
	obj_player.velocity.y = 0;

	//Set the x componenet of the current velocity
	gameState.currentVelocity.x = gameState.relativeVelocity.x;

	int tempCurrentVelocityY = gameState.relativeVelocity.y;

	//Stop the player clipping into platforms and the ground
	if (gameState.ptr_activeTile->pos.y - tileHeight / 2 + gameState.relativeVelocity.y < obj_player.pos.y + playerRunHeight)
	{
		tempCurrentVelocityY = (obj_player.pos.y + playerRunHeight) - (gameState.ptr_activeTile->pos.y - tileHeight / 2);
	}

	GameObject& groundTile = Play::GetGameObjectByType(TYPE_GROUND);
	int groundBasePosY = DISPLAY_HEIGHT - consts::spriteHeight_ground / 2;
	int groundNewPosY = groundTile.pos.y + tempCurrentVelocityY;

	//When the player approaches the ground move the player's position on the screen to get a better game view
	//and adjust the current velocity to maintain the overall relative velocity
	if (groundNewPosY < groundBasePosY)
	{
		gameState.currentVelocity.y = tempCurrentVelocityY + (groundBasePosY
			- groundNewPosY);
		obj_player.velocity.y = -(tempCurrentVelocityY - gameState.currentVelocity.y);
	}
	//When the player moves away from the ground move the player's position back
	//and adjust the current velocity to maintain the overall relative velocity
	else if (groundNewPosY > groundBasePosY
		&& obj_player.pos.y > DISPLAY_HEIGHT / 2 - playerRunHeight)
	{
		obj_player.velocity.y = -tempCurrentVelocityY;
		gameState.currentVelocity.y = 0;
	}
	//Otherwise don't adjust the player's position and don't adjust the current velocity
	else
	{
		gameState.currentVelocity.y = tempCurrentVelocityY;
	}
}

void DrawScene()
{
	//Draw background
	std::vector<int> vBackground = Play::CollectGameObjectIDsByType(TYPE_BACKGROUND);
	for (int id_background : vBackground)
	{
		Play::DrawObject(Play::GetGameObject(id_background));
	}

	//Draw ground
	std::vector<int> vGround = Play::CollectGameObjectIDsByType(TYPE_GROUND);
	for (int id_ground : vGround)
	{
		Play::DrawObject(Play::GetGameObject(id_ground));
	}

	//Draw platforms
	std::vector<int> vPlatforms = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);
	for (int id_platform : vPlatforms)
	{
		Play::DrawObject(Play::GetGameObject(id_platform));
	}

	//Draw coins
	for (Coin coin : vCoins)
	{
		coin.DrawObject();
	}

	//Draw obstacles
	std::vector<int> vObstacles = Play::CollectGameObjectIDsByType(TYPE_OBSTACLE);
	for (int id_obstacle : vObstacles)
	{
		Play::DrawObject(Play::GetGameObject(id_obstacle));
	}

	//Draw player
	Play::DrawObject(Play::GetGameObjectByType(TYPE_PLAYER));

	//Draw text
	Play::DrawFontText("132px", std::to_string(gameState.score),
		{ DISPLAY_WIDTH / 2, 50 }, Play::CENTRE);

	/*
	//Draw debug text
	Play::DrawFontText("64px", std::to_string(vCoins.size()),
		{ DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 }, Play::CENTRE);
	*/
}