#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <math.h>

#define MAX_CITIES 100

typedef struct {
    char name[50];
    double latitude;
    double longitude;
} City;

typedef struct {
    int path[MAX_CITIES];
    int distance;
} Route;

double calculateDistance(City city1, City city2) {
    // Calculate distance between two cities using Haversine formula
    double lat1 = city1.latitude;
    double lon1 = city1.longitude;
    double lat2 = city2.latitude;
    double lon2 = city2.longitude;

    // Convert latitude and longitude from degrees to radians
    lat1 *= 3.14159265358979323846 / 180.0;
    lon1 *= 3.14159265358979323846 / 180.0;
    lat2 *= 3.14159265358979323846 / 180.0;
    lon2 *= 3.14159265358979323846 / 180.0;

    // Earth radius in kilometers
    double earthRadius = 6371.0;

    // Haversine formula to calculate distance between two points on Earth
    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;
    double a = sin(dLat / 2) * sin(dLat / 2) + cos(lat1) * cos(lat2) * sin(dLon / 2) * sin(dLon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double distance = earthRadius * c;

    return distance;
}

void bfsTSP(City cities[], int numCities, int startCity) {
    int visited[MAX_CITIES] = {0};
    Route queue[MAX_CITIES * MAX_CITIES];
    int front = 0, rear = 0;

    // Initialize the queue with the starting city
    Route initialRoute;
    initialRoute.path[0] = startCity;
    initialRoute.distance = 1; // Start with one city visited
    queue[rear++] = initialRoute;
    visited[startCity] = 1;

    Route bestRoute;
    bestRoute.distance = INT_MAX; // Initialize with a large value

    while (front != rear) {
        Route currentRoute = queue[front++];
        int currentCity = currentRoute.path[currentRoute.distance - 1];
        visited[currentCity] = 1;

        // If we have visited all cities once and return to the start
        if (currentRoute.distance == numCities && currentCity == startCity) {
            if (currentRoute.distance < bestRoute.distance) {
                bestRoute = currentRoute;
            }
            continue;
        }

        // Explore all unvisited neighboring cities
        for (int nextCity = 0; nextCity < numCities; nextCity++) {
            if (!visited[nextCity]) {
                double distance = calculateDistance(cities[currentCity], cities[nextCity]);

                // Create a new route extending the current route
                Route newRoute = currentRoute;
                newRoute.path[newRoute.distance++] = nextCity;

                // Enqueue the new route
                queue[rear++] = newRoute;
            }
        }
    }

    // Print the best route found
    printf("Best route found: ");
    for (int i = 0; i < numCities; i++) {
        printf("%s", cities[bestRoute.path[i]].name);
        if (i < numCities - 1) {
            printf(" -> ");
        }
    }
    printf("\nBest route distance: %.6f km\n", bestRoute.distance);
}

int main() {
    char filename[50];
    char startCityName[50];
    FILE *file;
    City cities[MAX_CITIES];
    int numCities = 0;
    int startCity = -1;

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
            startCity = numCities;
        }
        numCities++;
        if (numCities >= MAX_CITIES) {
            break;
        }
    }

    fclose(file);

    if (startCity == -1) {
        printf("Error: Starting city not found in the list.\n");
        return 1;
    }

    bfsTSP(cities, numCities, startCity);

    return 0;
}