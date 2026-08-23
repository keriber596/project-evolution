#pragma once
#include <vector>
#include <cmath>
#include "new_heights.h"

//class RandInt {
//	int max_num, min_num, rand_int, seed;
//public:
//	RandInt(int _max = 0, int _min = 0, int _seed = 0) :max_num(_max), min_num(_min), rand_int(1), seed(_seed) {};
//	int generate() {
//		rand_int = (rand() + seed) % max_num + 1;
//		if (rand_int < min_num) {
//			return generate();
//		}
//		return rand_int;
//	}
//	double uniform(double a, double b) {
//		return a + generate() * (b - a) / max_num;
//	}
//};

class IntPair {
public:
	int x, y;
	IntPair(int x = -1, int y = -1) : x(x), y(y) {};
};

int** oceanSmoothing(int x, int y, int** heights, int oceanLevel, int levelOfSmooth) {
	int** NewHeights = new int* [x];
	for (int i = 0; i < x; i++) {
		NewHeights[i] = new int[y];
		for (int j = 0; j < y; j++) {
			NewHeights[i][j] = heights[i][j];
		}
	}
	std::vector<IntPair> oceanExt;
	std::vector<IntPair> oceanLast;
	for (int x0 = 0; x0 < x; x0++) {
		for (int y0 = 0; y0 < x; y0++) {
			if (NewHeights[x0][y0] == oceanLevel) {
				oceanLast.push_back(IntPair(x0, y0));
			}
		}
	}
	int lastOceanLevel = oceanLevel;
	for (int i = 1; i < levelOfSmooth; i++) {
		oceanLevel = oceanLevel + i;
		for (int x0 = 0; x0 < x; x0++) {
			for (int y0 = 0; y0 < x; y0++) {
				if (NewHeights[x0][y0] == oceanLevel) {
					oceanExt.push_back(IntPair(x0, y0));
				}
			}
		}
		for (int j = 0; j < oceanExt.size(); j++) {
			NewHeights[oceanExt[j].x][oceanExt[j].y] -= 1;
		}
		for (int j = 0; j < oceanLast.size(); j++) {
			if (i % 2 == 0)
				NewHeights[oceanLast[j].x][oceanLast[j].y] += 1;
		}
		oceanLast.clear();
		oceanLast = oceanExt;
		oceanExt.clear();
	}
	return NewHeights;
}

int** elution(int x, int y, int** heights, int oceanLevel, int levelOfElution, bool** isMild) {
	int** NewHeights = new int* [x];
	for (int i = 0; i < x; i++) {
		NewHeights[i] = new int[y];
		for (int j = 0; j < y; j++) {
			NewHeights[i][j] = heights[i][j];
		}
	}

	std::vector<IntPair> proccessElution;
	for (int x0 = 0; x0 < x; x0++) {
		for (int y0 = 0; y0 < y; y0++) {
			if (NewHeights[x0][y0] > oceanLevel && NewHeights[x0][y0] <= (oceanLevel + levelOfElution)) {
				proccessElution.push_back(IntPair(x0, y0));
			}
		}
	}

	int level = oceanLevel + levelOfElution;
	while (level != oceanLevel - levelOfElution) {
		for (int i = 0; i < proccessElution.size(); i++) {
			int x0 = proccessElution[i].x, y0 = proccessElution[i].y;
			if (isMild[x0][y0]) {
				bool flag = false;
				for (int x1 = -1 + x0; x1 <= x0 + 1; x1++) {
					for (int y1 = -1 + y0; y1 <= y0 + 1; y1++) {
						if (x0 == x1 && y0 == y1) {
							continue;
						}
						if (x1 >= 0 && x1 < x && y1 >= 0 && y1 < y && NewHeights[x1][y1] > level) {
							flag = true;
						}
					}
				}
				if (!flag) {
					NewHeights[x0][y0]--;
				}
			}


		}
		level--;
	}
	return NewHeights;
}

void northenProccessOfColdElution(IntPair p, int borderX, int y, int** heights, bool** isMild, int power, int oceanLevel, int levelOfElution) {
	std::vector<IntPair> closePoints;

	if (p.x - 1 >= 0 && p.x - 1 < borderX && p.y >= 0 && p.y < y && isMild[p.x - 1][p.y]) {
		closePoints.push_back(IntPair(p.x - 1, p.y));
	}
	if (p.x + 1 >= 0 && p.x + 1 < borderX && p.y >= 0 && p.y < y && isMild[p.x + 1][p.y]) {
		closePoints.push_back(IntPair(p.x + 1, p.y));
	}
	if (p.x >= 0 && p.x < borderX && p.y - 1 >= 0 && p.y - 1 < y && isMild[p.x][p.y - 1]) {
		closePoints.push_back(IntPair(p.x, p.y - 1));
	}
	if (p.x >= 0 && p.x < borderX && p.y + 1 >= 0 && p.y + 1 < y && isMild[p.x][p.y + 1]) {
		closePoints.push_back(IntPair(p.x, p.y + 1));
	}

	bool** isUsed = new bool* [borderX];
	for (int i = 0; i < borderX; i++) {
		isUsed[i] = new bool[y];
		for (int j = 0; j < y; j++) {
			isUsed[i][j] = false;
		}
	}

	std::vector<IntPair> points;
	while (power != 0 && !closePoints.empty()) {
		int min = heights[closePoints[0].x][closePoints[0].y];
		IntPair cord = closePoints[0];
		int ind = 0;
		for (int i = 0; i < closePoints.size(); i++) {
			if (heights[closePoints[i].x][closePoints[i].y] > min) {
				cord = closePoints[i];
				min = heights[closePoints[i].x][closePoints[i].y];
				ind = i;
			}
		}
		closePoints.erase(closePoints.begin() + ind);
		heights[cord.x][cord.y] = oceanLevel;
		isUsed[cord.x][cord.y] = true;

		int x0 = cord.x, y0 = cord.y;
		bool flag = false;
		for (int x1 = -1 + x0; x1 <= x0 + 1; x1++) {
			for (int y1 = -1 + y0; y1 <= y0 + 1; y1++) {
				if (x0 == x1 && y0 == y1) {
					continue;
				}
				if (x1 >= 0 && x1 < borderX && y1 >= 0 && y1 < y && heights[x1][y1] > oceanLevel) {
					flag = true;
				}
			}
		}
		if (!flag) {
			heights[cord.x][cord.y]--;
		}

		if (x0 - 1 >= 0 && x0 - 1 < borderX && y0 >= 0 && y0 < y && isMild[x0 - 1][y0] && !isUsed[x0 - 1][y0]) {
			closePoints.push_back(IntPair(x0 - 1, y0));
		}
		if (x0 + 1 >= 0 && x0 + 1 < borderX && y0 >= 0 && y0 < y && isMild[x0 + 1][y0] && !isUsed[x0 + 1][y0]) {
			closePoints.push_back(IntPair(x0 + 1, y0));
		}
		if (x0 >= 0 && x0 < borderX && y0 - 1 >= 0 && y0 - 1 < y && isMild[x0][y0 - 1] && !isUsed[x0][y0 - 1]) {
			closePoints.push_back(IntPair(x0, y0 - 1));
		}
		if (x0 >= 0 && x0 < borderX && y0 + 1 >= 0 && y0 + 1 < y && isMild[x0][y0 + 1] && !isUsed[x0][y0 + 1]) {
			closePoints.push_back(IntPair(x0, y0 + 1));
		}

		power--;
	}

	for (int i = 0; i < borderX; i++) {
		delete[] isUsed[i];
	}
	delete[] isUsed;
}

void southenProccessOfColdElution(IntPair p, int x, int borderX, int y, int** heights, bool** isMild, int power, int oceanLevel, int levelOfElution) {
	std::vector<IntPair> closePoints;

	if (p.x - 1 >= x - borderX && p.x - 1 < x && p.y >= 0 && p.y < y && isMild[p.x - 1][p.y]) {
		closePoints.push_back(IntPair(p.x - 1, p.y));
	}
	if (p.x + 1 >= x - borderX && p.x + 1 < x && p.y >= 0 && p.y < y && isMild[p.x + 1][p.y]) {
		closePoints.push_back(IntPair(p.x + 1, p.y));
	}
	if (p.x >= x - borderX && p.x < x && p.y - 1 >= 0 && p.y - 1 < y && isMild[p.x][p.y - 1]) {
		closePoints.push_back(IntPair(p.x, p.y - 1));
	}
	if (p.x >= x - borderX && p.x < x && p.y + 1 >= 0 && p.y + 1 < y && isMild[p.x][p.y + 1]) {
		closePoints.push_back(IntPair(p.x, p.y + 1));
	}

	bool** isUsed = new bool* [x];
	for (int i = x - borderX; i < x; i++) {
		isUsed[i] = new bool[y];
		for (int j = 0; j < y; j++) {
			isUsed[i][j] = false;
		}
	}

	std::vector<IntPair> points;
	while (power != 0 && !closePoints.empty()) {
		int min = heights[closePoints[0].x][closePoints[0].y];
		IntPair cord = closePoints[0];
		int ind = 0;
		for (int i = 0; i < closePoints.size(); i++) {
			if (heights[closePoints[i].x][closePoints[i].y] > min) {
				cord = closePoints[i];
				min = heights[closePoints[i].x][closePoints[i].y];
				ind = i;
			}
		}
		closePoints.erase(closePoints.begin() + ind);
		heights[cord.x][cord.y] = oceanLevel;
		isUsed[cord.x][cord.y] = true;

		int x0 = cord.x, y0 = cord.y;
		bool flag = false;
		for (int x1 = -1 + x0; x1 <= x0 + 1; x1++) {
			for (int y1 = -1 + y0; y1 <= y0 + 1; y1++) {
				if (x0 == x1 && y0 == y1) {
					continue;
				}
				if (x1 >= x - borderX && x1 < x && y1 >= 0 && y1 < y && heights[x1][y1] > oceanLevel) {
					flag = true;
				}
			}
		}
		if (!flag) {
			heights[cord.x][cord.y]--;
		}

		if (x0 - 1 >= x - borderX && x0 - 1 < x && y0 >= 0 && y0 < y && isMild[x0 - 1][y0] && !isUsed[x0 - 1][y0]) {
			closePoints.push_back(IntPair(x0 - 1, y0));
		}
		if (x0 + 1 >= x - borderX && x0 + 1 < x && y0 >= 0 && y0 < y && isMild[x0 + 1][y0] && !isUsed[x0 + 1][y0]) {
			closePoints.push_back(IntPair(x0 + 1, y0));
		}
		if (x0 >= x - borderX && x0 < x && y0 - 1 >= 0 && y0 - 1 < y && isMild[x0][y0 - 1] && !isUsed[x0][y0 - 1]) {
			closePoints.push_back(IntPair(x0, y0 - 1));
		}
		if (x0 >= x - borderX && x0 < x && y0 + 1 >= 0 && y0 + 1 < y && isMild[x0][y0 + 1] && !isUsed[x0][y0 + 1]) {
			closePoints.push_back(IntPair(x0, y0 + 1));
		}

		power--;
	}

	for (int i = x - borderX; i < x; i++) {
		delete[] isUsed[i];
	}

	delete[] isUsed;
}

int** coldElution(int x, int y, int** heights, int oceanLevel, int northenBorder, int southenBorder, int levelOfElution, bool** isMild, int seed) {

	std::vector<IntPair> southenElution;
	std::vector<IntPair> northenElution;

	for (int x0 = 1; x0 < northenBorder - 1; x0++) {
		for (int y0 = 1; y0 < y - 1; y0++) {
			if ((heights[x0 - 1][y0] <= oceanLevel || heights[x0 + 1][y0] <= oceanLevel
				|| heights[x0][y0 - 1] <= oceanLevel || heights[x0][y0 + 1] <= oceanLevel)
				&& isMild[x0][y0] && heights[x0][y0] < oceanLevel + levelOfElution) {
				northenElution.push_back(IntPair(x0, y0));
			}
		}
	}
	for (int x0 = southenBorder + 1; x0 < x - 1; x0++) {
		for (int y0 = 1; y0 < y - 1; y0++) {
			if ((heights[x0 - 1][y0] <= oceanLevel || heights[x0 + 1][y0] <= oceanLevel
				|| heights[x0][y0 - 1] <= oceanLevel || heights[x0][y0 + 1] <= oceanLevel)
				&& isMild[x0][y0] && heights[x0][y0] < oceanLevel + levelOfElution) {
				southenElution.push_back(IntPair(x0, y0));
			}
		}
	}

	int numOfNorthenElution = int(sqrt(northenBorder * northenBorder + y * y) / (x)) % northenElution.size();
	int numOfSouthenElution = int(sqrt(southenBorder * southenBorder + y * y) / (x)) % southenElution.size();
	bool* isNorthenUsed = new bool[northenElution.size()];
	for (int i = 0; i < northenElution.size(); i++) {
		isNorthenUsed[i] = false;
	}
	bool* isSouthenUsed = new bool[southenElution.size()];
	for (int i = 0; i < southenElution.size(); i++) {
		isSouthenUsed[i] = false;
	}

	IntPair* northenPoints = new IntPair[numOfNorthenElution];
	IntPair* southenPoints = new IntPair[numOfSouthenElution];

	RandInt randNP = RandInt(northenElution.size() - 1, 0, seed);
	RandInt randSP = RandInt(southenElution.size() - 1, 0, seed);
	RandInt randPower = RandInt(numOfNorthenElution + numOfSouthenElution + levelOfElution, levelOfElution, seed);


	for (int i = 0; i < numOfNorthenElution; i++) {
		int index = 0;
		do {
			index = randNP.generate();
		} while (isNorthenUsed[index]);
		northenPoints[i] = northenElution[index];
		isNorthenUsed[index] = true;
	}
	for (int i = 0; i < numOfSouthenElution; i++) {
		int index = 0;
		do {
			index = randSP.generate();
		} while (isSouthenUsed[index]);
		southenPoints[i] = southenElution[index];
		isSouthenUsed[index] = true;
	}

	int* nPowerOfElution = new int[numOfNorthenElution];
	int* sPowerOfElution = new int[numOfSouthenElution];

	for (int i = 0; i < numOfNorthenElution; i++) {
		nPowerOfElution[i] = randPower.generate();
	}
	for (int i = 0; i < numOfSouthenElution; i++) {
		sPowerOfElution[i] = randPower.generate();
	}

	int** NewHeights = new int* [x];
	for (int i = 0; i < x; i++) {
		NewHeights[i] = new int[y];
		for (int j = 0; j < y; j++) {
			NewHeights[i][j] = heights[i][j];
		}
	}

	for (int i = 0; i < numOfNorthenElution; i++) {
		northenProccessOfColdElution(northenPoints[i], northenBorder, y, NewHeights, isMild, nPowerOfElution[i], oceanLevel, levelOfElution);
	}
	for (int i = 0; i < numOfSouthenElution; i++) {
		southenProccessOfColdElution(southenPoints[i], x, southenBorder, y, NewHeights, isMild, sPowerOfElution[i], oceanLevel, levelOfElution);
	}

	delete[] isNorthenUsed;
	delete[] isSouthenUsed;
	delete[] nPowerOfElution;
	delete[] sPowerOfElution;

	return NewHeights;
}