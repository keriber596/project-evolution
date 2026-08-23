#pragma once
#include <vector>
#include <cmath>



class RandInt {
	int max_num, min_num, rand_int, seed;
public:
	RandInt(int _max = 0, int _min = 0, int _seed = 0) :max_num(_max), min_num(_min), rand_int(1), seed(_seed) {};
	int generate() {
		rand_int = (rand() + seed) % max_num + 1;
		if (rand_int < min_num) {
			return generate();
		}
		return rand_int;
	}
	double uniform(double a, double b) {
		return a + generate() * (b - a) / max_num;
	}
};

class IntPair {
public:
	int x, y;
	IntPair(int x = -1, int y = -1) : x(x), y(y) {};

	friend bool operator == (const IntPair& a, const IntPair& b) {
		return a.x == b.x && a.y == b.y;
	}
	friend bool operator != (const IntPair& a, const IntPair& b) {
		return !(a == b);
	}

};


class Point {
public:
	int x, y, height;
	Point(int x = 0.0, int y = 0.0, int height = 0.0) : x(x), y(y), height(height) {};
	Point(const Point& a) {
		x = a.x;
		y = a.y;
		height = a.height;
	}
	Point operator = (const Point& a) {
		x = a.x;
		y = a.y;
		height = a.height;
		return *this;
	}
	friend bool operator == (const Point& a, const Point& b) {
		return a.x == b.x && a.y == b.y && a.height == b.height;
	}
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

	int numOfNorthenElution = int(sqrt(northenBorder * northenBorder + y * y) / (x - (x / 20 * levelOfElution))) % northenElution.size();
	int numOfSouthenElution = int(sqrt(southenBorder * southenBorder + y * y) / (x - (x / 20 * levelOfElution))) % southenElution.size();
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

bool isOceanMountains(int x, int y, int** heights, IntPair dot1, IntPair dot2, int oceanLevel, int levelOfSmooth) {
	int count = 0;
	int oceanCount = 0;
	for (int x0 = dot1.x; x0 <= dot2.x; x0++) {
		for (int y0 = dot1.y; y0 < dot2.y; y0++) {
			if (x0 >= 0 && x0 < x && y0 >= 0 && y0 < y) {
				count++;
				if (heights[x0][y0] <= oceanLevel) {
					oceanCount++;
				}
			}
		}
	}

	return (oceanCount + levelOfSmooth >= count / 2);
}

void proccessOfSmoothOceanMountains(int x, int y, int** heights, IntPair dot, int oceanLevel, int levelOfSmooth) {
	IntPair dot1 = IntPair(dot.x - levelOfSmooth / 2, dot.y - levelOfSmooth / 2);
	IntPair dot2 = IntPair(dot.x + levelOfSmooth / 2, dot.y + levelOfSmooth / 2);

	std::vector<IntPair> rockyBank;
	for (int x0 = dot1.x; x0 < dot2.x; x0++) {
		for (int y0 = dot1.y; y0 < dot2.x; y0++) {
			if (x0 >= 0 && x0 < x && y0 >= 0 && y0 < y) {
				bool flag = false;
				for (int x1 = -1 + x0; x1 <= x0 + 1; x1++) {
					for (int y1 = -1 + y0; y1 <= y0 + 1; y1++) {
						if (x0 == x1 && y0 == y1) {
							continue;
						}
						if ((x1 < 0 || x1 >= x) && (y1 < 0 || y1 >= y) || (x1 >= 0 && x1 < x) && (y1 >= 0 && y1 < y) && (heights[x1][y1] > oceanLevel)) {
							flag = true;
						}
					}
				}
				if (!flag) {
					rockyBank.push_back(IntPair(x0, y0));
				}
			}
		}
	}
	int power = levelOfSmooth;
	while (power != 0 && !rockyBank.empty()) {
		int min = heights[rockyBank[0].x][rockyBank[0].y];
		IntPair cord = rockyBank[0];
		int ind = 0;
		for (int i = 0; i < rockyBank.size(); i++) {
			if (heights[rockyBank[i].x][rockyBank[i].y] > min) {
				cord = rockyBank[i];
				min = heights[rockyBank[i].x][rockyBank[i].y];
				ind = i;
			}
		}
		rockyBank.erase(rockyBank.begin() + ind);
		heights[cord.x][cord.y] = oceanLevel;

		int x0 = cord.x, y0 = cord.y;
		bool flag = false;
		for (int x1 = -1 + x0; x1 <= x0 + 1; x1++) {
			for (int y1 = -1 + y0; y1 <= y0 + 1; y1++) {
				if (x0 == x1 && y0 == y1) {
					continue;
				}
				if (x1 >= 0 && x1 < x && y1 >= 0 && y1 < y && heights[x1][y1] > oceanLevel) {
					flag = true;
				}
			}
		}
		if (!flag) {
			heights[cord.x][cord.y]--;
		}

		if (x0 - 1 >= 0 && x0 - 1 < x && y0 >= 0 && y0 < y) {
			rockyBank.push_back(IntPair(x0 - 1, y0));
		}
		if (x0 + 1 >= 0 && x0 + 1 < x && y0 >= 0 && y0 < y) {
			rockyBank.push_back(IntPair(x0 + 1, y0));
		}
		if (x0 >= 0 && x0 < x && y0 - 1 >= 0 && y0 - 1 < y) {
			rockyBank.push_back(IntPair(x0, y0 - 1));
		}
		if (x0 >= 0 && x0 < x && y0 + 1 >= 0 && y0 + 1 < y) {
			rockyBank.push_back(IntPair(x0, y0 + 1));
		}

		power--;
	}


}

int** smoothOceanMountains(int x, int y, int** heights, int oceanLevel, int levelOfSmooth) {
	int** newHeights = new int* [x];
	for (int i = 0; i < x; i++) {
		newHeights[i] = new int[y];
		for (int j = 0; j < y; j++) {
			newHeights[i][j] = heights[i][j];
		}
	}

	std::vector<IntPair> oceanMountains;

	for (int x0 = 0; x0 < x; x0 += levelOfSmooth / 2) {
		for (int y0 = 0; y0 < y; y0 += levelOfSmooth / 2) {
			IntPair dot1 = IntPair(x0 - levelOfSmooth / 2, y0 - levelOfSmooth / 2);
			IntPair dot2 = IntPair(x0 + levelOfSmooth / 2, y0 + levelOfSmooth / 2);
			if ((((dot1.x < 0 || dot1.x >= x) && (dot1.y < 0 || dot1.y >= y)) || ((dot1.x >= 0 && dot1.x < x) && (dot1.y >= 0 && dot1.y < y)) && heights[dot1.x][dot1.y] <= oceanLevel) &&
				(((dot2.x < 0 || dot2.x >= x) && (dot2.y < 0 || dot2.y >= y)) || ((dot2.x >= 0 && dot2.x < x) && (dot2.y >= 0 && dot2.y < y)) && heights[dot2.x][dot2.y] <= oceanLevel)) {
				if (isOceanMountains(x, y, heights, dot1, dot2, oceanLevel, levelOfSmooth)) {
					oceanMountains.push_back(IntPair(x0, y0));
				}
			}
		}
	}

	for (int i = 0; i < oceanMountains.size(); i++) {
		proccessOfSmoothOceanMountains(x, y, newHeights, oceanMountains[i], oceanLevel, levelOfSmooth);
	}

	return newHeights;
}

IntPair randExpVector(int power, int seed) {
	RandInt rand = RandInt(2, 0, seed);

	return IntPair((rand.generate() - 1) * power, (rand.generate() - 1) * power);
}

int randPower(int max, int min, int seed) {
	RandInt rand = RandInt(max, min, seed);
	return rand.generate();
}


std::vector<IntPair>* pointsFromHeights(int** heights, int x, int y, int numOfTiles) {
	// высоты - это номера плит!!!
	std::vector<IntPair>* tiles = new std::vector<IntPair>[numOfTiles];
	for (int i = 0; i < x; i++)
		for (int j = 0; j < y; j++) tiles[heights[i][j]].push_back(IntPair(i, j));
	return tiles;
}


class LitosPlate {
public:
	int standartHeight;
	int power;
	IntPair expVector;

	IntPair center;
	std::vector<IntPair> points;
	std::vector<IntPair> expansion;
	std::vector<IntPair> leaving;

	bool inVector(IntPair point, std::vector<IntPair> vec) {
		for (int i = 0; i < vec.size(); i++) {
			if (point == vec[i]) {
				return true;
			}
		}
		return false;
	}
	bool inArray(IntPair point, IntPair* arr, int size) {
		for (int i = 0; i < size; i++) {
			if (point == arr[i]) {
				return true;
			}
		}
		return false;
	}

	LitosPlate(std::vector<IntPair> _points, int _standartHeight, int x, int y, int maxPower, int minPower, int seed) {
		points = _points;
		standartHeight = _standartHeight;

		power = randPower(maxPower, minPower, seed);
		expVector = randExpVector(power, seed);


		int sumX = 0, sumY = 0;
		for (int i = 0; i < points.size(); i++) {
			sumX += points[i].x; sumY += points[i].y;
			IntPair newDot = IntPair(points[i].x + expVector.x, points[i].y + expVector.y);
			if (!inVector(newDot, points) && newDot.x >= 0 && newDot.x < x && newDot.y >= 0 && newDot.y < y) {
				expansion.push_back(newDot);
			}
		}

		center = IntPair(sumX / points.size(), sumY / points.size());

		for (int i = 0; i < points.size(); i++) {
			if (!inVector(points[i], expansion)) {
				leaving.push_back(points[i]);
			}
		}
	}

	void doGenerationOfSharpnesses(IntPair point, int power, int coef, int x, int y, int** heights) {
		int basicPower = power;
		std::vector<IntPair> expansion;
		expansion.push_back(point);
		IntPair p;
		int h;
		int s;

		if (coef % 2 == 0) {
			coef = 1;
		}
		else {
			coef = -1;
		}

		while (power != 0 && !expansion.empty()) {
			s = expansion.size();
			for (int i = 0; i < s; i++) {
				p = expansion[i];
				heights[p.x][p.y] += coef * power;
				expansion.erase(expansion.begin() + i);
				s--;

				if (p.x - 1 >= 0 && p.x - 1 < x && p.y >= 0 && p.y < y) {
					expansion.push_back(IntPair(p.x - 1, p.y));
				}
				if (p.x + 1 >= 0 && p.x + 1 < x && p.y >= 0 && p.y < y) {
					expansion.push_back(IntPair(p.x + 1, p.y));
				}
				if (p.x >= 0 && p.x < x && p.y - 1 >= 0 && p.y - 1 < y) {
					expansion.push_back(IntPair(p.x, p.y - 1));
				}
				if (p.x >= 0 && p.x < x && p.y + 1 >= 0 && p.y + 1 < y) {
					expansion.push_back(IntPair(p.x, p.y + 1));
				}
			}
			power--;
		}
	}

	void generateLitosSharpnesses(int number, int minPower, int maxPower, int x, int y, int** heights, int seed) {
		RandInt randPower = RandInt(maxPower, minPower, seed);
		RandInt randPoints = RandInt(points.size() - 1, 0, seed);
		RandInt randCoef = RandInt(1, 0, seed);

		IntPair* arrayOfPoints = new IntPair[number];
		for (int i = 0; i < number; i++) {
			IntPair tmp;
			do {
				tmp = randPoints.generate();
			} while (inArray(tmp, arrayOfPoints, i));
			arrayOfPoints[i] = tmp;
		}

		for (int i = 0; i < number; i++) {
			doGenerationOfSharpnesses(arrayOfPoints[i], randPower.generate(), randCoef.generate(), x, y, heights);
		}
	}
};


void createSharpnessWithLitosPlates(int n, LitosPlate* plates, int x, int y, int** heights, int seed) {
	for (int i = 0; i < n; i++) {
		plates[i].generateLitosSharpnesses(3, 5, 20, x, y, heights, seed);
	}
}