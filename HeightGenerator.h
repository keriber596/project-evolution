#pragma once

#include <ctime>
#include <random>
#include <cmath>
#include <map>
#include <iostream>

const int ocean = 40;
const int sea = 80;
const int plain = 160;
const int mountain = 250;

class RandInt {
	int max_num, min_num, rand_int, seed;
public:
	RandInt(int _max, int _min, int _seed) :max_num(_max), min_num(_min), rand_int(1), seed(_seed) {};
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

bool intersect(Point a, Point b, double len) {
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y)) <= len;
}

bool notIntersect(Point a, int n, Point* arr, double len) {
	bool flag = true;
	for (int i = 0; i < n; i++) {
		if (intersect(a, arr[i], len)) {
			flag = false;
			break;
		}
	}
	return flag;
}

class HeightMap {
private:
	//standart small functions
	void task(int x0, int y0, std::vector<Point>& ext, double height) {
		if (x0 >= 0 && x0 < x && y0 >= 0 && y0 < y) {
			if (data[x0][y0] == -1) {
				data[x0][y0] = height;
				ext.push_back(data[x0][y0]);

			}
		}
	}
	void toStandart(Point& begin, double height) {
		if (begin.height == height) return;

		std::vector<Point> extension;
		int standart = begin.height, rd = 1;
		begin.height = height;
		while (true) {
			bool flag = false;
			for (int i = -rd + begin.x; i < rd + 1 + begin.x; i++)
				for (int j = -rd + begin.y; j < rd + 1 + begin.y; j++)
					if (i >= 0 && i < x && j >= 0 && j < y) 
						if (data[i][j] == standart) {
							data[i][j] = height;
							flag = true;
						}
			if (!flag)  break;
			rd++;
		}
	}
	std::map<int, int> calc_amounts(int i, int j) {
		std::map<int, int> amounts;
		for (int i1 = -1; i1 <= 1; i1++) {
			for (int j1 = -1; j1 <= 1; j1++) {
				if (amounts.find(heights[i + i1][j + j1]) != amounts.end())  amounts[heights[i + i1][j + j1]]++;
				else amounts[heights[i + i1][j + j1]] = 1;
			}
		}
		return amounts;
	}
	int  common_value(int i, int j, int k) {
		int summ = 0, cnt = 0;
		for (int i1 = -k; i1 <= k; i1++) 
			for (int j1 = -k; j1 <= k; j1++) {
				summ += heights[i + i1][j + j1];
				cnt++;
			}
		return int(summ / cnt);
	}
	// generation of base of the map 
	void generate_points(RandInt& randX, RandInt& randY, double& mLen, bool*& stops, std::vector<Point>*& extension) {
		//fill data with empty points
		for (int i = 0; i < x; i++) {
			data[i] = new int[y];
			for (int j = 0; j < y; j++) {
				data[i][j] = -1;
			}
		}
		//generate new random points
		for (int i = 0; i < counter; i++) {
			Point tmp;
			do {
				tmp = Point(randX.generate(), randY.generate(), i);
			} while (!notIntersect(tmp, counter, points, mLen));
			points[i] = tmp;
			data[points[i].x][points[i].y] = i;
			stops[i] = false;
			extension[i].push_back(points[i]);
		}
	}
	void fill_small_points(bool*& stops, std::vector<Point>*& extension) {
		int rd = 1, stop = 0;

		while (stop != counter) {
			for (int i = 0; i < counter; i++) {
				if (!stops[i]) {
					bool flag = false;
					Point p;
					int s = extension[i].size();
					for (int j = 0; j < s; j++) {
						p = extension[i][j];

						extension[i].erase(extension[i].begin() + j);
						s--;

						task(p.x - 1, p.y, extension[i], p.height);
						task(p.x + 1, p.y, extension[i], p.height);
						task(p.x, p.y - 1, extension[i], p.height);
						task(p.x, p.y + 1, extension[i], p.height);

						if (rd % 2 == 0) {
							task(p.x - 1, p.y + 1, extension[i], p.height);
							task(p.x + 1, p.y - 1, extension[i], p.height);
							task(p.x - 1, p.y - 1, extension[i], p.height);
							task(p.x + 1, p.y + 1, extension[i], p.height);
						}
						if (extension[i].size() > s) flag = true;
					}
					if (!flag) {
						stops[i] = true;
						stop++;
					}
				}
			}
			rd++;
		}
	}
	//merging small tiles into big plates
	void adjacency_matrix(int** matrix) {
		for (int i = 0; i < counter; i++) {
			matrix[i] = new int[counter];
			for (int j = 0; j < counter; j++) matrix[i][j] = 0;
		}

		for (int i = 0; i < x; i++)
			for (int j = 1; j < y; j++)
				if (data[i][j - 1] != data[i][j] && data[i][j - 1] != -1 && data[i][j] != -1)
					matrix[int(data[i][j - 1])][int(data[i][j])]++;

		for (int i = 0; i < y; i++)
			for (int j = 1; j < x; j++) 
				if (data[j - 1][i] != data[j][i] && data[j - 1][i] != -1 && data[j][i] != -1)
					matrix[int(data[j - 1][i])][int(data[j][i])]++;

		for (int i = 0; i < counter; i++) 
			for (int j = i; j < counter; j++) {
				matrix[i][j] += matrix[j][i];  matrix[j][i] = matrix[i][j];
			}
	}

	void merge_tiles(int** matrix) {
		//initialization 
		std::vector<std::vector<Point>> litospheros;
		int* current_tiles = new int[tiles];
		current_tiles[0] = 0;
		bool *stops = new bool[tiles], * Used = new bool[counter];
		stops[0] = true;
		int stop = 0;
		RandInt randP(counter, 1, seed);
		for (int i = 0; i < counter; i++) 	Used[i] = false;
		for (int i = 0; i < tiles; i++) stops[i] = false;
		litospheros.push_back(std::vector<Point>());
		//end of initialization 
		for (int i = 1; i < y; i++) {
			for (int j = 0; j < counter; j++) {
				if (data[0][i] == points[j].height) {
					litospheros[0].push_back(points[j]);
					Used[j] = true;
				}
				if (data[x - 1][i]== points[j].height) {
					litospheros[0].push_back(points[j]);
					Used[j] = true;
				}
			}
		}
		for (int i = 1; i < tiles; i++) {
			int r;
			do {
				r = randP.generate();

			} while (Used[r]);
			litospheros.push_back(std::vector<Point>());
			litospheros[i].push_back(points[r]);
		}
		for (int i = 1; i < tiles; i++) {
			litospheros.push_back(std::vector<Point>());
			int ind = litospheros[i][0].height;
			litospheros[i].push_back(points[ind]);
			Used[ind] = true;
			current_tiles[i] = ind;
		}

		while (stop != tiles - 1) {
			for (int i = 1; i < tiles; i++) {
				while (!stops[i]) {
					int ind1 = current_tiles[i];
					int max = -1;
					int ind2 = -1;
					for (int j = 0; j < counter; j++) 
						if (matrix[ind1][j] > max && !Used[j] && matrix[ind1][j] != 0) {
							max = matrix[ind1][j];
							ind2 = j;
						}
					if (ind2 == -1) {
						for (int j = 0; j < litospheros[i].size(); j++) {
							if (litospheros[i][j].height == ind1) {
								if (j + 1 == litospheros[i].size()) {
									stop++;
									stops[i] = true;
								}
								else current_tiles[i] = litospheros[i][j + 1].height;
							}
						}
					}
					else {
						litospheros[i].push_back(points[ind2]);
						Used[ind2] = true;
						break;
					}
				}
			}
		}
		//filling huge tiles with correct height (based on "central" point)
		for (int i = 0; i < tiles; i++) 
			for (int j = 0; j < litospheros[i].size(); j++) 
				toStandart(litospheros[i][j], litospheros[i][0].height);
	}

	void smooth_borders() {
		for (int i = 1; i < x - 1; i++) {
			for (int j = 1; j < y - 1; j++) {
				std::map<int, int> amounts = calc_amounts(i, j);
				int max_n = 0, max_h=0;
				for (auto a : amounts) {
					if (a.second > max_n) {
						max_n = a.second;
						max_h = a.first;
					}
				}
				heights[i][j] = max_h;
			}
		}
	}

	void add_horisontal_mountain(int k, int i, int j) {
		int k1 = 1, k2 = 1;
		if (heights[i][j] > heights[i][j + 1]) {
			k1 = 2; k2 = 1;
		}
		else {
			k1 = 1; k2 = 2;
		}
		for (int i1 = k; i1 > 0; i1--) {
			if ((j - k + i1 >= 0) && !visited[i][j - (k - i1)]) {
				extra_heights[i][j - (k - i1)] += i1 * k1;
				visited[i][j - (k - i1)] = true;
			}
			if ((j + -k + i1 < x) && !visited[i][j + (k - i1)])
			{
				extra_heights[i][j + (k - i1)] += i1 * k2;
				visited[i][j + (k - i1)] = true;
			}
		}
	}
	void add_vertical_mountain(int k, int i, int j) {
		int k1 = 1, k2 = 1;
		if (heights[i][j] > heights[i + 1][j]) {
			k1 = 2; k2 = 1;
		}
		else {
			k1 = 1; k2 = 2;
		}
		for (int i1 = k; i1 > 0; i1--) {
			if ((i + i1 - k >= 0) && !visited[i - (k - i1)][j]) {
				extra_heights[i - (k - i1)][j] += i1 * k1;
				visited[i - (k - i1)][j] = true;

			}
			if ((i + k - i1 < y) && !visited[i + (k - i1)][j]) {
				extra_heights[i + (k - i1)][j] += i1 * k2;
				visited[i + (k - i1)][j] = true;
			}
		}
	}

	void add_mountains(int k) {
		visited = new bool* [y];
		for (int i = 0; i < y; i++) {
			visited[i] = new bool[x];
			for (int j = 0; j < x; j++) visited[i][j] = false;
		}
		for (int i = 0; i < y; i++) 
			for (int j = 0; j < x - 1; j++) 
				if ((heights[i][j] > heights[i][j + 1]) || (heights[i][j + 1] > heights[i][j])) {
					add_horisontal_mountain(k, i, j);
					j += k;
				}
		for (int j = 0; j < x; j++) 
			for (int i = 0; i < y - 1; i++) 
				if ((heights[i][j] > heights[i + 1][j]) || (heights[i + 1][j] > heights[i][j]) && !visited[i][j] && !visited[i + 1][j]) {
					add_vertical_mountain(k, i, j);
					i += k;
				}
		delete[] visited;
	}

	void add_diamond_square(int x0, int x1, int y0, int y1) {
		int mid_x = x0 + (x1 - x0) / 2, mid_y = y0 + (y1 - y0) / 2,
			avg = (extra_heights[y0][x0] + extra_heights[y1][x0] + extra_heights[y0][x1] + extra_heights[y1][x1]) / 4;
		extra_heights[mid_y][mid_x] += avg + new_rand.generate();

		if ((x1 - x0) >= 2 && (y1 - y0) >= 2) {
			extra_heights[mid_y][x1] += new_rand.generate();
			extra_heights[mid_y][x0] += new_rand.generate();
			extra_heights[y1][mid_x] += new_rand.generate();
			extra_heights[y0][mid_x] += new_rand.generate();
			add_diamond_square(x0, mid_x, y0, mid_y);
			add_diamond_square(mid_x, x1, y0, mid_y);
			add_diamond_square(x0, mid_x, mid_y, y1);
			add_diamond_square(mid_x, x1, mid_y, y1);
		}
	}

public:
	int x, y, counter, tiles, seed, maxH, minH;
	Point* points;
	int** data;
	int** heights, ** extra_heights;
	bool** visited;
	RandInt new_rand;

	HeightMap(int x, int y, int minH, int maxH, int counter, int seed, int tiles) : 
		x(x), y(y), maxH(maxH), minH(minH), counter(counter), seed(seed), tiles(tiles), data(new int* [x]), points(new Point[counter]),
	new_rand(RandInt(10, 0, seed)){
		double mLen = (y / (sqrt(counter))) / 1.25;
		RandInt randH(maxH, minH, seed), randX(x - 1, 0, seed), randY(y - 1, 0, seed);
		bool* stops = new bool[counter];
		std::vector<Point>* extension = new std::vector<Point>[counter];

		generate_points(randX, randY, mLen, stops, extension);
		fill_small_points(stops, extension);

		delete[] extension;
		delete[] stops;

		int** matrix = new int* [counter];
		adjacency_matrix(matrix);
		merge_tiles(matrix);

		heights = new int* [x];
		extra_heights = new int* [x];

		//fill heights with normalized values
		for (int i = 0; i < x; i++) {
			heights[i] = new int[y];
			extra_heights[i] = new int[y];
			for (int j = 0; j < y; j++) {
				extra_heights[i][j] = 0;
				heights[i][j] = int((data[i][j] + 10) * 100) % maxH;
			}
		}
		smooth_borders();
		//add_mountains(7);
		//add_diamond_square(0, x-1, 0, y-1);
		
	}

	friend std::ostream& operator << (std::ostream& stream, const HeightMap& data) {
		for (int i = 0; i < data.x; i++) {
			for (int j = 0; j < data.y; j++) {
				stream << data.heights[i][j] + data.extra_heights[i][j]<< " ";
			}
			stream << "\n";
		}
		return stream;
	}
};