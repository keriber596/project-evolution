#include <iostream>
#include <fstream>

#include "PreLitosGeneration.h"
#include "Litosplate.h"
#include <string>

int main() {
	int seed = 21; // 83 check 
	int x = 1024;
	int y = 640;
	int lits = 16;

	PreMap map = PreMap(x, y, 1024, lits, seed);
	map.generateRegions();
	map.repairMap();
	map.generateAdjacencyMatrix();
	map.generateLitos();
	
	int n = 3;
	IntPair* attractors = new IntPair[n];
	int mLen = y / sqrt(n) / 1.25;
	for (int i = 0; i < n; i++) {
		IntPair tmp;
		do {
			tmp = randCenterOfAttraction(x - x / 4, x / 4, y - y / 4, y / 4, seed);
		} while (intersectAttractors(tmp, attractors, i, mLen));
		attractors[i] = tmp;
	}

	LitosPlate* litos = new LitosPlate[lits];
	for (int i = 0; i < lits; i++) {
		litos[i] = LitosPlate(map.litos[i], x, y, n, attractors, seed);
	}
	generateHeightMap(lits, litos, x, y, map.heights, seed);
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

}