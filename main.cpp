#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

// coordinate struct
struct Coordinate {
    int id;
    double x;
    double y;
};

// input map
struct Map {
    int n;
    std::vector<Coordinate> coordinates;
};

// result TSP tour
struct Tour {
    int m;
    std::vector<int> path;
    double dist;
};

void calculateDist(Tour& tour, const Map& map) {
    tour.dist = 0.0;
    for (int i = 0; i < tour.m; ++i) {
        Coordinate a = map.coordinates[tour.path[i]];
        Coordinate b = map.coordinates[tour.path[(i + 1) % tour.m]];
        double dx = a.x - b.x;
        double dy = a.y - b.y;
        tour.dist += std::sqrt(dx * dx + dy * dy);
    }
}

void naiveTSP(const Map& map, Tour& tour) {
    // set vector size
    tour.path.resize(map.n);
    // set number of nodes in tour
    tour.m = map.n;
    // create used vector
    std::vector<int> used = std::vector<int>(map.n, 0);
    tour.path[0] = 0;
    used[0] = 1;

    // construct tour
    for (int i = 1; i < tour.m; ++i) {
        int best = -1;
        for (int j = 0; j < map.n; ++j) {
            if (!used[j] && (best == -1 || (map.coordinates[j].x < map.coordinates[best].x && map.coordinates[j].y < map.coordinates[best].y))) {
                best = j;
            }
        }
        tour.path[i] = best;
        used[best] = 1;
    }

    // calculate dist
    tour.dist = 0.0;
    calculateDist(tour, map);
}

void randomTSP(const Map& map, Tour& tour) {
    // set vector size
    tour.path.resize(map.n);
    // set number of nodes in tour
    tour.m = map.n;
    // create used vector
    std::vector<int> used = std::vector<int>(map.n, 0);

    // construct tour
    for (int i = 0; i < tour.m; ++i) {
        int r = rand() % map.n;
        while (used[r]) {
            r = rand() % map.n;
        }
        tour.path[i] = r;
        used[r] = 1;
    }

    // calculate dist
    tour.dist = 0.0;
    calculateDist(tour, map);
}

void groupTSP(const Map& map, Tour& tour) {
    // TODO
}

void readMap(Map& map) {
    // first read the number of nodes
    std::cin >> map.n;

    // set the vector size of the coordinates
    map.coordinates.resize(map.n);

    // read the coordinates
    for (int i = 0; i < map.n; ++i) {
        double x, y;
        std::cin >> x >> y;
        Coordinate coord = {i, x, y};
        map.coordinates[i] = coord;
    }
}

void printMap(const Map& map) {
    // print every point on the map
    for (const auto& coord : map.coordinates) {
        std::cout << coord.id << ": " << coord.x << " " << coord.y << std::endl;
    }
}

void printTour(const Tour& tour) {
    // print each point in tour
    for (const auto& node : tour.path) {
        std::cout << node << "\n";
    }
    // std::cout << tour.dist << std::endl;
    // std::cout << std::endl;
}

int main(void) {
    Map map;
    readMap(map); 
    // printMap(map);

    // naive TSP
    Tour naiveTour;
    naiveTSP(map, naiveTour);
    // std::cout << "Naive tour: \n";
    // printTour(naiveTour);

    // random TSP
    Tour randomTour;
    for(int i = 0; i < 10000; ++i){
        Tour randomTourTmp;
        randomTSP(map, randomTourTmp);
        if(i == 0 || randomTourTmp.dist < randomTour.dist){
            randomTour = randomTourTmp;
        }
    }
    // std::cout << "Random tour: \n";
    // printTour(randomTour);

    Tour groupTour;
    groupTSP(map, groupTour);

    if(randomTour.dist < naiveTour.dist){
        printTour(randomTour);
    } else {
        printTour(naiveTour);
    }

    return 0; 
}