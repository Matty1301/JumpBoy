#pragma once

enum PlayerState
{
	STATE_RUN = 0,
	STATE_AIRBORNE,
	STATE_SLIDE,
	STATE_DEAD,
};

const float playerRunSpeed = -5.0f;
const int playerRunHeight = 35;	//Distance from sprite centre to feet
const int playerJumpSpeed = 12;
const int playerCollisionRadius = 30;

void UpdatePlayer();
void HandlePlayerControls();
void PlayerDeath();
