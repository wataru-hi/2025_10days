#include <KamataEngine.h>
using namespace KamataEngine;
#include <algorithm>

class Fade {
public:
	enum class Status {
		None,    // フェードなし
		FadeIn,  // フェードイン中
		FadeOut, // フェードアウト中
	};

	/// コンストクラタ
	Fade();
	/// デストラクタ
	~Fade();

	void Initialize();
	void Update();
	void Draw(ID3D12GraphicsCommandList* commandList);

	// フェード開始
	void Start(Status status, float duration);
	void Stop();

	// フェード終了判定
	bool IsFinished() const;

	bool IsFadeOutFinished() const;

private:
	Sprite* sprite_ = nullptr;

	// 現在のフェードの状態
	Status status_ = Status::None;

	// フェードの持続時間
	float duration_ = 0.0f;
	// 経過時間カウンター
	float counter_ = 0.0f;
};
