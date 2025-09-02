#include "GameScene.h"



using namespace KamataEngine;
void GameScene::Initialize() { 
	camera.Initialize(); 

	player = std::make_unique<Player>();
	player->Initialize();
}

void GameScene::Update() { 
	player->Update(); 
}

void GameScene::Draw() { 
	Model::PreDraw();

	player->Draw(camera);

	Model::PostDraw();
}
