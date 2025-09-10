#include "MapChip.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace MathUtility;

MapChip::~MapChip() {
	for (auto line : mapChipWorldTransforms_) {
		for (auto& sprite : line) {
			if (sprite) {
				delete sprite;
				sprite = nullptr;
			}
		}
	}



	blockModel = nullptr;
	GoalModel = nullptr;
}

void MapChip::Initialize(std::string fileName) {
	directory = fileName;

	MapCreate(fileName);
	CreateModel();

	blockModel = Model::CreateFromOBJ("block");
	GoalModel = Model::CreateFromOBJ("door");
}

void MapChip::Update() {}

void MapChip::Draw(const Camera& camera) {
	int y = 0; // 行のインデックスを初期化
	for (const auto& line : mapChipWorldTransforms_) {
		int x = 0; // 列のインデックスを初期化
		for (const auto& worldTransform : line) {
			if (worldTransform) {
				// 正しいインデックスを使ってマップチップのタイプを取得
				MapChipType type = GetMapChipType(MapChipIndex{x, y});

				if (MapChipType::Block == type) {
					blockModel->Draw(*worldTransform, camera);
				}
				if (MapChipType::Goal == type) {
					GoalModel->Draw(*worldTransform, camera);
				}
			}
			x++;
		}
		y++;
	}
}


MapChipType MapChip::GetMapChipType(const Vector3& position) {
	MapChipIndex index = GetMapChipIndex(position);
	if (index.y < 0 || index.y >= mapChipData.size() || index.x < 0 || index.x >= mapChipData[0].size()) {
		return MapChipType::Blank; // 範囲外の場合はBlankを返す
	}
	return mapChipData[index.y][index.x];
}

MapChipType MapChip::GetMapChipType(const MapChipIndex& index) { return mapChipData[index.y][index.x]; }

MapChip::Rect MapChip::GetMapRect(const Vector3& position) {
	MapChipIndex index = GetMapChipIndex(position);
	Rect rect;
	rect.top = index.y * BlockSize;
	rect.bottom = rect.top + BlockSize;
	rect.left = index.x * BlockSize;
	rect.right = rect.left + BlockSize;

	return rect;
}

MapChip::MapChipIndex MapChip::GetMapChipIndex(const Vector3& position) {
	MapChipIndex index;
	index.x = static_cast<int>(position.x / BlockSize);
	index.y = GetMaxVerticalMapSize() - 1 - static_cast<int>(position.y / BlockSize);
	index.x = std::clamp<int>(index.x, 0, static_cast<int>(mapChipData[0].size()) - 1);
	index.y = std::clamp<int>(index.y, 0, static_cast<int>(mapChipData.size()) - 1);
	return index;
}

void MapChip::MapCreate(std::string fileName) {
	std::ifstream file(fileName);
	if (!file) {
		std::cerr << "ファイルを開けませんでした。" << std::endl;
		return;
	}

	std::string line;
	while (std::getline(file, line)) {
		mapChipData.push_back(std::vector<MapChipType>());
		int lineNumber = static_cast<int>(mapChipData.size()) - 1;
		std::stringstream ss(line);
		std::string cell;

		while (std::getline(ss, cell, ',')) {
			int value = std::stoi(cell);
			mapChipData[lineNumber].push_back(static_cast<MapChipType>(value));
		}
	}
	maxMapSize.x = static_cast<int>(mapChipData[0].size()) * BlockSize;
	maxMapSize.y = static_cast<int>(mapChipData.size()) * BlockSize;
}

Vector3 MapChip::GetMapPos(const MapChipIndex& index) { return Vector3(BlockSize * index.x, BlockSize * index.y, 0.0f); }

void MapChip::CreateModel() {
	for (size_t y = 0; y < mapChipData.size(); ++y) {
		mapChipWorldTransforms_.push_back(std::vector<WorldTransform*>());
		for (size_t x = 0; x < mapChipData[y].size(); ++x) {
			if (mapChipData[y][x] == MapChipType::Blank) {
				mapChipWorldTransforms_[y].push_back(nullptr);
			} else if (mapChipData[y][x] == MapChipType::Block) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransform->translation_ = {(BlockSize * 0.5f) + (BlockSize * x), (BlockSize * 0.5f) + (BlockSize * (mapChipData.size() - 1 - y)), 0.0f};
				worldTransform->UpdateMatirx();
				mapChipWorldTransforms_[y].push_back(worldTransform);
			}
		}
	}
}