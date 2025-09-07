#include "MapChip.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace MathUtility;

MapChip::~MapChip() {
	for (auto line : mapChipTransforms_) {
		for (auto& sprite : line) {
			if (sprite) {
				delete sprite;
			}
		}
	}

}

void MapChip::Initialize(std::string fileName) {
	std::ifstream file(fileName);
	if (!file) {
		std::cerr << "ファイルを開けませんでした。" << std::endl;
		return;
	}

	std::string line;
	while (std::getline(file, line)) {
		mapChipData.push_back(std::vector<MapChipType>());
		int lineNumber = static_cast<int>(mapChipData.size()) - 1;
		std::stringstream ss(line); // 行をストリーム化
		std::string cell;

		// カンマ区切りで分解
		while (std::getline(ss, cell, ',')) {
			int value = std::stoi(cell); // 文字列を整数に変換
			mapChipData[lineNumber].push_back(static_cast<MapChipType>(value));
		}

	}
	maxMapSize.x = static_cast<int>(mapChipData[0].size()) * BlockSize;
	maxMapSize.y = static_cast<int>(mapChipData.size()) * BlockSize;
	ModelCreate();

	blockModel_ = std::unique_ptr<Model>(Model::CreateFromOBJ("cube"));
}

void MapChip::Update() {

}

void MapChip::Draw(const Camera& camera) {
	int a = 0;

	for (auto line : mapChipTransforms_) {
		for (auto& worldTransform : line) {
			if (worldTransform) {
				blockModel_->Draw(*worldTransform, camera);
				a++;
			}
		}
	}

}

MapChipType MapChip::GetMapChipType(const Vector2& position) { 
	int xIndex = static_cast<int>(position.x / BlockSize);
	int yIndex = static_cast<int>(position.y / BlockSize);
	xIndex = std::max<int>(0, std::min<int>(xIndex, static_cast<int>(mapChipData[0].size()) - 1));
	yIndex = std::max<int>(0, std::min<int>(yIndex, static_cast<int>(mapChipData.size()) - 1));
	return mapChipData[yIndex][xIndex];
}

MapChip::Rect MapChip::GetMapRect(const Vector2& position) {
	int xIndex = static_cast<int>(position.x / BlockSize);
	int yIndex = static_cast<int>(position.y / BlockSize);
	xIndex = std::max<int>(0, std::min<int>(xIndex, static_cast<int>(mapChipData[0].size()) - 1));
	yIndex = std::max<int>(0, std::min<int>(yIndex, static_cast<int>(mapChipData.size()) - 1));
	Rect rect;
	rect.top = yIndex * BlockSize;
	rect.bottom = rect.top + BlockSize;
	rect.left = xIndex * BlockSize;
	rect.right = rect.left + BlockSize;

	return rect;
}

void MapChip::ModelCreate() {
	size_t y = 0;
	// マップの全行数
	size_t totalRows = mapChipData.size();

	for (auto line : mapChipData) {
		size_t x = 0;
		mapChipTransforms_.push_back(std::vector<WorldTransform*>());
		float yPos = (totalRows - 1 - y) * BlockSize;

		for (auto cell : line) {
			int lineIndex = static_cast<int>(mapChipTransforms_.size()) - 1;

			if (cell == MapChipType::Blank) {
				mapChipTransforms_[lineIndex].push_back(nullptr);
			} else if (cell == MapChipType::Block) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				// 座標を計算
				worldTransform->translation_ = {x * BlockSize, yPos, 0.0f};
				worldTransform->scale_ = {1.0f, 1.0f, 1.0f}; // 3Dモデルのスケールも調整
				worldTransform->rotation_ = {0.0f, 0.0f, 0.0f};
				worldTransform->UpdateMatirx();
				mapChipTransforms_[lineIndex].push_back(worldTransform);
			}
			x++;
		}
		y++;
	}

}
