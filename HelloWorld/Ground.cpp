//#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include "Ground.h"
#include "MainGame.h"

void UpdateGround()
{
	std::vector<int> vGround = Play::CollectGameObjectIDsByType(TYPE_GROUND);
	for (int id_ground : vGround)
	{
		GameObject& obj_tile = Play::GetGameObject(id_ground);

		obj_tile.velocity = gameState.currentVelocity;
		Play::UpdateGameObject(obj_tile);

		//If tile goes off LHS bring it back on RHS
		if (obj_tile.pos.x < -(consts::spriteWidth_ground / 2))
			obj_tile.pos.x += consts::spriteWidth_ground * vGround.size();
	}
}
