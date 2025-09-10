#pragma once
#include <KamataEngine.h>
#include <string>
#include <vector>
#include <list>
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
	struct MapChipIndex {
		int x;
		int y;
	};
	struct MapData {
		MapChipType type;
		Sprite* sprite;
		Sprite* mapSprite;
	};
	~MapChip();

	void Initialize(std::string file);
	
	void Update();

	void Draw(const Camera& camera);

	//void MapDraw();

	MapChipType GetMapChipType(const Vector2& position);

	MapChipType GetMapChipType(const MapChipIndex& index);

	Rect GetMapRect(const Vector2& position);

	//void SetPlayerPos(const Vector2& pos) { playerPos_ = pos; }

	MapChipIndex GetMapChipIndex(const Vector2& position);

	int GetMaxVerticalMapSize() { return static_cast<int>(mapChipData.size()); }
	int GetMaxHorizontalMapSize() { return static_cast<int>(mapChipData[1].size()); }

	float GetMapChipBlockSize() { return BlockSize; }

	//void RoundReset(std::string file);

private:
	void MapCreate(std::string file);

	Vector2 GetMapPos(const MapChipIndex& index);

private:
	//void SpriteCreate();
	void CreateModel();
	std::vector<std::vector<MapChipType>> mapChipData;
	std::vector<std::vector<WorldTransform*>> mapChipWorldTransforms_;
	//std::vector<std::vector<Sprite*>> mapSprites;
	//std::vector<Sprite*> sprites;
	//std::vector<Sprite*> mapsprites;
	//bool isStart =false;
	//Sprite* payerMapSprite_ = nullptr;
	//Sprite* backMapSprite_ = nullptr;
	//std::vector<Sprite*> itemMapSprite;
	//std::vector<Sprite*> enemyMapSprite;

	std::string directory = "";

	int a[100][100];
	
	Model* blockModel;

	float BlockSize = 2.0f;
	float mapBlockSize = 0.0f;

	Vector2 maxMapSize;
	const float kMapSize = 200.0f;

	Vector2 playerPos_;

	//Vector2 scroll_ = {0, 0};

};
