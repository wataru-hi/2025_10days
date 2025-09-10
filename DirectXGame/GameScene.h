#pragma once
#include <KamataEngine.h>
#include "Player.h"
#include "MapChip.h"


//class Player;
class GameScene {
public:
	~GameScene();
	void Initialize();
	void Update();
	void Draw();

private:
	KamataEngine::Camera camera;

	std::unique_ptr<Player> player;
	MapChip* map;

	Model* testM;
	WorldTransform wor;
};
