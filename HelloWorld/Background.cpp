//#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include "Background.h"
#include "MainGame.h"

void UpdateBackground()
{
	std::vector<int> vBackground = Play::CollectGameObjectIDsByType(TYPE_BACKGROUND);

	for (int id_background : vBackground)
	{
		GameObject& obj_background = Play::GetGameObject(id_background);

		obj_background.velocity = gameState.currentVelocity;
		Play::UpdateGameObject(obj_background);

		//If background goes off LHS bring it back on RHS
		if (obj_background.pos.x < -(consts::spriteWidth_BG / 2))
			obj_background.pos.x += consts::spriteWidth_BG * 2;
	}
}
