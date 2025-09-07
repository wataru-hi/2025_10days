#pragma once
#include <KamataEngine.h>
#include <string>
#include <vector>
#include <memory>

using namespace KamataEngine;

enum class MapChipType {
	Blank,
	Block,
};


class MapChip {
public:
	struct Rect {
		float left;
		float top;
		float right;
		float bottom;
	};
	~MapChip();

	void Initialize(std::string file);
	
	void Update();

	void Draw(const Camera& camera);

	MapChipType GetMapChipType(const Vector2& position);

	Rect GetMapRect(const Vector2& position);

	void SetPlayerPos(const Vector2& pos) { playerPos_ = pos; }

	Vector2 GetScroll() { return scroll_; }

private:
	void ModelCreate();
	std::vector<std::vector<MapChipType>> mapChipData;
	std::vector<std::vector<WorldTransform*>> mapChipTransforms_;
	
	float BlockSize = 2.0f;

	Vector2 maxMapSize;

	Vector2 playerPos_;

	Vector2 scroll_ = {0, 0};

	std::unique_ptr<Model> blockModel_ = nullptr;

};
