#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_CITIES 15
#define INF 1e9

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

void dfsTSP(City cities[], int numCities, int visited[], int route[], int currentCity, int depth, double *minDistance, int bestRoute[], int startLocation) {
    if (depth == numCities) {
        // Calculate the distance of the current route
        double totalDistance = 0.0;
        for (int i = 0; i < numCities; i++) {
            int currentCityIndex = route[i];
            int nextCityIndex = route[(i + 1) % numCities];
            totalDistance += calculateDistance(cities[currentCityIndex], cities[nextCityIndex]);
        }

        // Update the best route if the current route is shorter
        if (totalDistance < *minDistance) {
            *minDistance = totalDistance;
            memcpy(bestRoute, route, numCities * sizeof(int));
        }
        return;
    }

    // Explore all possible next cities
    for (int nextCity = 0; nextCity < numCities; nextCity++) {
        if (!visited[nextCity]) {
            visited[nextCity] = 1;
            route[depth] = nextCity;
            dfsTSP(cities, numCities, visited, route, nextCity, depth + 1, minDistance, bestRoute, startLocation);
            visited[nextCity] = 0; // Backtrack
        }
    }
}

void findShortestRoute(City cities[], int numCities, int startLocation) {
    int visited[MAX_CITIES] = {0};
    int route[MAX_CITIES];
    double minDistance = INF;
    int bestRoute[MAX_CITIES];

    visited[startLocation] = 1;
    route[0] = startLocation;

    clock_t start = clock();
    dfsTSP(cities, numCities, visited, route, startLocation, 1, &minDistance, bestRoute, startLocation);
    clock_t end = clock();
    double timeElapsed = (double)(end - start) / CLOCKS_PER_SEC;

    // Prepare the output for the best route found
    printf("Best route found: ");

    // Find the index where the starting location appears in the bestRoute
    int startIndex = -1;
    for (int i = 0; i < numCities; i++) {
        if (bestRoute[i] == startLocation) {
            startIndex = i;
            break;
        }
    }

    // Print the route from starting point to the end
    for (int i = startIndex; i < numCities; i++) {
        printf("%s", cities[bestRoute[i]].name);
        if (i < numCities - 1) {
            printf(" -> ");
        }
    }

    // Print the route from the beginning to the starting point
    printf(" -> %s\n", cities[startLocation].name);

    printf("Best route distance: %.6f km\n", minDistance);
    printf("Time elapsed: %.10f s\n", timeElapsed);
}

int main() {
    char filename[50];
    char startCityName[50];
    FILE *file;
    City cities[MAX_CITIES];
    int numCities = 0;
    int startLocation = -1;

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
        if (numCities >= MAX_CITIES) {
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
