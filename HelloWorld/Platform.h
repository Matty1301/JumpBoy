#pragma once

const int platformHeight = 240;

void UpdatePlatforms();
int SpawnPlatform(int platformLevel);
void SpawnPlatformPiece(Vector2f, const char*);
