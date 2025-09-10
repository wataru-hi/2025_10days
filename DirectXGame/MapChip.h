#pragma once
#include <KamataEngine.h>
#include <memory>
#include <string>
#include <vector>

using namespace KamataEngine;

enum class MapChipType {
	Blank,
	Block,
	Goal,
};

class MapChip {
public:
	struct Rect {
		float left;
		float top;
		float right;
		float bottom;
	};
	struct MapChipIndex {
		int x;
		int y;
	};

	~MapChip();

	void Initialize(std::string file);

	void Update();

	void Draw(const Camera& camera);

	MapChipType GetMapChipType(const Vector3& position);

	MapChipType GetMapChipType(const MapChipIndex& index);

	Rect GetMapRect(const Vector3& position);

	MapChipIndex GetMapChipIndex(const Vector3& position);

	int GetMaxVerticalMapSize() { return static_cast<int>(mapChipData.size()); }
	int GetMaxHorizontalMapSize() { return static_cast<int>(mapChipData[0].size()); }

	float GetMapChipBlockSize() { return BlockSize; }

private:
	void MapCreate(std::string file);

	Vector3 GetMapPos(const MapChipIndex& index);
	void CreateModel();

private:
	std::vector<std::vector<MapChipType>> mapChipData;
	std::vector<std::vector<WorldTransform*>> mapChipWorldTransforms_;

	std::string directory = "";

	Model* blockModel;
	Model* GoalModel;

	float BlockSize = 2.0f;

	Vector2 maxMapSize;
};