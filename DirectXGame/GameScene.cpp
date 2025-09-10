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

	testM = Model::CreateFromOBJ("cube");
	wor.Initialize();
	wor.translation_ = {0.0f, 0.0f, 0.0f};
	wor.scale_ = {10.0f, 0.1f, 0.1f};
	wor.UpdateMatirx();
}

void GameScene::Update() { 
	player->Update(); 
	map->Update();
}

void GameScene::Draw() { 
	Model::PreDraw();

	map->Draw(camera);

	player->Draw(camera);

	testM->Draw(wor, camera);

	Model::PostDraw();
}
