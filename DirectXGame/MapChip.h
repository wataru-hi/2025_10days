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
	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};

	struct Rect {
		float left;
		float top;
		float right;
		float bottom;
	};

	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	

	~MapChip();

	void Initialize(std::string file);
	
	void Update();

	void Draw(const Camera& camera);

	MapChipType GetMapChipType(const Vector2& position);

	Rect GetMapRect(const Vector2& position);

	void SetPlayerPos(const Vector2& pos) { playerPos_ = pos; }

	Vector2 GetScroll() { return scroll_; }

	IndexSet GetMapChipIndexSetByPosition(const Vector3& position);

	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);
	MapChipType GetMapChipTypeByPosition(const Vector3& position);
	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

private:
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	void ModelCreate();
	std::vector<std::vector<MapChipType>> mapChipData;
	std::vector<std::vector<WorldTransform*>> mapChipTransforms_;
	
	float BlockSize = 2.0f;

	Vector2 maxMapSize;

	Vector2 playerPos_;

	Vector2 scroll_ = {0, 0};

	std::unique_ptr<Model> blockModel_ = nullptr;

};
