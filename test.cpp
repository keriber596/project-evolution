
#include <iostream>
#include <cmath>
#include "TempGen.h"

#define PI 3.141562

float toRadians(float angle) {
	return angle * PI / 180;
}

float toGrads(float radian) {
	return 180 * radian / PI;
}
int main() {
	std::ifstream file("dataSpec1.txt");
	int** heights = new int*[1024];
	if (file.is_open()) {
		for (int i = 0; i < 1024; i++) {
			heights[i] = new int[640];
			for (int j = 0; j < 640; j++) {
				file >> heights[i][j];
			}
		}
	}
	TempGen map = TempGen(1024, 640, 600, 100, 120, 20, 6173, 100, 12, heights);

}