//#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include "Coin.h"
#include "MainGame.h"

Coin::Coin(Vector2f platformPiecePos)
{
	pos = { platformPiecePos.x, platformPiecePos.y - playerRunHeight - 25 };
	velocity = gameState.currentVelocity;
	collisionRadius = consts::spriteWidth_coin / 2;
	spriteId = PlayGraphics::Instance().GetSpriteId("Coin");
	frame = 0;

	/*
	Vector2f coinPos = { platformPiecePos.x, platformPiecePos.y - playerRunHeight - 25 };

	std::vector<int> vCoins = Play::CollectGameObjectIDsByType(TYPE_COIN);

	//Object pooling
	for (int id_coin : vCoins)
	{
		GameObject& obj_coin = Play::GetGameObject(id_coin);

		if (obj_coin.pos.x <= -consts::spriteWidth_coin / 2)
		{
			obj_coin.pos = coinPos;
			return;
		}
	}

	int id_coin = Play::CreateGameObject(TYPE_COIN, coinPos, consts::spriteWidth_coin / 2, "coin");
	Play::GetGameObject(id_coin).animSpeed = 0.05f;
	*/
}

void Coin::Update()
{
	velocity = gameState.currentVelocity;
	pos += velocity;

	if (gameState.playerState != STATE_DEAD && IsCollidingWithPlayer() == true)
	{
		gameState.score += 10;
		pos.x = -consts::spriteWidth_coin;	//Move coin off screen
	}
}

bool Coin::IsCollidingWithPlayer()
{
	GameObject& obj_player = Play::GetGameObjectByType(TYPE_PLAYER);

	int xDiff = int(pos.x) - int(obj_player.pos.x);
	int yDiff = int(pos.y) - int(obj_player.pos.y);
	int radii = obj_player.radius + collisionRadius;

	// Game progammers don't do square root!
	return((xDiff * xDiff) + (yDiff * yDiff) < radii * radii);
}

void Coin::DrawObject()
{
	PlayGraphics::Instance().Draw(spriteId, pos, frame);
}
