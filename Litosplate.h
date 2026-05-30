#pragma once
#include <set>

bool intersectAttractors(IntPair point, IntPair* arr, int size, int mLen) {
	for (int i = 0; i < size; i++) {
		if (sqrt((point.x - arr[i].x) * (point.x - arr[i].x)) + ((point.y - arr[i].y )*(point.y * arr[i].y)) <= mLen) {
			return true;
		}
	}
	return false;
}


IntPair randCenterOfAttraction(int maxX, int minX, int maxY, int minY, int seed) {
	RandInt randX = RandInt(maxX, minX, seed);
	RandInt randY = RandInt(maxY, minY, seed);
	return IntPair(randX.generate(), randY.generate());
}

IntPair generateExpVector(const IntPair& attractor, const IntPair& center, float diagonal, float coef) {
	std::cout << "( " << attractor.x << ", " << attractor.y << " )" << std::endl;
	std::cout << attractor.x << " - " << center.x << " = " << attractor.x - center.x << std::endl;
	std::cout << attractor.y << " - " << center.y << " = " << attractor.y - center.y << std::endl;
	std::cout << "( " << abs(attractor.x - center.x) * coef / diagonal << ", "
		<< abs(attractor.y - center.y) * coef / diagonal << " )" << '\n';
	std::cout << "\n";

	return IntPair((attractor.x - center.x) * coef / diagonal, (attractor.y - center.y) * coef / diagonal);
}

float lenBetweenPoints(const IntPair& a, const IntPair& b) {
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

class LitosPlate {
public:
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

	LitosPlate(std::vector<IntPair> _points = std::vector<IntPair>(1),
		int x = 1,
		int y = 1,
		int n = 0,
		IntPair* attractors = nullptr,
		int seed = 1) {
		points = _points;

		long long sumX = 0, sumY = 0;
		for (int i = 0; i < points.size(); i++) {
			sumX += points[i].x; sumY += points[i].y;
		}
		center = IntPair(sumX / points.size(), sumY / points.size());
		
		int min = sqrt(x * x + y * y);
		int ind = 0;
		for (int i = 0; i < n; i++) {
			if (lenBetweenPoints(center, attractors[i]) < min) {
				min = lenBetweenPoints(center, attractors[i]);
				ind = i;
			}
		}

		if (attractors != nullptr) expVector = generateExpVector(attractors[ind], center, sqrt(x * x + y * y), 100);


		std::set<IntPair> beforeShift, afterShift;
		for (int i = 0; i < points.size(); i++) {
			beforeShift.insert(points[i]);
			if ((points[i].x + expVector.x) >= 0 && (points[i].x + expVector.x) < x &&
				(points[i].y + expVector.y) >= 0 && (points[i].y + expVector.y) < y)
				afterShift.insert(IntPair(points[i].x + expVector.x, points[i].y + expVector.y));

		}

		for (auto& p : beforeShift)
			if (afterShift.count(p) == 0)
				leaving.push_back(p);

		for (auto& p : afterShift)
			if (beforeShift.count(p) == 0)
				expansion.push_back(p);
	}

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

	void basicHeight(int standardHeight, int x, int y, int** heights) {
		for (int i = 0; i < points.size(); i++) {
			heights[points[i].x][points[i].y] = standardHeight;
		}
	}

	void basicSmooth(int x, int y, int** heights, int power) {
		for (int i = 0; i < points.size(); i++) {
			smooth(points[i], power, x, y, heights);
		}
	}
	void basicSmooth(int x, int y, int** heights, int power, bool** isSolid) {
		for (int i = 0; i < points.size(); i++) {
			if (!isSolid[points[i].x][points[i].y])
				smooth(points[i], power, x, y, heights);
			else
				smooth(points[i], power / 1.5, x, y, heights);
		}
	}


	void basicHeightWithShiftForMountains(int x, int y, int** heights, int shiftHeight, int randMax, int randMin,
		bool** isSolid, int seed) {
		RandInt randShift = RandInt(randMax, randMin, seed);
		RandInt coef = RandInt(1, 0, seed);
		for (auto& p: expansion) {
			heights[p.x][p.y] += shiftHeight;
			if (coef.generate() % 2 == 0) {
				heights[p.x][p.y] += randShift.generate();
			}
			else {
				heights[p.x][p.y] -= randShift.generate();
			}
			isSolid[p.x][p.y] = true;
		}
		for (auto& p : expansion) {
			smooth(p, 3, x, y, heights);
		}
	}

	void basicHeightWithShiftForDivide(int x, int y, int** heights, int shiftHeight, int randMax, int randMin,
		bool** isSolid, int seed) {
		RandInt randShift = RandInt(randMax, randMin, seed);
		RandInt coef = RandInt(1, 0, seed);
		for (auto& p : leaving) {
			heights[p.x][p.y] -= shiftHeight;
			if (coef.generate() % 2 == 0) {
				heights[p.x][p.y] += randShift.generate();
			}
			else {
				heights[p.x][p.y] -= randShift.generate();
			}
			isSolid[p.x][p.y] = true;

		}
		for (auto& p : leaving) {
			smooth(p, 3, x, y, heights);
		}
	}
};

void generateHeightMap(int n, LitosPlate* plates, int x, int y, int** heights, int seed) {
	RandInt rH = RandInt(128 + 20, 128 - 20, seed);
	RandInt rHForDivide = RandInt(100, 60, seed);
	RandInt rHForMountains = RandInt(120, 60, seed);

	bool** isSolid = new bool* [x];
	for (int i = 0; i < x; i++) {
		isSolid[i] = new bool[y];
		for (int j = 0; j < y; j++) {
			isSolid[i][j] = false;
		}
	}

	plates[0].basicHeight(128 - 40, x, y, heights);
	for (int i = 1; i < n; i++) {
		plates[i].basicHeight(rH.generate(), x, y, heights);
	}
	for (int i = 0; i < n; i++) {
		plates[i].basicSmooth(x, y, heights, sqrt(x * x + y * y) / 64);
	}
	for (int i = 1; i < n; i++) {
		plates[i].basicHeightWithShiftForMountains(x, y, heights, rHForMountains.generate(), 20, 5, isSolid, seed);
		plates[i].basicHeightWithShiftForDivide(x, y, heights, rHForDivide.generate(), 10, 5, isSolid, seed);
	}
	for (int i = 0; i < n; i++) {
		plates[i].basicSmooth(x, y, heights, sqrt(x * x + y * y) / 64, isSolid);
	}
	/*for (int i = 0; i < n; i++) {
		for (auto& j: plates[i].expansion) {
			heights[j.x][j.y] = 255;
		}
		for (auto& j : plates[i].leaving) {
			heights[j.x][j.y] = 0;
		}
	}*/
}
