#include "GameScene.h"



using namespace KamataEngine;
GameScene::~GameScene() { 
	delete map;
}
void GameScene::Initialize() {
	camera.Initialize();

	map = new MapChip();
	map->Initialize("Resources/mapTest2.csv");

	player = std::make_unique<Player>();
	player->Initialize(map);

}

void GameScene::Update() { 
	player->Update(); 
	map->Update();
}

void GameScene::Draw() { 
	Model::PreDraw();

	map->Draw(camera);

	player->Draw(camera);

	Model::PostDraw();
}
