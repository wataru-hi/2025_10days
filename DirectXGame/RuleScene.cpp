#include "RuleScene.h"

RuleScene::RuleScene() {}
RuleScene::~RuleScene() { delete sprite_; }

void RuleScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	// ビュープロジェクション
	camera_.Initialize();

	textureHandle_ = TextureManager::Load("rule.png");
	sprite_ = Sprite::Create(textureHandle_, {0, 0});
}

void RuleScene::Update() {
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		if (phase_ != Phase::kFadeOut) {
			phase_ = Phase::kFadeOut;
			finished_ = true;
		}
	}
}

void RuleScene::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);
	sprite_->Draw();
	// スプライト描画後処理
	Sprite::PostDraw();
}
