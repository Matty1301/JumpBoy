#pragma once

class Coin
{
private:
	Vector2f pos;
	Vector2f velocity;
	int collisionRadius;
	int spriteId;
	int frame;

	bool IsCollidingWithPlayer();

public:
	Coin(Vector2f);
	void Update();
	void DrawObject();
};