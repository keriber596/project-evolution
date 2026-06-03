#pragma once
#include <vector>
#include <iostream>
#include <set>
#include <queue>
#include <fstream>
#include "Basics.h"

const int M		= 200;
const int H		= 170;
const int HP	= 146;
const int P		= 128;
const int S		= 90;
const int O		= 60;

const int hLevels[6] = {
	200, 170, 146, 128, 90, 60
};

//class HeightZone {
//public:
//	std::set<IntPair> borders;
//	std::set<IntPair> points;
//	float avgHeight;
//	HeightZone(float _avgHeight) :avgHeight(_avgHeight) {};
//
//	void addPoint(IntPair p, int x, int y) {
//		points.insert(p);
//		if (borders.count(p) != 0) {
//			borders.erase(p);
//		}
//		for (int i = -1; i <= 1; i++)
//			for (int j = -1; j <= i; j++)
//				if ((p.x + i < x) && (p.x + i >= 1) && (p.y + j < y) && (p.y + j >= 0) && (!(i == 0) && (j == 0)))
//					borders.insert(IntPair(p.x + i, p.y + j));
//	}
//};
//
//class ZonesMap {
//private:
//	void fillHeights(int** heights) {
//		for (int i = 0; i < oldX; i++) {
//			for (int j = 0; j < oldY; j++) {
//				bool flag = false;
//				for(int k=0; k<numHeights-1; k++)
//					if (heights[i][j] < heightBorders[k]) {
//						allHeights[k].push_back(IntPair(i, j));
//						flag = true;
//						break;
//					}
//				if (!flag) allHeights[numHeights-1].push_back(IntPair(i, j));
//			}
//		}
//	}
//	bool inArray(IntPair* arr, IntPair value, int size) {
//		for (int i = 0; i < size; i++)
//			if (arr[i] == value)
//				return true;
//		return false;
//	}
//public:
//	std::vector<HeightZone>* zones;
//	std::vector<IntPair>* allHeights;
//	bool** used;
//	int** newHeights, * heightBorders; // lenght - numHeights-1
//	int numHeights, numAreas, power, oldX, oldY, newX, newY; // numAreas must divide at numHeights 
//	int seed;
//	ZonesMap(int _numHeights, int _numAreas, int _power, int _oldX, int _oldY, int _newX, int _newY, int* _heightBorders, int _seed) : 
//		numHeights(_numHeights), numAreas(_numAreas), zones(), seed(_seed),
//		oldX(_oldX), oldY(_oldY), newX(_newX), newY(_newY), power(_power), heightBorders(_heightBorders){
//		used = new bool*[oldX];
//		for (int i = 0; i < oldX; i++) {
//			used[i] = new bool[oldY];
//			for (int j = 0; j < newY; j++) used[i][j] = false;
//		}
//		newHeights = new int* [newX];
//		for (int i = 0; i < newX; i++) {
//			newHeights[i] = new int[newY];
//			for (int j = 0; j < newY; j++) newHeights[i][j] = 0;
//		}
//		zones = new std::vector<HeightZone>[numHeights];
//		allHeights = new std::vector<IntPair>[numHeights];
//	}
//	void generateAreas(int** heights) {
//		IntPair** centers = new IntPair*[numHeights];
//		RandInt randNum(oldX * oldY, 0, seed);
//		for (int i = 0; i < numHeights; i++) {
//			centers[i] = new IntPair[numAreas / numHeights];
//			for (int j = 0;j < numAreas / numHeights; j++) {
//				IntPair tmp;
//				do {
//					tmp = allHeights[i][randNum.generate()%allHeights[i].size()];
//				} while (inArray(centers[i], tmp, j));
//				centers[i][j] = tmp;
//				used[tmp.x][tmp.y] = true;
//			}
//		}
//		int* powers = new int[numAreas];
//		for (int i = 0; i < numHeights; i++) {
//			for (int j = 0;j < numAreas / numHeights; j++) {
//
//			}
//		}
//	}
//
//
//
//};

class PostLitosGen {

	void addToQueue(const Point& p, std::priority_queue<Point>& queue, bool**& isUsed, int avHeight) {
		for (int i = p.x - 1; i <= p.x + 1; i++) {
			for (int j = p.y - 1; j <= p.y + 1; j++) {
				if (i >= 0 && i < x && j >= 0 && j < y && !isUsed[i][j] && heights[i][j] >= hLevels[5]) {
					queue.push(Point(i, j, -abs(heights[i][j] - avHeight)));
				}
			}
		}
	}

	int addToVec(std::vector<IntPair>& vec, std::priority_queue<Point>& queue, int power, bool**& isUsed, long int& sum, int index) {
		Point tmp;
		int count = 0;
		while (!queue.empty() && power >= 0) {
			tmp = queue.top();
			power -= abs(tmp.height);
			if (power >= 0) {
				queue.pop();
				isUsed[tmp.x][tmp.y] = true;

				sum += heights[tmp.x][tmp.y];

				vec.push_back(IntPair(tmp.x, tmp.y));
				addToQueue(tmp, queue, isUsed, sum / vec.size());

				regionMap[tmp.x][tmp.y] = index;
				count++;
			}
			else {
				break;
			}
		}
		return count;
	}

	bool isBorder(IntPair p, int size) {
		int level = regionMap[p.x][p.y];
		for (int i = p.x - size; i <= p.x + size; i++) {
			for (int j = p.y - size; j <= p.y + size; j++) {
				if (i >= 0 && i < x && j >= 0 && j < y && level != regionMap[i][j]) {
					return true;
				}
			}
		}
		return false;
	}

	int defineLevel(IntPair p) {
		for (int i = 0; i < 6; i++) {
			if (heights[p.x][p.y] > hLevels[i]) {
				return i;
			}
		}
		return -1;
	}

	void genRegs() {

		regs = new std::vector<IntPair>[numOfRegs];
		std::vector<IntPair>* vLevels = new std::vector<IntPair>[6];
		int voidPoints = 0;
		for (int i = 0; i < x; i++) {
			for (int j = 0; j < y; j++) {
				int tmp = defineLevel(IntPair(i, j));
				if (tmp != -1) {
					vLevels[tmp].push_back(IntPair(i, j));
				}
				else {
					voidPoints++;
				}
			}
		}

		int power = 255;
		bool** isUsed = new bool* [x];
		for (int i = 0; i < x; i++) {
			isUsed[i] = new bool[y];
			for (int j = 0; j < y; j++) {
				isUsed[i][j] = false;
			}
		}

		int* numOfPoints = new int[6];
		RandInt* randP = new RandInt[6];
		int newNumOfRegs = 0;
		for (int i = 0; i < 6; i++) {
			numOfPoints[i] = long double(numOfRegs * (vLevels[i].size() * 1000 / (x * y - voidPoints))) / 1000;
			newNumOfRegs += numOfPoints[i];
			randP[i] = RandInt(vLevels[i].size() - 1, 0, seed);
		}
		numOfRegs = newNumOfRegs;

		int index = 0;
		for (int i = 0; i < 6; i++) {
			for (int j = 0; j < numOfPoints[i]; j++) {
				int tmp;
				do {
					tmp = randP[i].generate();
				} while (isUsed[vLevels[i][tmp].x][vLevels[i][tmp].y]);
				regs[index].push_back(vLevels[i][tmp]);
				isUsed[vLevels[i][tmp].x][vLevels[i][tmp].y] = true;
				regionMap[vLevels[i][tmp].x][vLevels[i][tmp].y] = index;
				index++;
			}
		}
		delete[] numOfPoints;
		delete[] randP;


		std::priority_queue<Point>* queues = new std::priority_queue<Point>[numOfRegs];
		long int* sums = new long int[numOfRegs];
		for (int i = 0; i < numOfRegs; i++) {
			int x = regs[i][0].x, y = regs[i][0].y;
			sums[i] += heights[x][y];
			addToQueue(Point(x, y, sums[i]), queues[i], isUsed, sums[i]);
		}

		int stops = 0;
		bool* stop = new bool[numOfRegs];
		for (int i = 0; i < numOfRegs; i++) {
			stop[i] = false;
		}
		int tmp;
		while (stops != numOfRegs) {
			for (int i = 0; i < numOfRegs; i++) {
				if (!stop[i]) {
					tmp = addToVec(regs[i], queues[i], power, isUsed, sums[i], i);
					if (tmp == 0) {
						stop[i] = true;
						stops++;
					}
				}
			}
		}

		delete[] sums;
		delete[] stop;
		for (int i = 0; i < x; i++) {
			delete[] isUsed[i];
		}
		delete[] isUsed;
	}

	void defineBorders(int radius) {
		for (int i = 0; i < x; i++) {
			for (int j = 0; j < y; j++) {
				if (isBorder(IntPair(i, j), radius)) {
					borders.push_back(IntPair(i, j));
				}
			}
		}
	}
public:
	int x, y;
	int nX, nY;

	int** heights;
	int** regionMap;
	int numOfRegs;
	std::vector<IntPair>* regs;
	std::vector<IntPair> borders;
	int** nHeights;

	int seed;

	PostLitosGen(int _x, int _y, int** _heights, int _scale, int _numOfRegs, int _seed) :
		x(_x), y(_y), nX(x * _scale), nY(y * _scale), numOfRegs(_numOfRegs), seed(_seed)
	{
		heights = new int* [x];
		for (int i = 0; i < x; i++) {
			heights[i] = new int[y];
			for (int j = 0; j < y; j++) {
				heights[i][j] = _heights[i][j];
			}
		}

		regionMap = new int* [x];
		for (int i = 0; i < x; i++) {
			regionMap[i] = new int[y];
			for (int j = 0; j < y; j++) {
				regionMap[i][j] = -1;
			}
		}

		genRegs();
		//defineBorders(3);

		for (int i = 0; i < numOfRegs; i++) {
			for (int j = 0; j < regs[i].size(); j++) {
				heights[regs[i][j].x][regs[i][j].y] = (i * 10) % 255;
			}
		}

		for (int i = 0; i < borders.size(); i++) {
			heights[borders[i].x][borders[i].y] = 255;
		}
		std::ofstream ofs("C:/Users/gf/PycharmProjects/PythonProject/postData.txt");
		for (int i = 0; i < x; i++) {
			for (int j = 0; j < y; j++)
				ofs << regionMap[i][j] << ' ';
			ofs << '\n';
		}
		ofs.close();


	}
};
