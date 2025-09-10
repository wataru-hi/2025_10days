#include "MapChip.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace MathUtility;

MapChip::~MapChip() {
	
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

	for (const auto& line : mapChipTransforms_) {
		for (const auto& worldTransform : line) {
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

	// // マップデータが空の場合は処理を中止
	//if (mapChipData.empty()) {
	//	return MapChipType::Blank; // または適切なデフォルト値を返す
	//}

	//int xIndex = static_cast<int>(position.x / BlockSize);
	//int yIndex = static_cast<int>(position.y / BlockSize);

	//// 範囲チェックをより安全な方法に修正
	//if (yIndex < 0 || yIndex >= mapChipData.size() || xIndex < 0 || xIndex >= mapChipData[0].size()) {
	//	return MapChipType::Blank; // 範囲外はBlankとして扱う
	//}

	//return mapChipData[yIndex][xIndex];
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

MapChip::IndexSet MapChip::GetMapChipIndexSetByPosition(const Vector3& position) { 
	//IndexSet indexSet = {};
	/*indexSet.xIndex = static_cast<uint32_t>((position.x + kBlockWidth / 2.0f) / kBlockWidth);
	indexSet.yIndex = kNumBlockVirtical - 1 - static_cast<uint32_t>(position.y + kBlockHeight / 2.0f / kBlockHeight);
	return indexSet;*/

	IndexSet indexSet = {};

	// X座標のインデックスを計算
	// プレイヤーのX座標をブロックの幅で割る
	indexSet.xIndex = static_cast<uint32_t>(position.x / kBlockWidth);

	// Y座標のインデックスを計算
	// Y軸はゲームのワールド座標（上向きが正）とマップチップ配列（下向きが正）で逆転しているため、
	// まずマップチップ全体の高さからプレイヤーのY座標を引いて、マップの上部からの相対座標に変換
	float relativeY = maxMapSize.y - position.y;
	// その後、ブロックの高さで割ってインデックスに変換
	indexSet.yIndex = static_cast<uint32_t>(relativeY / kBlockHeight);

	return indexSet;
}

MapChipType MapChip::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {
	if (yIndex >= mapChipData.size() || xIndex >= mapChipData[0].size()) {
		return MapChipType::Blank; // 範囲外はBlank
	}

	MapChipType resultChipType = mapChipData.data()[yIndex][xIndex];

	return resultChipType;
}

MapChipType MapChip::GetMapChipTypeByPosition(const Vector3& position) {
	IndexSet indexSet = GetMapChipIndexSetByPosition(position);
	
	return GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
}

Vector3 MapChip::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) {
	// マップの全行数を取得
	size_t totalRows = static_cast<uint32_t>(mapChipData.size());
	// ModelCreate()と同じロジックでY座標を計算
	float yPos = static_cast<float>(totalRows - 1 - yIndex) * kBlockSize;
	return Vector3(static_cast<float>(xIndex) * kBlockSize, yPos, 0.0f);
}


MapChip::Rect MapChip::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) {
	/*Vector3 center = GetMapChipPositionByIndex(xIndex, yIndex);

	Rect rect;
	rect.left = center.x - kBlockWidth / 2.0f;
	rect.right = center.x + kBlockWidth / 2.0f;
	rect.bottom = center.y - kBlockWidth / 2.0f;
	rect.top = center.y + kBlockWidth / 2.0f;

	return rect;*/

	Vector3 bottomLeft = GetMapChipPositionByIndex(xIndex, yIndex);

	Rect rect;
	// 左下の座標を基準に、ブロックの幅と高さを足して右上の座標を計算
	rect.left = bottomLeft.x;
	rect.right = bottomLeft.x + kBlockWidth;
	rect.bottom = bottomLeft.y;
	rect.top = bottomLeft.y + kBlockHeight;

	return rect;

	//Rect rect;
	//// マップの左上を基準に矩形を計算
	//rect.left = static_cast<float>(xIndex) * kBlockSize;
	//rect.right = rect.left + kBlockSize;
	//rect.top = static_cast<float>(yIndex) * kBlockSize;
	//rect.bottom = rect.top + kBlockSize;

	//// Y軸をゲーム世界の座標系に変換
	//rect.top = maxMapSize.y - rect.top;
	//rect.bottom = maxMapSize.y - rect.bottom;

	//// Y座標の`top`と`bottom`を入れ替える
	//std::swap(rect.top, rect.bottom);

	//return rect;
}
void MapChip::ModelCreate() {
	size_t y = 0;
	// マップの全行数
	size_t totalRows = mapChipData.size();

	//for (auto line : mapChipData) {
	//	size_t x = 0;
	//	mapChipTransforms_.push_back(std::vector<WorldTransform*>());
	//	float yPos = (totalRows - 1 - y) * BlockSize;

	//	for (auto cell : line) {
	//		int lineIndex = static_cast<int>(mapChipTransforms_.size()) - 1;

	//		if (cell == MapChipType::Blank) {
	//			mapChipTransforms_[lineIndex].push_back(nullptr);
	//		} else if (cell == MapChipType::Block) {
	//			WorldTransform* worldTransform = new WorldTransform();
	//			worldTransform->Initialize();
	//			// 座標を計算
	//			worldTransform->translation_ = {x * BlockSize, yPos, 0.0f};
	//			worldTransform->scale_ = {1.0f, 1.0f, 1.0f}; // 3Dモデルのスケールも調整
	//			worldTransform->rotation_ = {0.0f, 0.0f, 0.0f};
	//			worldTransform->UpdateMatirx();
	//			mapChipTransforms_[lineIndex].push_back(worldTransform);
	//		}
	//		x++;
	//	}
	//	y++;
	//}

	 for (const auto& line : mapChipData) {
		size_t x = 0;
		mapChipTransforms_.emplace_back(); // vector<unique_ptr<WorldTransform>> を追加

		float yPos = (totalRows - 1 - y) * BlockSize;

		for (auto cell : line) {
			int lineIndex = static_cast<int>(mapChipTransforms_.size()) - 1;

			if (cell == MapChipType::Blank) {
				mapChipTransforms_[lineIndex].push_back(nullptr);
			} else if (cell == MapChipType::Block) {
				auto worldTransform = std::make_unique<WorldTransform>();
				worldTransform->Initialize();
				worldTransform->translation_ = {x * BlockSize -2.0f , yPos + 0.5f, 0.0f};
				worldTransform->scale_ = {1.0f, 1.0f, 1.0f};
				worldTransform->rotation_ = {0.0f, 0.0f, 0.0f};
				worldTransform->UpdateMatirx();
				mapChipTransforms_[lineIndex].push_back(std::move(worldTransform));
			
			}
			x++;
		}
		y++;
	}

	

}
