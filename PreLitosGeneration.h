#pragma once
#include <cmath>
#include <vector>
#include <algorithm>
#include "Basics.h"

bool minimumLenFromBorder(IntPair p, int minX, int maxX, int minY, int maxY) {
	return (p.x > minX && p.y < maxX && p.y > minY && p.y < maxY);
}

class PreMap {
public:
	int x, y;
	int numOfRegions, numOfLitos;
	int** heights;
	float mLen;
	IntPair* centersOfRegions;
	IntPair* centersOfLitos;
	std::vector<IntPair>* regions;
	std::vector<IntPair>* litos;
	int** adjacencyMatrix;

	int seed;

	PreMap(int _x = 512, int _y = 512, int _numOfRegions = 64, int _numOfLitos = 12, int _seed = 0) :
		x(_x), y(_y), numOfRegions(_numOfRegions), numOfLitos(_numOfLitos), seed(_seed) {
		IntPair::w = x;
		regions = new std::vector<IntPair>[numOfRegions];
		litos = new std::vector<IntPair>[numOfLitos];
		centersOfRegions = new IntPair[numOfRegions];
		centersOfLitos = new IntPair[numOfLitos];

		adjacencyMatrix = new int* [numOfRegions];
		for (int i = 0; i < numOfRegions; i++) {
			adjacencyMatrix[i] = new int[numOfRegions];
			for (int j = 0; j < numOfRegions; j++) {
				adjacencyMatrix[i][j] = 0;
			}
		}

		heights = new int* [x];
		for (int i = 0; i < x; i++) {
			heights[i] = new int[y];
			for (int j = 0; j < y; j++) {
				heights[i][j] = -1;
			}
		}

		mLen = y / sqrt(numOfRegions) / 1.25;
	}

	bool inArray(IntPair point, IntPair* arr, int size) {
		for (int i = 0; i < size; i++) {
			if (point == arr[i]) {
				return true;
			}
		}
		return false;
	}

	bool inVector(std::vector<int> vec, int value) {
		for (int i = 0; i < vec.size(); i++) {
			if (vec[i] == value) return true;
		}
		return false;
	}
	bool intersectEachOther(IntPair point, IntPair* arr, int size) {
		for (int i = 0; i < size; i++) {
			if (sqrt((point.x - arr[i].x) * (point.x - arr[i].x)) + ((point.y - arr[i].y) * (point.y * arr[i].y)) <= mLen) {
				return true;
			}
		}
		return false;
	}

	void task(const IntPair& point, std::vector<IntPair>& exp, std::vector<IntPair>& constExp, const int& curHeight, int height) {
		if (point.x >= 0 && point.x < x && point.y >= 0 && point.y < y && heights[point.x][point.y] == curHeight) {
			heights[point.x][point.y] = height;
			exp.push_back(point);
			constExp.push_back(point);
		}
	}

	void generateRegions() {
		RandInt randPX = RandInt(x - 1, 0, seed);
		RandInt randPY = RandInt(y - 1, 0, seed);

		IntPair* pointsOfRegs = new IntPair[numOfRegions];
		for (int i = 0; i < numOfRegions; i++) {
			IntPair tmp;
			do {
				tmp = IntPair(randPX.generate(), randPY.generate());
			} while (intersectEachOther(tmp, pointsOfRegs, i) && !minimumLenFromBorder(tmp, x / 10, x - x / 10,
				y / 10, y - y / 10));

			pointsOfRegs[i] = tmp;
		}

		int stop = 0, step = 1;
		bool* stops = new bool[numOfRegions];
		for (int i = 0; i < numOfRegions; i++) {
			stops[i] = false;
		}

		std::vector<IntPair>* expansion = new std::vector<IntPair>[numOfRegions];
		for (int i = 0; i < numOfRegions; i++) {
			expansion[i].push_back(pointsOfRegs[i]);
			centersOfRegions[i] = pointsOfRegs[i];
		}

		IntPair p;
		while (stop != numOfRegions) {
			for (int i = 0; i < numOfRegions; i++) {
				if (!stops[i]) {
					int s = expansion[i].size();
					for (int j = 0; j < s; j++) {

						p = expansion[i][j];
						expansion[i].erase(expansion[i].begin() + j);
						s--;

						task(IntPair(p.x - 1, p.y), expansion[i], regions[i], -1, i);
						task(IntPair(p.x + 1, p.y), expansion[i], regions[i], -1, i);
						task(IntPair(p.x, p.y - 1), expansion[i], regions[i], -1, i);
						task(IntPair(p.x, p.y + 1), expansion[i], regions[i], -1, i);

						if (step % 2 == 0) {
							task(IntPair(p.x - 1, p.y - 1), expansion[i], regions[i], -1, i);
							task(IntPair(p.x + 1, p.y + 1), expansion[i], regions[i], -1, i);
							task(IntPair(p.x - 1, p.y + 1), expansion[i], regions[i], -1, i);
							task(IntPair(p.x + 1, p.y - 1), expansion[i], regions[i], -1, i);
						}
						else if (expansion[i].size() <= s) {
							task(IntPair(p.x - 1, p.y - 1), expansion[i], regions[i], -1, i);
							task(IntPair(p.x + 1, p.y + 1), expansion[i], regions[i], -1, i);
							task(IntPair(p.x - 1, p.y + 1), expansion[i], regions[i], -1, i);
							task(IntPair(p.x + 1, p.y - 1), expansion[i], regions[i], -1, i);
						}
					}
					if (expansion[i].size() <= s) {
						stop++;
						stops[i] = true;
					}
				}
			}
			step++;
		}
		delete[] stops;
		delete[] expansion;
		delete[] pointsOfRegs;
	}

	int inVector(int p, std::vector<IntPair> vec) {
		for (int i = 0; i < vec.size(); i++) {
			if (vec[i].x == p) {
				return i;
			}
		}
		return -1;
	}

	int checkMaxNearPoints(IntPair p, int size) {
		std::vector<IntPair> vec;
		for (int i = p.x - size; i <= p.x + size; i++) {
			for (int j = p.y - size; j <= p.y + size; j++) {
				if (i >= 0 && i < x && j >= 0 && j < y && heights[i][j] != -1) {
					int c = inVector(heights[i][j], vec);
					if (c != -1) {
						vec[c].y++;
					}
					else {
						vec.push_back(IntPair(heights[i][j], 1));
					}
				}
			}
		}
		int max = 0;
		int index = 0;
		for (auto& i : vec) {
			if (i.y > max) {
				max = i.y;
				index = i.x;
			}
		}
		return index;
	}

	void repairMap() {
		for (int i = 0; i < x; i++) {
			for (int j = 0; j < y; j++) {
				if (heights[i][j] == -1) {
					heights[i][j] = checkMaxNearPoints(IntPair(i, j), 1);
					regions[heights[i][j]].push_back(IntPair(i, j));
				}
			}
		}
	}

	void generateAdjacencyMatrix() {
		for (int j = 0; j < y; j++) {
			for (int i = 0; i < x - 1; i++) {
				if (heights[i][j] != heights[i + 1][j] && heights[i][j] != -1 && heights[i + 1][j] != -1) {
					adjacencyMatrix[heights[i][j]][heights[i + 1][j]]++;
					adjacencyMatrix[heights[i + 1][j]][heights[i][j]]++;
				}
			}
		}
		for (int i = 0; i < x; i++) {
			for (int j = 0; j < y - 1; j++) {
				if (heights[i][j] != heights[i][j + 1] && heights[i][j] != -1 && heights[i][j + 1] != -1) {
					adjacencyMatrix[heights[i][j]][heights[i][j + 1]]++;
					adjacencyMatrix[heights[i][j + 1]][heights[i][j]]++;
				}
			}
		}
	}

	void generateLitos() {
		RandInt randP = RandInt(numOfRegions - 1, 0, seed);
		bool* isUsed = new bool[numOfRegions];

		std::vector<int>* expansion = new std::vector<int>[numOfLitos];
		IntPair* centersOfLitos = new IntPair[numOfLitos];

		int* currElem = new int[numOfLitos];
		for (int i = 0; i < numOfRegions; i++) {
			isUsed[i] = false;
		}

		expansion[0].push_back(heights[0][0]);
		expansion[0].push_back(heights[x - 1][0]);
		expansion[0].push_back(heights[0][y - 1]);
		expansion[0].push_back(heights[x - 1][y - 1]);

		isUsed[heights[0][0]] = true;
		isUsed[heights[x - 1][0]] = true;
		isUsed[heights[0][y - 1]] = true;
		isUsed[heights[x - 1][y - 1]] = true;


		for (int x0 = 0; x0 < x - 1; x0++) {
			if (heights[x0 + 1][0] != heights[x0][0]) {
				expansion[0].push_back(heights[x0 + 1][0]);
				isUsed[heights[x0 + 1][0]] = true;
			}
			if (heights[x0 + 1][y - 1] != heights[x0][y - 1]) {
				expansion[0].push_back(heights[x0 + 1][y - 1]);
				isUsed[heights[x0 + 1][y - 1]] = true;
			}
		}

		for (int y0 = 0; y0 < y - 1; y0++) {
			if (heights[0][y0 + 1] != heights[0][y0]) {
				expansion[0].push_back(heights[0][y0 + 1]);
				isUsed[heights[0][y0 + 1]] = true;
			}
			if (heights[x - 1][y0 + 1] != heights[x - 1][y0]) {
				expansion[0].push_back(heights[x - 1][y0 + 1]);
				isUsed[heights[x - 1][y0 + 1]] = true;
			}
		}

		for (int i = 1; i < numOfLitos; i++) {
			int tmp;
			do {
				tmp = randP.generate();
			} while (isUsed[tmp]);
			expansion[i].push_back(tmp);
			centersOfLitos[i] = centersOfRegions[tmp];
			isUsed[tmp] = true;
			currElem[i] = 0;
		}

		int stop = 0, step = 1;
		bool* stops = new bool[numOfLitos];
		int* currentPoint = new int[numOfLitos];

		stops[0] = true;
		for (int i = 1; i < numOfLitos; i++) {
			stops[i] = false;
			expansion[i].push_back(heights[centersOfLitos[i].x][centersOfLitos[i].y]);
			isUsed[heights[centersOfLitos[i].x][centersOfLitos[i].y]] = true;
			currentPoint[i] = 0;
		}
		while (stop != numOfLitos - 1) {
			for (int i = 1; i < numOfLitos; i++) {
				while (!stops[i]) {
					int s = expansion[i].size();
					int ind1 = expansion[i][currentPoint[i]];
					int max = 0;
					int ind2 = -1;
					for (int j = 0; j < numOfRegions; j++) {
						if (adjacencyMatrix[ind1][j] > max && !isUsed[j]) {
							max = adjacencyMatrix[ind1][j];
							ind2 = j;
						}
					}
					if (ind2 == -1) {
						if (currentPoint[i] == expansion[i].size() - 1) {
							stop++;
							stops[i] = true;
						}
						else {
							currentPoint[i]++;
						}
					}
					else {
						expansion[i].push_back(ind2);
						isUsed[ind2] = true;
						break;
					}
				}
			}
		}
		for (int i = 0; i < numOfLitos; i++) {
			for (int j = 0; j < expansion[i].size(); j++) {
				for (int k = 0; k < regions[expansion[i][j]].size(); k++) {
					litos[i].push_back(regions[expansion[i][j]][k]);
				}
			}
		}
	}

	~PreMap() {
		for (int i = 0; i < x; i++)				delete[] heights[i];
		delete[] heights;
		for (int i = 0; i < numOfRegions; i++)	delete adjacencyMatrix[i];
		delete[] adjacencyMatrix;
		delete[] centersOfLitos;
		delete[] centersOfRegions;
		delete[] regions;
		delete[] litos;
	}
};
