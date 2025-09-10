#pragma once
#include <KamataEngine.h>
using namespace KamataEngine;

class ClearScene {
public:
	enum class Phase {
		kFadeIn,  // フェードイン
		kMain,    // メイン部
		kFadeOut, // フェードアウト
	};
	ClearScene();
	~ClearScene();

	/// <summary>
	/// 初期化
	/// <summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// <summary>
	void Update();

	/// <summary>
	/// 描画
	/// <summary>
	void Draw();

	// デスフラグのgetter
	bool IsFinished() const { return finished_; }

private:
	// 終了フラグ
	bool finished_ = false;

	// ビュープロジェクション
	Camera camera_;

	WorldTransform worldTransformFont_;

	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	Sprite* sprite_ = nullptr;

	uint32_t textureHandle_ = 0;

	// 現在のフェーズ
	Phase phase_ = Phase::kFadeIn;
};
