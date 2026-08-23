#pragma once
#include <vector>
#include <iostream>
#include <map>
#include <cmath>
#include <set>
#include "new_heights.h"

int pow_num(int a, int d) {
	for (int i = 0; i < d; i++)
		a *= a;
	return a;
}

class FPoint {
public:
	static inline int w;
	int x, y;
	FPoint() :x(0), y(0) {};
	FPoint(int _x, int _y) :x(_x), y(_y) {};
	
	bool operator< (const FPoint& other) const {
		return (x + y * w) < (other.x + other.y * other.w);
	}
	bool operator> (const FPoint& other) const {
		return (x + y * w) > (other.x + other.y * other.w);
	}
	bool operator== (const FPoint& other) const  {
		return (x + y * w) == (other.x + other.y * other.w);
	}
};

class LitosPlate {
public:
	int height, weight;
	FPoint direct;
	std::vector<FPoint> plate_cells, down_shift, up_shift;
	LitosPlate() :height(0), weight(0), direct(FPoint()) {};
	LitosPlate(int _height, int _weight, FPoint _direct) :height(_height), weight(_weight), direct(_direct) {};

	void calc_shifts(int x, int y);
};

std::map<int, LitosPlate> fill_litos(int** heights, int x, int y, int tiles) {
	FPoint::w = x;
	std::map<int, LitosPlate> plates;
	RandInt rand_h(185, 37, 1039);
	plates[heights[0][0]].height = heights[0][0];
	for (int i = 0; i < x; i++)
		for (int j = 0; j < y; j++) plates[heights[i][j]].plate_cells.push_back(FPoint(i, j));

	int cnt = tiles-1;
	for (auto& [i, p] : plates) {
		p.weight = cnt;
		if (p.height == heights[0][0]) {
			p.direct = FPoint(0, 0);
		}
		else {
			p.height = rand_h.generate();
			p.direct = FPoint((rand() + cnt * tiles) % (tiles)*pow_num(-1, (rand() + cnt * tiles) % (tiles)),
				(rand() - cnt * tiles) % (tiles)*pow_num(-1, (rand() + cnt * tiles) % (tiles)));

		}
		cnt--;
	}

	/*for (auto p:plates) {
		std::cout << p.first << std::endl;
		for (int j = 0; j < p.second.plate_cells.size(); j++) {
			std::cout << p.second.plate_cells[j].x<< " "<< p.second.plate_cells[j].y << "    ";
		}
		std::cout << std::endl;
	}*/
	return plates;
}

void LitosPlate::calc_shifts(int x, int y) {
	std::multiset<FPoint> before_shift, after_shift;
	if (direct.x == 0 && direct.y == 0) return;
	for (int i = 0; i < plate_cells.size(); i++) {
		before_shift.insert(plate_cells[i]);
		if((plate_cells[i].x + direct.x < x)&& (plate_cells[i].x + direct.x >= 0) && (plate_cells[i].y + direct.y < y) && (plate_cells[i].y + direct.y >=0))
		after_shift.insert(FPoint(plate_cells[i].x + direct.x, plate_cells[i].y + direct.y));
	}
	for (auto& p : before_shift) 
		if (after_shift.count(p) == 0) down_shift.push_back(p);

	for (auto& p : after_shift)
		if (before_shift.count(p) == 0) up_shift.push_back(p);
}

int** heights_with_shifts(int** height, std::map<int, LitosPlate> plates, int x, int y, int tiles) {
	int** new_heights = new int* [x];

	for (int i = 0; i < x; i++) {
		new_heights[i] = new int[y]; 
		for (int j = 0; j < y; j++)
			new_heights[i][j] = 0;
	}
	for (auto& [i, p] : plates) {
		for (FPoint up_p : p.up_shift) {
			if (new_heights[up_p.x][up_p.y] > 0) {
				new_heights[up_p.x][up_p.y] += (tiles - p.weight)/2;
			}
			else
				new_heights[up_p.x][up_p.y] += (tiles - p.weight);
			
		}
		for (FPoint down_p : p.down_shift) {
			new_heights[down_p.x][down_p.y] -= p.weight;
		}

	}
	/*for (int i = 0; i < x; i++)
		for (int j = 0; j < y; j++) new_heights[i][j] += height[i][j];
	*/


	return new_heights;
}

void task(int**& heights, int**& newHeights, int x, int y, int x0, int y0, int standart, int added, std::vector<Point>& ext) {
	if (x0 >= 0 && x0 < x && y0 >= 0 && y0 < y) {
		if (heights[x0][y0] == standart && newHeights[x0][y0] == -1) {
			newHeights[x0][y0] = added;
			ext.push_back(Point(x0, y0, heights[x0][y0]));
		}
	}
}

int** generateLitospheros(int x, int y, int** heights, int basicLevel, int numOfLitos, Point* points) {
	int* arrayOfHeights = new int[numOfLitos];
	std::vector<Point>* ext = new std::vector<Point>[numOfLitos];
	for (int i = 0; i < numOfLitos; i++) {
		if (i < numOfLitos / 2) {
			arrayOfHeights[i] = 128 - 4 * i;
		}
		else {
			arrayOfHeights[i] = 128 + 4 * i;
		}
		ext[i].push_back(points[i]);
	}

	int** newHeights = new int* [x];
	for (int i = 0; i < x; i++) {
		newHeights[i] = new int[y];
		for (int j = 0; j < y; j++) {
			newHeights[i][j] = -1;
		}
	}

	int stop = 0, step = 1;
	bool* stops = new bool[numOfLitos];
	for (int i = 0; i < numOfLitos; i++) {
		stops[i] = false;
	}

	while (stop != numOfLitos) {
		for (int i = 0; i < numOfLitos; i++) {

			if (!stops[i]) {
				bool flag = false;
				int s = ext[i].size();
				Point p;
				bool visited_step = false;
				for (int j = 0; j < s; j++) {
					p = ext[i][j];
					ext[i].erase(ext[i].begin() + j);
					s--;
					if (step % 32 == 0 && !visited_step) {
						if (i < numOfLitos / 2) {
							arrayOfHeights[i] += 1;
						}
						else {
							arrayOfHeights[i] -= 1;
						}
						visited_step = true;
					}
					else if (step % 32 != 0) {
						visited_step = false;
					}
					task(heights, newHeights, x, y, p.x - 1, p.y, points[i].height, arrayOfHeights[i], ext[i]);
					task(heights, newHeights, x, y, p.x + 1, p.y, points[i].height, arrayOfHeights[i], ext[i]);
					task(heights, newHeights, x, y, p.x, p.y - 1, points[i].height, arrayOfHeights[i], ext[i]);
					task(heights, newHeights, x, y, p.x, p.y + 1, points[i].height, arrayOfHeights[i], ext[i]);
					if (step % 2 == 0) {
						task(heights, newHeights, x, y, p.x - 1, p.y + 1, points[i].height, arrayOfHeights[i], ext[i]);
						task(heights, newHeights, x, y, p.x + 1, p.y - 1, points[i].height, arrayOfHeights[i], ext[i]);
						task(heights, newHeights, x, y, p.x - 1, p.y - 1, points[i].height, arrayOfHeights[i], ext[i]);
						task(heights, newHeights, x, y, p.x + 1, p.y + 1, points[i].height, arrayOfHeights[i], ext[i]);
					}
					if (ext[i].size() > s) {
						flag = true;
					}
				}

				if (!flag) {
					stop++;
					stops[i] = true;
				}
			}

		}
		step++;
	}
	std::cout << step;
	return newHeights;

}

