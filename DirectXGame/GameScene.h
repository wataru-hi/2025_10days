#pragma once
#include <KamataEngine.h>
#include "Player.h"
#include "MapChip.h"


//class Player;
class GameScene {
public:
	void Initialize();
	void Update();
	void Draw();

private:
	KamataEngine::Camera camera;

	std::unique_ptr<Player> player;
	std::unique_ptr<MapChip> map;
};
