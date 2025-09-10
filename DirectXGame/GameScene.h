#pragma once
#include "FolloCamera.h"
#include "MapChip.h"
#include "Player.h"
#include <KamataEngine.h>

class GameScene {
public:
	enum class Phase {
		kFadeIn,  // フェードイン
		kMain,    // メイン部
		kFadeOut, // フェードアウト
	};

	~GameScene();
	void Initialize();
	void Update();
	void Draw();

	bool IsGoalPlayer();

	float stopTimer = 0.0f;

private:
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	std::unique_ptr<FollowCamera> followCamera_; // FollowCamera のインスタンス

	std::unique_ptr<Player> player;
	MapChip* map;

	uint32_t openDoorSoundHandle_ = 0;
	uint32_t voiceHandle = 0;
	int flag = 0;

	// サウドデータハンドル
	uint32_t soundDataHandle_ = 0;
	// 音声再生ハンドル
	uint32_t voiceHandle_ = 0;

	// 現在のフェーズ
	Phase phase_ = Phase::kFadeIn;

	bool isGoal_ = false;
};