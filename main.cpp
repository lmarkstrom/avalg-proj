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

void naiveTSP(const Map& map, Tour& tour, int n) {
    // set vector size
    tour.path.resize(map.n);
    // set number of nodes in tour
    tour.m = map.n;
    // create used vector
    std::vector<bool> used = std::vector<bool>(map.n, false);
    tour.path[0] = map.coordinates[0].id;
    used[0] = true;

    // construct tour
    for (int i = 1; i < tour.m; ++i) {
        int best = -1;
        for (int j = 1; j < map.n; ++j) {
            if (!used[j] && (best == -1 || distance(map.coordinates[j], map.coordinates[tour.path[i - 1]]) < distance(map.coordinates[best], map.coordinates[tour.path[i - 1]]))) {
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
            // calculate total distance to leaders
            double dist = 0.0;
            for(const auto& leader : groupLeaders){
                double dx = map.coordinates[i].x - map.coordinates[leader].x;
                double dy = map.coordinates[i].y - map.coordinates[leader].y;
                dist += std::sqrt(dx * dx + dy * dy);
            }
            // update max pos
            if(maxDistPos == -1 || dist > maxDist){
                maxDistPos = i;
                maxDist = dist;
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
    naiveTSP(groupOrderMap, groupOrderTour, groupOrderMap.n);

    tour.path.clear();

    // insert group tours
    for (int i = 0; i < groupOrderTour.m; i++) {
        int groupIdx = groupOrderTour.path[i];
        Map& group = groups[groupIdx];

        Tour groupTour;
        groupTour.path.resize(group.coordinates.size());

        Map groupMap;
        groupMap.n = static_cast<int>(group.coordinates.size());
        groupMap.coordinates = std::vector<Coordinate>(group.coordinates);

        for (int j = 0; j < groupMap.n; ++j) {
            groupMap.coordinates[j].id = j;
        }

        naiveTSP(groupMap, groupTour, groupMap.n);

        for (int j = 0; j < groupTour.m; ++j) {
            int localIdx = groupTour.path[j];
            int originalId = group.coordinates[localIdx].id;
            tour.path.push_back(originalId);
        }
    }

    tour.m = static_cast<int>(tour.path.size());
    calculateDist(tour, map);

}

int main(void) {
    Map map;
    readMap(map); 
    // printMap(map);

    // naive TSP
    Tour naiveTour;
    naiveTSP(map, naiveTour, map.n);
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
    int k = static_cast<int>(std::round(std::sqrt(map.n)));
    if (k < 1) k = 1;
    // if (k >= map.n) k = map.n - 1;
    groupTSP(map, groupTour, k);
    // std::cout << "Group tour: \n";
    // printTour(groupTour);

    if(randomTour.dist < naiveTour.dist && randomTour.dist < groupTour.dist){
        printTour(randomTour);
    } else if(naiveTour.dist < groupTour.dist){
        printTour(naiveTour);
    } else {
        printTour(groupTour);
    }

    return 0; 
}