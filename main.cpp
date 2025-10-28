#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <functional>
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
    double x,y;
};

// result TSP tour
struct Tour {
    int m;
    std::vector<int> path;
    double dist;
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
        Coordinate coord = {i, x, y};
        map.coordinates[i] = coord;
    }
}

void printTour(const Tour& tour, bool dist) {
    // print each point in 
    if(!dist){
        for (const auto& node : tour.path) {
            std::cout << node << "\n";
        }
    } else
        std::cout << tour.dist << std::endl;
}

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

int distance(Coordinate a, Coordinate b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return static_cast<int>(std::round(std::sqrt(dx * dx + dy * dy)));
}

void naiveTSP(const Map& map, Tour& tour, int start) {
    // set vector size
    tour.path.resize(map.n);
    // set number of nodes in tour
    tour.m = map.n;
    // create used vector
    std::vector<bool> used = std::vector<bool>(map.n, false);
    tour.path[0] = map.coordinates[start].id;
    used[start] = true;

    // construct tour
    for (int i = 1; i < tour.m; ++i) {
        int best = -1;
        for (int j = 0; j < map.n; ++j) {
            if (!used[j] && (best == -1 || distance(map.coordinates[j], map.coordinates[tour.path[i-1]]) < distance(map.coordinates[best], map.coordinates[tour.path[i-1]]))) {
                best = j;
            }
        }
        tour.path[i] = map.coordinates[best].id;
        used[best] = true;
    }

    // calculate dist
    tour.dist = 0.0;
    calculateDist(tour, map);
}

void randomTSP(const Map& map, Tour& tour) {
    for(int i = 0; i < 1000; ++i){
        Tour randomTourTmp;
        // set vector size
        randomTourTmp.path.resize(map.n);
        // set number of nodes in tour
        randomTourTmp.m = map.n;
        // create used vector
        std::vector<int> used = std::vector<int>(map.n, 0);

        // construct tour
        for (int i = 0; i < randomTourTmp.m; ++i) {
            int r = rand() % map.n;
            while (used[r]) {
                r = rand() % map.n;
            }
            randomTourTmp.path[i] = r;
            used[r] = 1;
        }

        // calculate dist
        randomTourTmp.dist = 0.0;
        calculateDist(randomTourTmp, map);
        if(i == 0 || randomTourTmp.dist < tour.dist){
            tour = randomTourTmp;
        }
    }
}

void optimizeNaiveLocal(const Map& map, Tour& tour) {
    bool improved = true;
    while (improved) {
        improved = false;
        for (int i = 0; i < tour.m - 1; i++) {
            for (int j = i + 2; j < tour.m; j++) {
                // switch two pairs
                int a = tour.path[i];
                int b = tour.path[i + 1];
                int c = tour.path[j];
                int d = tour.path[(j + 1) % tour.m];
                
                double oldDist = distance(map.coordinates[a], map.coordinates[b]) + distance(map.coordinates[c], map.coordinates[d]);
                double newDist = distance(map.coordinates[a], map.coordinates[c]) + distance(map.coordinates[b], map.coordinates[d]);

                if (newDist < oldDist) {
                    std::reverse(tour.path.begin() + i + 1, tour.path.begin() + j + 1);
                    tour.dist += (newDist - oldDist);
                    improved = true;
                }
            }
        }
    }
}

void optimizeNaiveTSP(const Map& map, Tour& tour, int iterations) {
    double minDist = -1.0;
    for(int it = 0; it < iterations; it++){
        Tour tempTour;
        naiveTSP(map, tempTour, rand() % map.n);
        optimizeNaiveLocal(map, tempTour);
        if(minDist == -1.0 || tempTour.dist < minDist){
            minDist = tempTour.dist;
            tour = tempTour;
        }
    }
}

void printAllDistances(const Tour& randomTour, const Tour& naiveTour, const Tour& optimizedTour){
    std::cout << "Naive tour distance: " << naiveTour.dist << std::endl;
    std::cout << "Random tour distance: " << randomTour.dist << std::endl;
    std::cout << "Optimized tour distance: " << optimizedTour.dist << std::endl;
}


void printDev(Tour& randomTour, Tour& naiveTour, Tour& optimizedTour){
    std::cout << "Naive tour: \n";
    printTour(naiveTour, true);

    std::cout << "Random tour: \n";
    printTour(randomTour, true);
    std::cout << "Optimized tour: \n";
    printTour(optimizedTour, true);
}

void printKattis(Tour& randomTour, Tour& naiveTour, Tour& optimizedTour){
    if(randomTour.dist < naiveTour.dist && randomTour.dist < optimizedTour.dist){
        printTour(randomTour, false);
    } else if(naiveTour.dist < optimizedTour.dist){
        printTour(naiveTour, false);
    } else {
        printTour(optimizedTour, false);
    }
}

int main(void) {
    Map map;
    readMap(map);

    // naive TSP
    Tour naiveTour;
    naiveTSP(map, naiveTour, 0);

    // random TSP
    Tour randomTour;
    randomTSP(map, randomTour);

    //optimized naive TSP
    Tour optimizedTour;
    int iterations = 10;
    optimizeNaiveTSP(map, optimizedTour, iterations);

    printKattis(randomTour, naiveTour, optimizedTour);
    //printDev(randomTour, naiveTour, optimizedTour);
    // printAllDistances(randomTour, naiveTour, optimizedTour);


    return 0; 
}