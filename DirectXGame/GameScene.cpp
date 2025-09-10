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

	// サウンドデータの読み込み
	soundDataHandle_ = audio_->LoadWave("gameSceneBGM.mp3");
	// 音声再生
	audio_->PauseWave(soundDataHandle_);
	// 第2引数でループ再生を指定
	voiceHandle_ = audio_->PlayWave(soundDataHandle_, true);

	openDoorSoundHandle_ = Audio::GetInstance()->LoadWave("openDoorSE.mp3");
}

void GameScene::Update() {
	player->Update();
	map->Update();

	if (Input::GetInstance()->TriggerKey(DIK_P) && flag == 0) {
		voiceHandle = Audio::GetInstance()->PlayWave(openDoorSoundHandle_, true);
		flag = 1;
	}

	if (flag == 1) {
		stopTimer += 1.0f;
	}

	if (stopTimer > 60.0f) {
		Audio::GetInstance()->StopWave(voiceHandle);
		flag = 0;
		stopTimer = 0.0f;
	}

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
