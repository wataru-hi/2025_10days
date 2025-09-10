#pragma once
#include "FolloCamera.h"
#include "MapChip.h"
#include "Player.h"
#include <KamataEngine.h>

class GameScene {
public:
	~GameScene();
	void Initialize();
	void Update();
	void Draw();

	bool IsGoalPlayer();


private:
	std::unique_ptr<FollowCamera> followCamera_; // FollowCamera のインスタンス

	std::unique_ptr<Player> player;
	MapChip* map;

	bool isGoal_ = false;
};