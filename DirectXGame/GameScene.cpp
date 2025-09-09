#include "GameScene.h"



using namespace KamataEngine;
GameScene::~GameScene() { 

}
void GameScene::Initialize() { 
	camera.Initialize(); 

	

	map = std::make_unique<MapChip>();
	map->Initialize("Resources/stage1.csv");

	player = std::make_unique<Player>();
	player->Initialize();

	player->SetMapChipData(map.get());
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
