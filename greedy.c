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

void findShortestRoute(City cities[], int numCities, int startLocation) {
    int visited[15] = {0};
    int route[15];
    double minDistance = 0.0;

    visited[startLocation] = 1;
    route[0] = startLocation;

    clock_t start = clock();
    int currentCity = startLocation;
    for (int i = 1; i < numCities; i++) {
        double minDist = INFINITY;
        int nextCity = -1;

        for (int j = 0; j < numCities; j++) {
            if (!visited[j]) {
                double dist = calculateDistance(cities[currentCity], cities[j]);
                if (dist < minDist) {
                    minDist = dist;
                    nextCity = j;
                }
            }
        }

        route[i] = nextCity;
        visited[nextCity] = 1;
        currentCity = nextCity;
        minDistance += minDist;
    }
    clock_t end = clock();
    double timeElapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Best route found: %s -> ", cities[startLocation].name);
    for (int i = 1; i < numCities; i++) {
        printf("%s", cities[route[i]].name);
        if (i < numCities - 1) {
            printf(" -> ");
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