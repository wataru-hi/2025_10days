#include <Windows.h>

#include <KamataEngine.h>

#include "GameScene.h"

using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	
	KamataEngine::Initialize(L"LE3D_12_ヒガ_ワタル");

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	std::unique_ptr<GameScene>gameScene = std::make_unique<GameScene>();

	gameScene->Initialize();

	while (true) {

		gameScene->Update();


		dxCommon->PreDraw();
		
		gameScene->Draw();

		dxCommon->PostDraw();
	}

	return 0;
}
