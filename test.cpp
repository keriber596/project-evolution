#include <iostream>
#include <fstream>
#include "PreLitosGeneration.h"
#include "Litosplate.h"
#include "BasicHeightMap.h"
#include "PostLitosGeneration.h"
#include <string>

int main() {
	int seed = 90; // 83 check 
	int x = 1024;
	int y = 640;
	int lits = 13;

	PreMap map = PreMap(x, y, 1024, lits, seed);
	map.generateRegions();
	map.repairMap();
	map.generateAdjacencyMatrix();
	map.generateLitos();
	
	int n = 2;
	IntPair* attractors = new IntPair[n];
	int mLen = y / sqrt(n) / 1.25;
	for (int i = 0; i < n; i++) {
		IntPair tmp;
		do {
			tmp = randCenterOfAttraction(x - x / 4, x / 4, y - y / 4, y / 4, seed);
		} while (intersectAttractors(tmp, attractors, i, mLen));
		attractors[i] = tmp;
	}

	int** expMap = new int* [x];
	for (int i = 0; i < x; i++) {
		expMap[i] = new int[y];
		for (int j = 0; j < y; j++) {
			expMap[i][j] = -2;
		}
	}
	int** leavMap = new int* [x];
	for (int i = 0; i < x; i++) {
		leavMap[i] = new int[y];
		for (int j = 0; j < y; j++) {
			leavMap[i][j] = -2;
		}
	}


	LitosPlate* litos = new LitosPlate[lits];
	for (int i = 0; i < lits; i++) {
		litos[i] = LitosPlate(map.litos[i], x, y, n, attractors, expMap, leavMap, i, seed);
	}


	generateHeightMap(lits, litos, x, y, map.heights, expMap, leavMap, seed);
	std::string str = "C:/Users/gf/PycharmProjects/PythonProject/dataSpec1.txt";
	std::ofstream file1(str);
	if (file1.is_open()) {
		for (int i = 0; i < x; i++) {
			for (int j = 0; j < y; j++) {
				file1 << map.heights[i][j] << " ";
			}
			file1 << "\n";
		}
		file1.close();
	}
	PostLitosGen(x, y, map.heights, 1, 64, seed);
}