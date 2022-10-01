#pragma once
#include "Player.h"
#include "Coin.h"

const int DISPLAY_WIDTH = 800;
const int DISPLAY_HEIGHT = 600;
const int DISPLAY_SCALE = 1;

const float constElapsedTime = 1.0f / 60;	//Const elapsed time doesn't vary with FPS
const float gravityAcceleration = -0.7f;
const float gravityMultiplier = 0.33f;	//When spacebar is held down

//Inline keyword prevents variable being defined multiple times which would lead to a linker error
inline std::vector<Coin> vCoins;

namespace consts
{
	//Would be better if these could be made const variables
	//but must be defined after play
	inline int spriteWidth_BG = 0;
	inline int spriteHeight_BG = 0;
	inline int spriteHeight_ground = 0;
	inline int spriteWidth_ground = 0;
	inline int spriteWidth_player = 0;
	inline int spriteWidth_coin = 0;
	inline int spriteWidth_platform = 0;
	inline int spriteHeight_platform = 0;
	inline int spriteWidth_obstacle = 0;
	const Vector2f spritePos_player = { 100, DISPLAY_HEIGHT / 2 - playerRunHeight };
};

struct GameState
{
	float timer = 0;
	int score = 0;

	//This must be a pointer and not a refernce because references cannot be reassigned
	GameObject* ptr_activeTile;

	std::vector<float> timeToSpawnPlatforms =
	{
		Play::RandomRoll(8) * 0.2f,
		Play::RandomRoll(8) * 0.2f,
		Play::RandomRoll(8) * 0.2f,
		Play::RandomRoll(8) * 0.2f,
	};

	PlayerState playerState = STATE_RUN;
	Vector2f relativeVelocity = { playerRunSpeed, 0 };	//Velocity of objects relative to the player
	Vector2f currentVelocity = { playerRunSpeed, 0 };	//Velocity of objects relative to the screen. Calculated each frame from the relative velocity
	Vector2f relativeAcceleration = { 0, 0 };	//Acceleration of objects relative to the player
};

extern GameState gameState;	//Declaration of gameState, definition in MainGame.cpp

enum GameObjectType
{
	TYPE_NULL = -1,
	TYPE_BACKGROUND,
	TYPE_GROUND,
	TYPE_PLATFORM,
	TYPE_COIN,
	TYPE_OBSTACLE,
	TYPE_PLAYER,
};

void GetActiveTile();
void UpdateCurrentVelocity();
void DrawScene();
void ResetGame();