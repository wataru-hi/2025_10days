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

	delete blockModel;
	blockModel = nullptr;
}

void MapChip::Initialize(std::string fileName) {
	directory = fileName;
	
	MapCreate(fileName);
	CreateModel();

	blockModel = Model::CreateFromOBJ("cube");

}

void MapChip::Update() {
	
}

void MapChip::Draw(const Camera& camera) {
	for (const auto& line : mapChipWorldTransforms_) {
		for (const auto& worldTransform : line) {
			if (worldTransform) {
				blockModel->Draw(*worldTransform, camera);
			}
		}
	}

}


MapChipType MapChip::GetMapChipType(const Vector2& position) {
	MapChipIndex index = GetMapChipIndex(position);
	return mapChipData[index.y][index.x];
}

MapChipType MapChip::GetMapChipType(const MapChipIndex& index) { return mapChipData[index.y][index.x]; }

MapChip::Rect MapChip::GetMapRect(const Vector2& position) {
	MapChipIndex index = GetMapChipIndex(position);
	Rect rect;
	rect.top = index.y * BlockSize;
	rect.bottom = rect.top + BlockSize;
	rect.left = index.x * BlockSize;
	rect.right = rect.left + BlockSize;

	return rect;
}

MapChip::MapChipIndex MapChip::GetMapChipIndex(const Vector2& position) {
	MapChipIndex index;
	index.x = static_cast<int>(position.x / BlockSize);
	index.y = static_cast<int>(position.y / BlockSize);
	index.x = std::clamp<int>(index.x, 0, static_cast<int>(mapChipData[0].size()) - 1);
	index.y = std::clamp<int>(index.y, 0, static_cast<int>(mapChipData.size()) - 1);
	return index;
}

void MapChip::MapCreate(std::string fileName) {
	// ファイルを開く
	std::ifstream file(fileName);
	if (!file) {
		std::cerr << "ファイルを開けませんでした。" << std::endl;
		return;
	}

	std::string line;
	// 行ごとに読み込み
	while (std::getline(file, line)) {
		mapChipData.push_back(std::vector<MapChipType>());
		int lineNumber = static_cast<int>(mapChipData.size()) - 1;
		std::stringstream ss(line); // 行をストリーム化
		std::string cell;

		// カンマ区切りで分解
		while (std::getline(ss, cell, ',')) {
			// 文字列を整数に変換
			int value = std::stoi(cell);
			mapChipData[lineNumber].push_back(static_cast<MapChipType>(value));
		}
	}
	// マップの最大サイズ計算
	maxMapSize.x = static_cast<int>(mapChipData[0].size()) * BlockSize;
	maxMapSize.y = static_cast<int>(mapChipData.size()) * BlockSize;
	// スプライト部分生成
	//SpriteCreate();
}

Vector2 MapChip::GetMapPos(const MapChipIndex& index) { return Vector2(1280.0f - kMapSize + mapBlockSize * index.x + mapBlockSize * 0.5f, mapBlockSize * index.y + mapBlockSize * 0.5f); }

void MapChip::CreateModel() {

	//mapBlockSize = kMapSize / static_cast<float>(mapChipData.size());
	// 不要なコメントは削除
	for (auto line : mapChipData) {
		mapChipWorldTransforms_.push_back(std::vector<WorldTransform*>());
		int lineIndex = static_cast<int>(mapChipWorldTransforms_.size()) - 1;

		for (auto cell : line) {
			int cellIndex = static_cast<int>(mapChipWorldTransforms_[lineIndex].size());

			if (cell == MapChipType::Blank) {
				mapChipWorldTransforms_[lineIndex].push_back(nullptr);
				a[cellIndex][lineIndex] = 0;
			} else if (cell == MapChipType::Block) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransform->translation_ = {
					(BlockSize * 0.5f) * cellIndex,
					(BlockSize * 0.5f) * (static_cast<int>(mapChipData.size()) - 1 - lineIndex),
					0.0f};
				worldTransform->UpdateMatirx();
				mapChipWorldTransforms_[lineIndex].push_back(worldTransform);
				a[cellIndex][lineIndex] = 1;
			}
		}
	}
}
