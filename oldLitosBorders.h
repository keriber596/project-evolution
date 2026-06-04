#pragma once
#include "Basics.h"
#include "Litosplate.h"

enum BTypes {
	Divide, Plain, Highland, Mountain
};

class LitosBorder {
	void smooth(IntPair point, int radius, int x, int y, int** heights) {
		int count = 0, sumHeight = 0;
		for (int i = point.x - radius; i <= point.x + radius; i++) {
			for (int j = point.y - radius; j <= point.y + radius; j++) {
				if (i >= 0 && i < x && j >= 0 && j < y) {
					count++;
					sumHeight += heights[i][j];
				}
			}
		}
		heights[point.x][point.y] = sumHeight / count;
	}
public:
	int shiftHeight;
	int randMax; 
	int randMin;

	std::vector<IntPair> points;
	BTypes type;

	LitosBorder(
		int shiftHeight,
		int randMax,
		int randMin,
		bool isExp,
		LType first,
		LType second,
		std::vector<IntPair> points) : shiftHeight(shiftHeight), randMax(randMax), randMin(randMin),
		points(points) {
		int t = abs(first - second);
		if (isExp) {
			if (t > 1) {
				type = Mountain;
			}
			else {
				type = Highland;
			}
		}
		else {
			if (t == 0) {
				type = Plain;
			}
			else {
				type = Divide;
			}
		}

	}

	LitosBorder(
		int shiftHeight,
		int randMax,
		int randMin,
		std::vector<IntPair> points,
		BTypes type) : shiftHeight(shiftHeight), randMax(randMax), randMin(randMin),
		points(points), type(type) {
	}

	void generate(int x, int y, int** heights, bool**& isSolid, bool** isUsed, int seed) {
		if (type == BTypes::Divide) {
			RandInt randShift = RandInt(randMax, randMin, seed);
			RandInt coef = RandInt(2, 1, seed);
			for (auto& p : points) {
				if (!isUsed[p.x][p.y]) {
					heights[p.x][p.y] -= shiftHeight;
					if (coef.generate() % 2 == 0) {
						heights[p.x][p.y] += randShift.generate();
					}
					else {
						heights[p.x][p.y] -= randShift.generate();
					}
					isSolid[p.x][p.y] = true;
					isUsed[p.x][p.y] = true;
				}
				
			}
			for (auto& p : points) {
				smooth(p, 3, x, y, heights);
			}	
		}

		else if (type == BTypes::Plain) {
			RandInt randShift = RandInt(randMax, randMin, seed);
			RandInt coef = RandInt(2, 1, seed);
			for (auto& p : points) {
				if (!isUsed[p.x][p.y]) {
					if (coef.generate() % 2 == 0) {
						heights[p.x][p.y] += randShift.generate();
					}
					else {
						heights[p.x][p.y] -= randShift.generate();
					}
					isSolid[p.x][p.y] = true;
					isUsed[p.x][p.y] = true;
				}
			}
			for (auto& p : points) {
				smooth(p, 5, x, y, heights);
			}
		}

		else if (type == BTypes::Highland) {
			RandInt randShift = RandInt(randMax, randMin, seed);
			RandInt coef = RandInt(2, 1, seed);
			for (auto& p : points) {
				if (!isUsed[p.x][p.y]) {
					heights[p.x][p.y] += shiftHeight / 2;
					if (coef.generate() % 2 == 0) {
						heights[p.x][p.y] += 2 * randShift.generate();
					}
					else {
						heights[p.x][p.y] -= 2 * randShift.generate();
					}
					isSolid[p.x][p.y] = true;
					isUsed[p.x][p.y] = true;
				}
			}
			for (auto& p : points) {
				smooth(p, 3, x, y, heights);
			}
		}

		else {
			RandInt randShift = RandInt(randMax, randMin, seed);
			RandInt coef = RandInt(2, 1, seed);
			for (auto& p : points) {
				if (!isUsed[p.x][p.y]) {
					heights[p.x][p.y] += shiftHeight;
					if (coef.generate() % 2 == 0) {
						heights[p.x][p.y] += randShift.generate();
					}
					else {
						heights[p.x][p.y] -= randShift.generate();
					}
					isSolid[p.x][p.y] = true;
					isUsed[p.x][p.y] = true;
				}
			}
			for (auto& p : points) {
				smooth(p, 2, x, y, heights);
			}
		}		
	}
};

int inVector(int p, std::vector<IntPair> vec) {
	for (int i = 0; i < vec.size(); i++) {
		if (vec[i].x == p) {
			return i;
		}
	}
	return -1;
}

bool pairInVector(IntPair p, std::vector<IntPair> vec) {
	for (auto i : vec) {
		if (i == p) return true;
	}
	return false;
}

IntPair defineCloseVecForMap(std::vector<IntPair> vec, int x, int y, int** map, int radius) {
	IntPair min = vec[0];
	IntPair max = vec[0];

	int sumX = 0, sumY = 0;
	for (auto p : vec) {
		sumX += p.x;
		sumY += p.y;
	}

	IntPair center = IntPair(sumX / vec.size(), sumY / vec.size());
	std::vector<IntPair> vecOfNums;

	for (int i = center.x - radius; i <= center.x + radius; i++) {
		for (int j = center.y - radius; j <= center.y + radius; j++) {
			if (i >= 0 && i < x && j >= 0 && j < y && map[i][j] != -2) {
				int c = inVector(map[i][j], vecOfNums);
				if (c != -1) {
					vecOfNums[c].y++;
				}
				else {
					vecOfNums.push_back(IntPair(map[i][j], 1));
				}
			}
		}
	}

	if (vecOfNums.size() == 0) {
		return IntPair(-1, 3);
	}

	max = vecOfNums[0];
	for (auto p : vecOfNums) {
		if (p.y > max.y) {
			max = p;
		}
	}

	int index = max.x, c = max.y;

	if (c < 0.5 * vec.size()) {
		return IntPair(index, 2);
	}
	else if (c < 0.3 * vec.size()) {
		return IntPair(index, 1);
	}
	else {
		return IntPair(index, 0);
	}
}

std::vector<IntPair> mergeVectors(std::vector<IntPair> v1, std::vector<IntPair> v2) {
	std::vector<IntPair> merged;
	for (int i = 0; i < v1.size(); i++) 
		merged.push_back(v1[i]);
	for (int i = 0; i < v2.size(); i++)
		if (!pairInVector(v2[i], merged))
			merged.push_back(v2[i]);
	return merged;
	
}

std::vector<LitosBorder> defineLitosBorders(LitosPlate* litos, int size, int x, int y, int** expMap, int** leavMap, int maxShift, int minShift, int seed) {
	RandInt randH = RandInt(maxShift, minShift, seed);
	std::vector<LitosBorder> borders;

	bool* notForUse = new bool[size];
	for (int i = 0; i < size; i++) {
		notForUse[i] = false;
	}

	IntPair tmp;
	for (int i = 1; i < size; i++) {
		if (litos[i].leaving.size() == 0) {
			continue;
		}
		tmp = defineCloseVecForMap(litos[i].leaving, x, y, leavMap, 30);
		if (tmp.x == -1) 
			borders.push_back(LitosBorder(randH.generate(), 10, 5, litos[i].leaving, BTypes::Divide));
		else {
			if (tmp.y == 2) {
				borders.push_back(LitosBorder(randH.generate(), 10, 5, false, litos[i].type, litos[tmp.x].type, litos[i].leaving));
			}
			else if (tmp.y == 1) {
				std::vector<IntPair> merged = mergeVectors(litos[i].leaving, litos[tmp.x].expansion);
				notForUse[tmp.x] = true;
				borders.push_back(LitosBorder(randH.generate(), 10, 5, false, litos[i].type, litos[tmp.x].type, merged));
			}
		}
		
	}

	for (int i = 1; i < size; i++) {
		if (!notForUse[i]) {
			if (litos[i].expansion.size() == 0) {
				continue;
			}
			tmp = defineCloseVecForMap(litos[i].expansion, x, y, expMap, 30);
			if (tmp.x == -1)
				borders.push_back(LitosBorder(randH.generate(), 10, 5, litos[i].expansion, BTypes::Mountain));
			else {
				if (tmp.y == 2) {
					std::vector<IntPair> merged = mergeVectors(litos[i].expansion, litos[tmp.x].leaving);
					borders.push_back(LitosBorder(randH.generate(), 10, 5, true, litos[i].type, litos[tmp.x].type, merged));
				}
				else if (tmp.y == 1) {
					borders.push_back(LitosBorder(randH.generate(), 10, 5, true, litos[i].type, litos[tmp.x].type, litos[i].expansion));
				}
			}
		}
	}

	delete[] notForUse;

	return borders;
}

void generateLitosBorders(std::vector<LitosBorder> borders, int x, int y, int**& heights, bool**& isSolid, int seed) {
	bool** isUsed = new bool* [x];
	for (int i = 0; i < x; i++) {
		isUsed[i] = new bool[y];
		for (int j = 0; j < y; j++) {
			isUsed[i][j] = false;
		}
	}
	for (int i = 0; i < borders.size(); i++) {
		borders[i].generate(x, y, heights, isSolid, isUsed, seed);
	}
	for (int i = 0; i < x; i++) {
		delete[] isUsed[i];
	}
	delete[] isUsed;
}
