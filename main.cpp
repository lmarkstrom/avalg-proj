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
    for (int i = 0; i < tour.m; ++i) {
        int best = -1;
        for (int j = 0; j < map.n; ++j) {
            if (!used[j] && (best == -1 || distance(map.coordinates[j], map.coordinates[tour.path[i]]) < distance(map.coordinates[best], map.coordinates[tour.path[i]]))) {
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

void createGroups(const Map& map, std::vector<Map>& groups, int k) {
    std::vector<int> groupLeaders;
    std::vector<bool> used = std::vector<bool>(map.n, false);

    groupLeaders.push_back(rand() % map.n);
    while(groupLeaders.size() <= k){
        int maxDistPos = -1;
        double maxDist = -1.0;
        for(int i = 0; i < map.n; ++i){
            // check if leader
            if(used[i]){
                continue;
            }
            // calculate lowest distance to a leader
            double lowestDistToLeader = -1.0;
            for(const auto& leader : groupLeaders){
                double dist = distance(map.coordinates[i], map.coordinates[leader]);
                if(dist < lowestDistToLeader || lowestDistToLeader == -1.0){
                    lowestDistToLeader = dist;
                }
            }
            // update max pos
            if(maxDistPos == -1 || lowestDistToLeader > maxDist){
                maxDistPos = i;
                maxDist = lowestDistToLeader;
            }
        }
        // add the best candidate to the group leaders
        groupLeaders.push_back(maxDistPos);
        used[maxDistPos] = true;
        Map newGroup;
        newGroup.coordinates.push_back(map.coordinates[maxDistPos]);
        groups.push_back(newGroup);
    }
    // create groups based on the group leaders
    for(int i = 0; i < map.n; i++){
        if(used[i]){
            continue;
        }
        int minDistLeader = -1;
        double minDist = -1.0;
        for(int j = 0; j < k; j++){
            double dx = map.coordinates[i].x - map.coordinates[groupLeaders[j]].x;
            double dy = map.coordinates[i].y - map.coordinates[groupLeaders[j]].y;
            double dist = std::sqrt(dx * dx + dy * dy);
            if (minDistLeader == -1 || dist < minDist){
                minDistLeader = j;
                minDist = dist;
            }
        }
        groups[minDistLeader].coordinates.push_back(map.coordinates[i]);
        used[i] = true;
    }
}

void groupTSP(const Map& map, Tour& tour, int k) {
    std::vector<Map> groups;
    createGroups(map, groups, k);

    // calc group avg positions
    Map groupOrderMap;
    groupOrderMap.n = groups.size();
    int i = 0;
    for(auto& group : groups){
        double avgX = 0.0;
        double avgY = 0.0;
        for(const auto& coord : group.coordinates){
            avgX += coord.x;
            avgY += coord.y;
        }
        avgX /= group.coordinates.size();
        avgY /= group.coordinates.size();
        group.x += avgX;
        group.y += avgY;
        groupOrderMap.coordinates.push_back({i, avgX, avgY});
        i++;
    }
    Tour groupOrderTour;

    // start at every group to find the best ordering of groups
    // saving the best tour found
    double minDist = -1.0;
    Tour bestGroupOrderTour;
    for (int i = 0; i < groupOrderMap.n; i++)
    {
        naiveTSP(groupOrderMap, groupOrderTour, i);
        if (minDist == -1.0 || groupOrderTour.dist < minDist)
        {
            minDist = groupOrderTour.dist;
            bestGroupOrderTour = groupOrderTour;
        }
    }

    tour.path.clear();

    // insert group tours
    for (int i = 0; i < bestGroupOrderTour.m; i++) {
        int groupIdx = bestGroupOrderTour.path[i];
        Map& group = groups[groupIdx];

        Tour groupTour;
        groupTour.path.resize(group.coordinates.size());

        Map groupMap;
        groupMap.n = static_cast<int>(group.coordinates.size());
        groupMap.coordinates = std::vector<Coordinate>(group.coordinates);

        for (int j = 0; j < groupMap.n; ++j) {
            groupMap.coordinates[j].id = j;
        }

        // try 10 different random starting points to find best local tour
        double minGroupDist = -1.0;
        Tour bestGroupTour;
        for (int i = 0; i < 10; i++) {
            Tour tempGroupTour;
            naiveTSP(groupMap, tempGroupTour, rand() % groupMap.n);
            if (minGroupDist == -1.0 || tempGroupTour.dist < minGroupDist) {
                minGroupDist = tempGroupTour.dist;
                bestGroupTour = tempGroupTour;
            }
        }
        groupTour = bestGroupTour;

        for (int j = 0; j < groupTour.m; ++j) {
            int localIdx = groupTour.path[j];
            int originalId = group.coordinates[localIdx].id;
            tour.path.push_back(originalId);
        }
    }

    tour.m = static_cast<int>(tour.path.size());
    calculateDist(tour, map);
}

void optimizeNaiveLocal(const Map& map, Tour& tour, int depth) {
    // make swaps to improve tour
    for (int i = 0; i < depth; i++) {
        for (int j = i + 1; j < tour.m; j++) {
            std::swap(tour.path[i], tour.path[j]);
            double oldDist = tour.dist;
            calculateDist(tour, map);
            if (tour.dist >= oldDist) {
                std::swap(tour.path[i], tour.path[j]);
                tour.dist = oldDist;
            }
        }
    }
}

void optimizeNaiveTSP(const Map& map, Tour& tour, int depth, int iterations) {
    double minDist = -1.0;
    for(int it = 0; it < iterations; it++){
        Tour tempTour;
        naiveTSP(map, tempTour, rand() % map.n); // naive with random start
        optimizeNaiveLocal(map, tempTour, depth); // small local swaps
        if(minDist == -1.0 || tempTour.dist < minDist){
            minDist = tempTour.dist;
            tour = tempTour;
        }
    }
}

void printDev(Tour& randomTour, Tour& naiveTour, Tour& groupTour, Tour& optimizedTour){
    std::cout << "Naive tour: \n";
    printTour(naiveTour);

    std::cout << "Random tour: \n";
    printTour(randomTour);

    std::cout << "Group tour: \n";
    printTour(groupTour);

    std::cout << "Optimized tour: \n";
    printTour(optimizedTour);
}

void printKattis(Tour& randomTour, Tour& naiveTour, Tour& groupTour, Tour& optimizedTour){
    if(randomTour.dist < naiveTour.dist && randomTour.dist < optimizedTour.dist){
        printTour(randomTour);
    } else if(naiveTour.dist < optimizedTour.dist){
        printTour(naiveTour);
    } else {
        printTour(optimizedTour);
    }
}

int main(void) {
    Map map;
    readMap(map); 
    // printMap(map);

    // naive TSP
    Tour naiveTour;
    naiveTSP(map, naiveTour, 0);

    // random TSP
    Tour randomTour;
    for(int i = 0; i < 1000; ++i){
        Tour randomTourTmp;
        randomTSP(map, randomTourTmp);
        if(i == 0 || randomTourTmp.dist < randomTour.dist){
            randomTour = randomTourTmp;
        }
    }

    // group TSP
    Tour groupTour;
    int k = static_cast<int>(std::round(std::sqrt(map.n)));
    if (k < 1) k = 1;
    if (k >= map.n) k = map.n - 1;
    groupTSP(map, groupTour, k);
    
    // optimized naive TSP
    Tour optimizedTour;
    int depth = 2;
    int iterations = 10;
    optimizeNaiveTSP(map, optimizedTour, depth, iterations);


    printDev(randomTour, naiveTour, groupTour, optimizedTour);
    printKattis(randomTour, naiveTour, groupTour, optimizedTour);

    return 0; 
}