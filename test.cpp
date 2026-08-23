#include "new_heights.h"
#include "Surface.h"
#include "elution.h"
#include <iostream>
#include <fstream>
#include <string>

void main() {
    HeightMap* map;
    map = new HeightMap(512, 512, 98, 135, 128, 2456, 14);
    bool** isMild = new bool* [512];
    for (int i = 0; i < 512; i++) {
        isMild[i] = new bool[512];
        for (int j = 0; j < 512; j++) isMild[i][j] = true;
    }
    //map->generate();
    //int** res = generateLitospheros(map->x, map->y, map->heights, 128, map->tiles, map->tiles_points);
   /* for (int i = 0; i < map->x; i++) {
        for (int j = 0; j < map->y; j++) {
            std::cout << res[i][j] << " ";
        }
        std::cout << std::endl;
    }*/


    // мап со всеми плитами
    std::map<int, LitosPlate> plates= fill_litos(map->heights, map->x, map->y, map->tiles);
    // рассчет сдвига для каждой плиты 
    for (auto& [i, p] : plates)
        p.calc_shifts(map->x, map->y);
    // просто массив сдвигов и их высот 
    int** res = heights_with_shifts(map->heights, plates, map->x, map->y, map->tiles);
    map->smooth_surface(12);
    map->add_mountains(8, res);
    for (int i = 0; i < map->x; i++) {
        for (int j = 0; j < map->y; j++) {
            map->heights[i][j] = res[i][j] + map->heights[i][j] + map->extra_heights[i][j] ;
        }
    }
    map->smooth_surface(1);
    map->heights = elution(map->x, map->y, map->heights, 114, 2, isMild);
    map->heights = coldElution(map->x, map->y, map->heights, 114, 64, 64, 2, isMild, 193831);
    map->heights = oceanSmoothing(map->x, map->y, map->heights, 114, 10);
    std::ofstream file("C:/Users/user/PycharmProjects/pythonProject/data01.txt");
    if (file.is_open()) {
        file << *map;
        file.close();
    }
}