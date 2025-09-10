#include "Fade.h"

Fade::Fade() {}

Fade::~Fade() {}

void Fade::Initialize() {

	sprite_ = Sprite::Create(0, Vector2{});
	sprite_->SetSize(Vector2(1280, 720));
	sprite_->SetColor(Vector4(0, 0, 0, 1));
}

void Fade::Update() {
	// フェード状態による分岐
	switch (status_) {
	case Status::None:
		// 何もしない
		break;
	case Status::FadeIn:
		// 1フレーム分の秒数をカウントダウン
		counter_ += 1.0f / 60.0f;
		// フェード継続時間に達したら打ち止め
		if (counter_ >= duration_) {
			counter_ = duration_;
		}
		// 0.0fから1.0fの間で、経過時間がフェード継続時間に近づくほどアルファ値を大きくする
		sprite_->SetColor(Vector4(0, 0, 0, 1.0f - std::clamp(counter_ / duration_, 0.0f, 1.0f)));
		break;
		break;
	case Status::FadeOut:
		// 1フレーム分の秒数をカウントアップ
		counter_ += 1.0f / 60.0f;
		// フェード継続時間に達したら打ち止め
		if (counter_ >= duration_) {
			counter_ = duration_;
		}
		// 0.0fから1.0fの間で、経過時間がフェード継続時間に近づくほどアルファ値を大きくする
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(counter_ / duration_, 0.0f, 1.0f)));
		break;
	}
}

void Fade::Draw(ID3D12GraphicsCommandList* commandList) {

	if (status_ == Status::None) {
		return;
	}

	Sprite::PreDraw(commandList);
	sprite_->Draw();
	Sprite::PostDraw();
}

void Fade::Start(Status status, float duration) {
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
}

void Fade::Stop() { status_ = Status::None; }

// フェードインとフェードアウトが終了した時に変わるフラグ
bool Fade::IsFinished() const {
	switch (status_) {
	case Status::FadeIn:

	case Status::FadeOut:
		if (counter_ >= duration_) {
			return true;
		} else {
			return false;
		}
	}
	return true;
}

// フェードアウトが終了の時のフラグ
bool Fade::IsFadeOutFinished() const {
	switch (status_) {
	case Status::FadeOut:
		if (counter_ >= duration_) {
			return true;
		} else {
			return false;
		}
	}
	return false;
}
