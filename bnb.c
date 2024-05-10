#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <time.h>

typedef struct {
    char name[50];
    double latitude;
    double longitude;
} City;

typedef struct {
    int *route;
    int level;
    double cost;
} Node;

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

double calculateRouteDistance(City cities[], int route[], int numCities) {
    double totalDistance = 0.0;
    for (int i = 0; i < numCities - 1; i++) {
        totalDistance += calculateDistance(cities[route[i]], cities[route[i + 1]]);
    }
    totalDistance += calculateDistance(cities[route[numCities - 1]], cities[route[0]]);
    return totalDistance;
}

void branchAndBound(City cities[], int numCities, int startLocation) {
    int *initialRoute = (int *)malloc(numCities * sizeof(int));
    for (int i = 0; i < numCities; i++) {
        initialRoute[i] = i;
    }
    
    Node initialNode = {initialRoute, 0, 0.0};
    Node currentNode = initialNode;

    double minDistance = INT_MAX;
    int *bestRoute = (int *)malloc(numCities * sizeof(int));
    memcpy(bestRoute, initialRoute, numCities * sizeof(int));

    clock_t start = clock();

    while (currentNode.level < numCities - 1) {
        for (int i = currentNode.level + 1; i < numCities; i++) {
            swap(&currentNode.route[currentNode.level + 1], &currentNode.route[i]);

            double currentCost = calculateRouteDistance(cities, currentNode.route, numCities);

            if (currentCost < minDistance) {
                memcpy(bestRoute, currentNode.route, numCities * sizeof(int));
                minDistance = currentCost;
            }

            swap(&currentNode.route[currentNode.level + 1], &currentNode.route[i]);
        }

        currentNode.level++;
    }

    // Tambahkan kembali kota awal ke akhir rute
    bestRoute[numCities] = startLocation; // Tambahkan kembali kota awal ke akhir rute

    clock_t end = clock();
    double timeElapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Best route found: ");
    for (int i = 0; i <= numCities; i++) { // Iterasi hingga numCities (termasuk kembali ke kota awal)
        printf("%s", cities[bestRoute[i]].name);
        if (i < numCities) {
            printf(" -> ");
        }
    }
    printf("\nBest route distance: %.6f km\n", minDistance);
    printf("Time elapsed: %.10f s\n", timeElapsed);

    free(initialRoute);
    free(bestRoute);
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

    branchAndBound(cities, numCities, startLocation);

    return 0;
}