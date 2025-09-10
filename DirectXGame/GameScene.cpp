#include "GameScene.h"
#include <memory>

using namespace KamataEngine;

GameScene::~GameScene() { }

void GameScene::Initialize() {
	// Camera の初期化は FollowCamera に任せる

	map = new MapChip();
	map->Initialize("Resources/stage1.csv");

	player = std::make_unique<Player>();
	player->Initialize(map);

	// FollowCamera を初期化し、プレイヤーを追跡ターゲットとして設定
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize(player.get());
}

void GameScene::Update() {
	player->Update();
	map->Update();

	// FollowCamera を更新
	followCamera_->Update();

	isGoal_ = player->isGoal();
}

void GameScene::Draw() {
	// FollowCamera からカメラインスタンスを取得して描画
	const Camera& camera = followCamera_->GetCamera();

	Model::PreDraw();
	map->Draw(camera);
	player->Draw(camera);
	Model::PostDraw();
}

bool GameScene::IsGoalPlayer() { return isGoal_; }
