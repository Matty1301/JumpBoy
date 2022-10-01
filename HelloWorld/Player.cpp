//#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include "Player.h"
#include "MainGame.h"

void UpdatePlayer()
{
	GameObject& obj_player = Play::GetGameObjectByType(TYPE_PLAYER);

	Play::UpdateGameObject(obj_player);

	int tileHeight = Play::GetSpriteHeight(gameState.ptr_activeTile->spriteId);

	//This is the position of the top of the active tile after it is updated
	int tileTopNewPosY = gameState.ptr_activeTile->pos.y - tileHeight / 2 + gameState.currentVelocity.y;

	switch (gameState.playerState)
	{
	case STATE_RUN:
		HandlePlayerControls();

		//If the active tile is beneath the player start falling
		if (tileTopNewPosY > obj_player.pos.y + playerRunHeight)
		{
			gameState.playerState = STATE_AIRBORNE;

			Play::SetSprite(obj_player, "jump", 0.125f);
			gameState.relativeAcceleration.y = gravityAcceleration;
		}
		break;

	case STATE_AIRBORNE:
		HandlePlayerControls();

		//If the player was above the active tile last frame but is on or below it this frame start running
		//active tile 'old' pos isn't needed here because it hasn't been updated for the current frame yet
		if (tileTopNewPosY <= obj_player.pos.y + playerRunHeight
			&& gameState.ptr_activeTile->oldPos.y - tileHeight / 2 > obj_player.oldPos.y + playerRunHeight)
		{
			gameState.playerState = STATE_RUN;

			Play::SetSprite(obj_player, "run", 0.3f);
			gameState.relativeVelocity.y = 0;
			gameState.relativeAcceleration.y = 0;
		}
		break;

	case STATE_SLIDE:
		if (Play::IsAnimationComplete(obj_player))
		{
			gameState.playerState = STATE_RUN;

			Play::SetSprite(obj_player, "run", 0.3f);
			obj_player.radius = playerCollisionRadius;
		}

		//If the active tile is beneath the player start falling
		if (tileTopNewPosY > obj_player.pos.y + playerRunHeight)
		{
			gameState.playerState = STATE_AIRBORNE;

			Play::SetSprite(obj_player, "jump", 0.125f);
			gameState.relativeAcceleration.y = gravityAcceleration;
			obj_player.radius = playerCollisionRadius;
		}
		break;

	case STATE_DEAD:
		//If the player lands on the active tile stop moving
		if (tileTopNewPosY <= obj_player.pos.y + playerRunHeight)
		{
			gameState.relativeVelocity = { 0, 0 };
			gameState.relativeAcceleration = { 0, 0 };
		}

		if (Play::IsAnimationComplete(obj_player))
		{
			obj_player.animSpeed = 0;
			if (Play::KeyPressed(VK_SPACE))
				ResetGame();
		}
	}
}

void HandlePlayerControls()
{
	GameObject& obj_player = Play::GetGameObjectByType(TYPE_PLAYER);

	//Holding down the spacebar let's you jump higher
	if (gameState.playerState == STATE_AIRBORNE)
	{
		if (Play::KeyDown(VK_SPACE) && gameState.relativeVelocity.y > 0)
		{
			gameState.relativeAcceleration.y = gravityAcceleration * gravityMultiplier;
		}
		else
		{
			gameState.relativeAcceleration.y = gravityAcceleration;
		}
	}

	if (gameState.playerState == STATE_RUN)
	{
		if (Play::KeyPressed(VK_SPACE))
		{
			gameState.playerState = STATE_AIRBORNE;

			Play::SetSprite(obj_player, "jump", 0.125f);
			gameState.relativeVelocity.y = playerJumpSpeed;
			gameState.relativeAcceleration.y = gravityAcceleration;
		}

		if (Play::KeyPressed(VK_DOWN))
		{
			gameState.playerState = STATE_SLIDE;

			Play::SetSprite(obj_player, "slide", 0.08f);
			obj_player.radius /= 2;
		}
	}
}

void PlayerDeath()
{
	gameState.playerState = STATE_DEAD;

	Play::SetSprite(Play::GetGameObjectByType(TYPE_PLAYER), "dead", 0.3f);

	//If the player is jumping start falling
	if (gameState.relativeVelocity.y > 0)
	{
		gameState.relativeAcceleration.y = gravityAcceleration;
		gameState.relativeVelocity.y = 0;
	}
}