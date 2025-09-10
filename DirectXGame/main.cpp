#include <Windows.h>

#include <KamataEngine.h>

#include "TitleScene.h"
#include "RuleScene.h"
#include "ClearScene.h"

#include "GameScene.h"

using namespace KamataEngine;

// シーン（型）
enum class Scene {
	kUnknown = 0,
	kTitle,
	kRule,
	kGame,
	kClear,
};

// 現在シーン（型）
Scene scene = Scene::kUnknown;

void ChangeScene();
void UpdateScene();
void DrawScene();

TitleScene* titleScene = nullptr;
RuleScene* ruleScene = nullptr;
std::unique_ptr<GameScene> gameScene = std::make_unique<GameScene>();
ClearScene* clearScene = nullptr;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	
	KamataEngine::Initialize(L"3065_Switching of Geist");

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	ImGuiManager* imGuiManager = ImGuiManager::GetInstance();

	// ゲームシーンの初期化
	titleScene = new TitleScene();
	titleScene->Initialize();
	scene = Scene::kTitle;

	while (true) {

		if (KamataEngine::Update()) {
			break;
		}

		imGuiManager->Begin();

		ChangeScene();
		UpdateScene();



		imGuiManager->End();

		dxCommon->PreDraw();

		DrawScene();
		
		

		imGuiManager->Draw();

		dxCommon->PostDraw();
	}

	// 各種解放
	delete titleScene;
	delete ruleScene;
	delete clearScene;

	KamataEngine::Finalize();

	return 0;
}

// シーン切り替え
// シーン切り替え
void ChangeScene() {

	switch (scene) {
	case Scene::kTitle:
		if (titleScene->IsFinished()) {
			// シーン変更
			scene = Scene::kRule;
			// 旧シーンの開放
			delete ruleScene;
			ruleScene = nullptr;
			// 新シーンの生成と初期化
			ruleScene = new RuleScene;
			ruleScene->Initialize();
		}
		break;
	case Scene::kRule:
		if (ruleScene->IsFinished()) {
			// シーン変更
			scene = Scene::kGame;
			
			gameScene.reset();
			// 新シーンの生成と初期化
			gameScene = std::make_unique<GameScene>();
			
			gameScene->Initialize();
		}
		break;
	case Scene::kGame:
		if (gameScene->IsGoalPlayer()) {
			// シーン変更
			scene = Scene::kClear;
			// 旧シーンの開放
			delete clearScene;
			gameScene->AudioStop();
			gameScene.reset();
			clearScene = nullptr;
			// 新シーンの生成と初期化
			clearScene = new ClearScene;
			clearScene->Initialize();
		}
		break;
	case Scene::kClear:
		if (clearScene->IsFinished()) {
			// シーン変更
			scene = Scene::kTitle;
			// 旧シーンの開放
			delete titleScene;
			titleScene = nullptr;
			// 新シーンの生成と初期化
			titleScene = new TitleScene;
			titleScene->Initialize();
		}
		break;
	}
}

// 現在シーン更新
void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kRule:
		ruleScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	case Scene::kClear:
		clearScene->Update();
		break;
	}
}

// 現在シーンの描画
void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kRule:
		ruleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	case Scene::kClear:
		clearScene->Draw();
		break;
	}
}