#pragma once
#include <KamataEngine.h>
#include "Player.h"

//class Player;
class GameScene {
public:
	void Initialize();
	void Update();
	void Draw();

private:
	KamataEngine::Camera camera;

	std::unique_ptr<Player> player;
};
