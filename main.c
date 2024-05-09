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

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void findShortestRouteHelper(City cities[], int route[], int location, int numCities, double *minDistance, int *bestRoute, int startLocation) {
    if (location == numCities) {
        if (route[0] != startLocation || route[numCities - 1] != startLocation) {
            double totalDistance = 0.0;
            for (int i = 0; i < numCities; i++) {
                int currentCityIndex = route[i];
                int nextCityIndex = route[(i + 1) % numCities];
                totalDistance += calculateDistance(cities[currentCityIndex], cities[nextCityIndex]);
            }
            if (totalDistance < *minDistance) {
                *minDistance = totalDistance;
                memcpy(bestRoute, route, numCities * sizeof(int));
            }
        }
        return;
    }

    for (int i = location; i < numCities; i++) {
        swap(&route[location], &route[i]);
        findShortestRouteHelper(cities, route, location + 1, numCities, minDistance, bestRoute, startLocation);
        swap(&route[location], &route[i]);
    }
}

void findShortestRoute(City cities[], int numCities, int startLocation) {
    int route[15];
    double minDistance = INFINITY;
    int bestRoute[15];

    for (int i = 0; i < numCities; i++) {
        route[i] = i;
    }

    clock_t start = clock();
    findShortestRouteHelper(cities, route, 1, numCities, &minDistance, bestRoute, startLocation);
    clock_t end = clock();
    double timeElapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Best route found: %s -> ", cities[startLocation].name);
    for (int i = 0; i < numCities; i++) {
        if (bestRoute[i] != startLocation) {
            printf("%s", cities[bestRoute[i]].name);
            if (i < numCities - 1 && bestRoute[i + 1] != startLocation) {
                printf(" -> ");
            }
        }
    }
    printf(" -> %s\n", cities[startLocation].name);

    printf("Best route distance: %.6f km\n", minDistance);
    printf("Time elapsed: %.10f s\n", timeElapsed);
}

int main() {
    char filename[50];
    char startCityName[50];
    FILE *file;
    City cities[15];
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