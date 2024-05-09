#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_CITIES 15
#define EARTH_RADIUS 6371.0

typedef struct {
    char name[50];
    double latitude;
    double longitude;
} City;

// Function to calculate distance between two cities using Haversine formula
double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    double phi1 = lat1 * M_PI / 180.0;
    double phi2 = lat2 * M_PI / 180.0;
    double delta_phi = (lat2 - lat1) * M_PI / 180.0;
    double delta_lambda = (lon2 - lon1) * M_PI / 180.0;

    double a = sin(delta_phi / 2) * sin(delta_phi / 2) +
               cos(phi1) * cos(phi2) *
               sin(delta_lambda / 2) * sin(delta_lambda / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double distance = EARTH_RADIUS * c;

    return distance;
}

// Function to read cities data from CSV file
int readCitiesFromFile(const char *filename, City cities[]) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("File not found.\n");
        return 0;
    }

    int cityCount = 0;
    char line[100];

    while (fgets(line, sizeof(line), file) != NULL && cityCount < MAX_CITIES) {
        char *name = strtok(line, ",");
        double lat = atof(strtok(NULL, ","));
        double lon = atof(strtok(NULL, ","));

        strcpy(cities[cityCount].name, name);
        cities[cityCount].latitude = lat;
        cities[cityCount].longitude = lon;

        cityCount++;
    }

    fclose(file);
    return cityCount;
}

// Nearest Neighbor algorithm to find shortest TSP route
void nearestNeighbor(int startCity, int cityCount, City cities[]) {
    int visited[MAX_CITIES] = {0};
    visited[startCity] = 1;

    int currentCity = startCity;
    int route[MAX_CITIES];
    route[0] = startCity;
    int step = 1;

    double totalDistance = 0.0;

    while (step < cityCount) {
        int nearestCity = -1;
        double minDistance = INFINITY;

        for (int i = 0; i < cityCount; i++) {
            if (i != currentCity && !visited[i]) {
                double dist = calculateDistance(cities[currentCity].latitude, cities[currentCity].longitude,
                                                cities[i].latitude, cities[i].longitude);
                if (dist < minDistance) {
                    minDistance = dist;
                    nearestCity = i;
                }
            }
        }

        if (nearestCity != -1) {
            visited[nearestCity] = 1;
            route[step++] = nearestCity;
            totalDistance += minDistance;
            currentCity = nearestCity;
        }
    }

    // Return to start city
    totalDistance += calculateDistance(cities[currentCity].latitude, cities[currentCity].longitude, cities[startCity].latitude, cities[startCity].longitude);
    route[step++] = startCity;

    printf("Best route found: ");
    for (int i = 0; i < step; i++) {
        printf("%s -> ", cities[route[i]].name);
    }
    printf("\nBest route distance: %.6f km\n", totalDistance);
}

int main() {
    char filename[100];
    char startCityName[50];
    City cities[MAX_CITIES];

    printf("Enter list of cities file name: ");
    scanf("%s", filename);

    int cityCount = readCitiesFromFile(filename, cities);
    if (cityCount == 0) {
        return 1;
    }

    printf("Enter starting point: ");
    scanf("%s", startCityName);

    int startCityIndex = -1;
    for (int i = 0; i < cityCount; i++) {
        if (strcmp(cities[i].name, startCityName) == 0) {
            startCityIndex = i;
            break;
        }
    }

    if (startCityIndex == -1) {
        printf("Starting city not found.\n");
        return 1;
    }

    clock_t startTime = clock();
    nearestNeighbor(startCityIndex, cityCount, cities);
    clock_t endTime = clock();

    double timeElapsed = (double)(endTime - startTime) / CLOCKS_PER_SEC;
    printf("Time elapsed: %.10f s\n", timeElapsed);

    return 0;
}
