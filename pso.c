#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

typedef struct {
    int position[15];
    double fitness;
    int personalBest[15];
    double personalBestFitness;
} Particle;

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

    double distance = 6371 * c;  // Earth's radius in kilometers
    return distance;
}

double calculateRouteDistance(City cities[], int route[], int numCities) {
    double totalDistance = 0.0;
    for (int i = 0; i < numCities - 1; i++) {
        int cityIndex1 = route[i];
        int cityIndex2 = route[i + 1];
        totalDistance += calculateDistance(cities[cityIndex1], cities[cityIndex2]);
    }
    // Add distance back to the starting city
    totalDistance += calculateDistance(cities[route[numCities - 1]], cities[route[0]]);
    return totalDistance;
}

void initializeParticles(Particle particles[], int numParticles, int numCities, City cities[]) {
    for (int i = 0; i < numParticles; i++) {
        for (int j = 0; j < numCities; j++) {
            particles[i].position[j] = j;
        }
        for (int j = 0; j < numCities; j++) {
            int temp = particles[i].position[j];
            int randomIndex = rand() % numCities;
            particles[i].position[j] = particles[i].position[randomIndex];
            particles[i].position[randomIndex] = temp;
        }
        particles[i].fitness = calculateRouteDistance(cities, particles[i].position, numCities);
        memcpy(particles[i].personalBest, particles[i].position, numCities * sizeof(int));
        particles[i].personalBestFitness = particles[i].fitness;
    }
}

void updateParticle(Particle *particle, int globalBest[], double globalBestFitness, int numCities, City cities[]) {
    int initialPosition[15];
    memcpy(initialPosition, particle->position, numCities * sizeof(int));

    for (int i = 0; i < numCities; i++) {
        double r1 = (double)rand() / RAND_MAX;
        double r2 = (double)rand() / RAND_MAX;

        particle->position[i] = globalBest[i];  // Update position towards global best

        particle->fitness = calculateRouteDistance(cities, particle->position, numCities);  // Recalculate fitness

        if (particle->fitness < particle->personalBestFitness) {
            memcpy(particle->personalBest, particle->position, numCities * sizeof(int));
            particle->personalBestFitness = particle->fitness;
        }

        memcpy(particle->position, initialPosition, numCities * sizeof(int));  // Restore position
    }
}

void localSearch2Opt(int route[], int numCities, City cities[]) {
    double currentDistance = calculateRouteDistance(cities, route, numCities);
    int improvement = 1;

    while (improvement) {
        improvement = 0;
        for (int i = 1; i < numCities - 1; i++) {
            for (int k = i + 1; k < numCities; k++) {
                // Apply 2-opt swap
                int temp = route[i];
                route[i] = route[k];
                route[k] = temp;

                double newDistance = calculateRouteDistance(cities, route, numCities);
                if (newDistance < currentDistance) {
                    currentDistance = newDistance;
                    improvement = 1;
                } else {
                    // Revert the swap
                    temp = route[i];
                    route[i] = route[k];
                    route[k] = temp;
                }
            }
        }
    }
}

void findShortestRoutePSO(City cities[], int numCities, const char startCityName[]) {
    Particle particles[50];
    int globalBestRoute[15];
    double globalBestFitness = INFINITY;
    clock_t start, end;
    double timeElapsed;

    srand(time(NULL));

    initializeParticles(particles, 50, numCities, cities);

    start = clock();

    for (int iter = 0; iter < 1000; iter++) {
        // Update global best route and fitness
        for (int i = 0; i < 50; i++) {
            if (particles[i].fitness < globalBestFitness) {
                globalBestFitness = particles[i].fitness;
                memcpy(globalBestRoute, particles[i].position, numCities * sizeof(int));
            }
        }

        // Update particles' positions based on global best route
        for (int i = 0; i < 50; i++) {
            updateParticle(&particles[i], globalBestRoute, globalBestFitness, numCities, cities);
        }
    }

    end = clock();
    timeElapsed = (double)(end - start) / CLOCKS_PER_SEC;

    // Apply 2-opt local search to further optimize the global best route
    localSearch2Opt(globalBestRoute, numCities, cities);

    // Find the index of the starting city in the global best route
    int startIndex = -1;
    for (int i = 0; i < numCities; i++) {
        if (strcmp(cities[globalBestRoute[i]].name, startCityName) == 0) {
            startIndex = i;
            break;
        }
    }

    // Construct the final best route starting and ending at the specified city
    int finalRoute[15];
    int routeIndex = 0;

    // Starting from the identified start index (excluding the duplicate start)
    for (int i = startIndex; i < numCities + startIndex; i++) {
        int index = i % numCities;
        if (strcmp(cities[globalBestRoute[index]].name, startCityName) != 0) {
            finalRoute[routeIndex++] = globalBestRoute[index];
        }
    }

    // Include the starting city at the end of the final route
    finalRoute[routeIndex++] = globalBestRoute[startIndex];

    // Print the final best route after local search
    printf("Best route found: %s -> ", startCityName);
    for (int i = 0; i < routeIndex; i++) {
        printf("%s", cities[finalRoute[i]].name);
        if (i < routeIndex - 1) {
            printf(" -> ");
        }
    }
    printf("\n");

    // Calculate the distance of the final best route
    double bestRouteDistance = calculateRouteDistance(cities, finalRoute, routeIndex);

    printf("Best route distance: %.6f km\n", bestRouteDistance);
    printf("Time elapsed: %.9f s\n", timeElapsed);
}

void initializeCities(const char filename[], City cities[], int *numCities) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Cannot open file %s\n", filename);
        *numCities = 0;
        return;
    }

    *numCities = 0;
    char line[256];
    while (fgets(line, sizeof(line), file) && *numCities < 15) {
        char cityName[50];
        double latitude, longitude;
        if (sscanf(line, "%49[^,],%lf,%lf", cityName, &latitude, &longitude) == 3) {
            strcpy(cities[*numCities].name, cityName);
            cities[*numCities].latitude = latitude;
            cities[*numCities].longitude = longitude;
            (*numCities)++;
        }
    }

    fclose(file);
}

int main() {
    char filename[50];
    char startCityName[50];
    City cities[15];
    int numCities = 0;

    printf("Enter list of cities file name: ");
    scanf("%s", filename);
    printf("Enter starting point: ");
    scanf("%s", startCityName);

    initializeCities(filename, cities, &numCities);

    if (numCities == 0) {
        printf("Error: No cities data available.\n");
        return 1;
    }

    findShortestRoutePSO(cities, numCities, startCityName);

    return 0;
}