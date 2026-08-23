#pragma once
#include "Litosplate.h"
#include "LitosBorders.h"

void generateHeightMap(int n, LitosPlate* plates, int x, int y, int** heights, int** expMap, int** leavMap, int seed) {
	RandInt rH = RandInt(128 + 25, 128 - 35, seed);

	bool** isSolid = new bool* [x];
	for (int i = 0; i < x; i++) {
		isSolid[i] = new bool[y];
		for (int j = 0; j < y; j++) {
			isSolid[i][j] = false;
		}
	}

	plates[0].basicHeight(128 - 50, x, y, heights);
	for (int i = 1; i < n; i++) {
		plates[i].basicHeight(rH.generate(), x, y, heights);
	}
	for (int i = 0; i < n; i++) {
		plates[i].basicSmooth(x, y, heights, sqrt(x * x + y * y) / 64);
	}

	std::string str = "C:/Users/gf/PycharmProjects/PythonProject/dataSpec2.txt";
	std::ofstream file1(str);
	if (file1.is_open()) {
		for (int i = 0; i < x; i++) {
			for (int j = 0; j < y; j++) {
				file1 << heights[i][j] << " ";
			}
			file1 << "\n";
		}
		file1.close();
	}

	std::vector<LitosBorder> borders = defineLitosBorders(plates, n, x, y, expMap, leavMap, 95, 60, seed);
	generateLitosBorders(borders, x, y, heights, isSolid, seed);

	for (int i = 0; i < n; i++) {
		plates[i].basicSmooth(x, y, heights, sqrt(x * x + y * y) / 128, isSolid);
	}

	/*for (int i = 0; i < n; i++) {
		for (auto& j : plates[i].expansion) {
			heights[j.x][j.y] = 255;
		}
		for (auto& j : plates[i].leaving) {
			heights[j.x][j.y] = 0;
		}
	}*/
}