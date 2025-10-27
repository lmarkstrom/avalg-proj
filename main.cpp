#include <iostream>
#include <vector>
using namespace std;

struct Coordinate {
    double x;
    double y;
};

struct Map {
    int n;
    std::vector<Coordinate> coordinates;
};

void readMap(Map& map) {
    // first read the number of nodes
    std::cin >> map.n;

    // set the vector size of the coordinates
    map.coordinates.resize(map.n);

    // read the coordinates
    for (int i = 0; i < map.n; ++i) {
        double x, y;
        std::cin >> x >> y;
        Coordinate coord = {x, y};
        map.coordinates[i] = coord;
    }
}

void printMap(const Map& map) {
    // print every point on the map
    for (const auto& coord : map.coordinates) {
        std::cout << coord.x << " " << coord.y << std::endl;
    }
}

int main(void) {
    Map map;
    readMap(map); 
    printMap(map);

    return 0; 
}