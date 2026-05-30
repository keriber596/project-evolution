#pragma once
#include <cmath>
#include <iostream> 
#include <fstream>

#define PI 3.141562


class TempGen {
public:
	int x, y;
	float cut;
	float aRadius;
	float bRadius;
	float earthAngle;
	float earthRadius;
	float basicSunPower;
	int div;
	float* sunPowerBySpaceTraverse;
	float* angleByTraverse;
	int** temp;

	float radius(float x, float y) {
		return sqrt(x * x + y * y);
	}
	float toRadians(float angle) {
		return angle * PI / 180;
	}

	float toGrads(float radian) {
		return 180 * radian / PI;
	}

	TempGen(int x = 512, int y = 512, float cut = 5,
		float aRadius = 100, float bRadius = 80,
		float earthAngle = 20, float earthRadius = 20, float basicSunPower = 10,
		int div = 12, int** heights = nullptr
	) : x(x), y(y), aRadius(aRadius), bRadius(bRadius), earthAngle(earthAngle),
		earthRadius(earthRadius), basicSunPower(basicSunPower), div(div) {

		float* angle = new float[12];
		float* divX = new float[div];
		float* divY = new float[div];

		for (int i = 0; i < div; i++) {
			angle[i] = i * 360 / div;
		}
		for (int i = 0; i < div; i++) {
			divX[i] = aRadius * cos(angle[i]);
		}
		for (int i = 0; i < div; i++) {
			divY[i] = -bRadius * sin(angle[i]);
		}
		delete[] angle;

		
		float* radiuses = new float[div];
		float max = 0;
		for (int i = 0; i < div; i++) {
			radiuses[i] = radius(divX[i], divY[i]);
			if (radiuses[i] > max) max = radiuses[i];
		}

		sunPowerBySpaceTraverse = new float[div];
		for (int i = 0; i < div; i++) {
			sunPowerBySpaceTraverse[i] = basicSunPower * (1 - radiuses[i] / max);  
		}

		angleByTraverse = new float[div];
		for (int i = 0; i < div; i++) {
			float x1 = tan(toRadians(earthAngle));
			double cosA = (x1 * divX[i]) /
				(sqrt(x1 * x1 + 1 * 1) * sqrt(divX[i] * divX[i] + divY[i] * divY[i]));
			angleByTraverse[i] = toGrads(acos(cosA));
		}
		delete[] divX;
		delete[] divY;


		divX = new float[y];
		divY = new float[y];
		float* dotEarthAngles = new float[y];
		for (int i = 0; i < y; i++) {
			divX[i] = -earthRadius + cut + (2 * (earthRadius - cut) / y) * i;
			divY[i] = sqrt(earthRadius * earthRadius - divX[i] * divX[i]);
			float tg = -divX[i] / sqrt(earthRadius * earthRadius - divX[i] * divX[i]);
			float angle = toGrads(atan(tg));
			dotEarthAngles[i] = 90 - angle;
		}

		float** dotSeasonsAngles = new float* [div];
		for (int i = 0; i < div;i++) {
			dotSeasonsAngles[i] = new float[y];
			float angle = 90 - angleByTraverse[i];
			for (int j = 0; j < y; j++) {
				dotSeasonsAngles[i][j] = dotEarthAngles[j] + angle;
			}
		}



		temp = new int* [x];
		for (int i = 0; i < x; i++) {
			temp[i] = new int[y];
		}

		for (int i = 0; i < y; i++) {
			for (int j = 0; j < x; j++) {
				temp[j][i] = ((basicSunPower * (1 - (abs(90 - dotEarthAngles[i]) / 90)) * (1 - (128 - heights[j][i]) / 128)) / basicSunPower) * 100;
			}
		}

		std::ofstream file("C:/Users/gf/PycharmProjects/PythonProject/dataTemp.txt");
		

		if (file.is_open()) {
			for (int i = 0; i < x; i++) {
				for (int j = 0; j < y; j++) {
					file << temp[i][j] << " ";
				}
				file << "\n";
			}
		}
	}
};