#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

typedef struct {
    char name[50];
    double latitude;
    double longitude;
} City;

double degreesToRadians(double degrees) {
    return degrees * 3.14159265358979323846 / 180.0;
}

double calculateDistance(City city1, City city2) {
    double lat1 = degreesToRadians(city1.latitude);
    double lon1 = degreesToRadians(city1.longitude);
    double lat2 = degreesToRadians(city2.latitude);
    double lon2 = degreesToRadians(city2.longitude);

    double deltaLat = lat2 - lat1;
    double deltaLon = lon2 - lon1;

    double a = sin(deltaLat / 2) * sin(deltaLat / 2) +
               cos(lat1) * cos(lat2) *
               sin(deltaLon / 2) * sin(deltaLon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    double distance = 6371 * c;
    return distance;
}

void updatePheromones(double pheromones[15][15], int numCities, int route[], double totalDistance) {
    // Evaporate pheromones
    for (int i = 0; i < numCities; i++) {
        for (int j = 0; j < numCities; j++) {
            if (i != j) {
                pheromones[i][j] *= (1.0 - 0.5);
                if (pheromones[i][j] < 0.0) {
                    pheromones[i][j] = 1.0;  // Prevent negative pheromone values
                }
            }
        }
    }

    // Deposit new pheromones
    for (int i = 0; i < numCities - 1; i++) {
        int city1 = route[i];
        int city2 = route[i + 1];
        pheromones[city1][city2] += (100 / totalDistance);
        pheromones[city2][city1] += (100 / totalDistance);
    }
}

void findShortestRoute(City cities[], int numCities, int startLocation) {
    double pheromones[15][15];
    double distances[15][15];
    int bestRoute[15];
    double minDistance = INFINITY;
    int ant, i, j;

    // Initialize pheromone levels and distances
    for (i = 0; i < numCities; i++) {
        for (j = 0; j < numCities; j++) {
            if (i != j) {
                pheromones[i][j] = 1.0;  // Initialize pheromones to a small value
                distances[i][j] = calculateDistance(cities[i], cities[j]);
            } else {
                pheromones[i][j] = 0.0;  // No pheromone on same city
                distances[i][j] = 0.0;    // No distance between same city
            }
        }
    }

    clock_t start = clock();
    
    // ACO main loop
    for (ant = 0; ant < numCities; ant++) {
        int visited[15] = {0};
        int route[15];
        double totalDistance = 0.0;

        // Ant's initial city
        int currentCity = startLocation;
        visited[currentCity] = 1;
        route[0] = currentCity;

        // Build the route for the current ant
        for (i = 1; i < numCities; i++) {
            double probabilities[15] = {0.0};
            double sum = 0.0;

            // Calculate probabilities for the next city
            for (j = 0; j < numCities; j++) {
                if (!visited[j]) {
                    probabilities[j] = pow(pheromones[currentCity][j], 1) * pow(1.0 / distances[currentCity][j], 5);
                    sum += probabilities[j];
                }
            }

            // Roulette wheel selection
            double roulette = (double)rand() / RAND_MAX * sum;
            double selectProb = 0.0;

            for (j = 0; j < numCities; j++) {
                if (!visited[j]) {
                    selectProb += probabilities[j];
                    if (selectProb >= roulette) {
                        currentCity = j;
                        break;
                    }
                }
            }

            visited[currentCity] = 1;
            route[i] = currentCity;
            totalDistance += distances[route[i - 1]][currentCity];
        }

        totalDistance += distances[route[numCities - 1]][startLocation];

        // Update pheromone levels
        updatePheromones(pheromones, numCities, route, totalDistance);

        // Check if the current ant found a better route
        if (totalDistance < minDistance) {
            minDistance = totalDistance;
            memcpy(bestRoute, route, sizeof(route));
        }
    }

    clock_t end = clock();
    double timeElapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Best route found: %s", cities[bestRoute[0]].name);
    for (i = 1; i < numCities; i++) {
        printf(" -> %s", cities[bestRoute[i]].name);
    }
    printf(" -> %s\n", cities[bestRoute[0]].name);  // Return to the starting point

    printf("Best route distance: %.6f km\n", minDistance);
    printf("Time elapsed: %.6f s\n", timeElapsed);
}

int main() {
    char filename[50];
    char startCityName[50];
    FILE *file;
    City cities[15];
    int numCities = 0;
    int startLocation = -1;

    srand(time(NULL));  // Seed for random numbers

    printf("Enter list of cities file name: ");
    scanf("%s", filename);
    printf("Enter starting point: ");
    scanf("%s", startCityName);

    file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: File not found.\n");
        return 1;
    }

    while (fscanf(file, "%[^,],%lf,%lf\n", cities[numCities].name, &cities[numCities].latitude, &cities[numCities].longitude) != EOF) {
        if (strcmp(cities[numCities].name, startCityName) == 0) {
            startLocation = numCities;
        }
        numCities++;
        if (numCities >= 15) {
            break;
        }
    }

    fclose(file);

    if (startLocation == -1) {
        printf("Error: Starting city not found in the list.\n");
        return 1;
    }

    findShortestRoute(cities, numCities, startLocation);

    return 0;
}