#include "GameScene.h"



using namespace KamataEngine;
void GameScene::Initialize() { 
	camera.Initialize(); 

	player = std::make_unique<Player>();
	player->Initialize();

	map = std::make_unique<MapChip>();
	map->Initialize("Resources/stage1.csv");
}

void GameScene::Update() { 
	player->Update(); 
}

void GameScene::Draw() { 
	Model::PreDraw();

	map->Draw(camera);

	player->Draw(camera);

	Model::PostDraw();
}
