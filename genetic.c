#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_CITIES 15
#define POP_SIZE 50
#define MAX_GENERATIONS 1000
#define MUTATION_RATE 0.01

typedef struct {
    char name[50];
    double latitude;
    double longitude;
} City;

typedef struct {
    int route[MAX_CITIES];
    double fitness;
} Individual;

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

void initializePopulation(Individual population[], int popSize, int numCities) {
    for (int i = 0; i < popSize; i++) {
        for (int j = 0; j < numCities; j++) {
            population[i].route[j] = j;
        }
        // Randomly shuffle the route (except the first city)
        for (int j = 1; j < numCities - 1; j++) {
            int k = j + rand() % (numCities - j);
            int temp = population[i].route[j];
            population[i].route[j] = population[i].route[k];
            population[i].route[k] = temp;
        }
        population[i].fitness = 0.0;
    }
}

double evaluateFitness(Individual *individual, City cities[], int numCities) {
    double totalDistance = 0.0;
    for (int i = 0; i < numCities - 1; i++) {
        int cityIndex1 = individual->route[i];
        int cityIndex2 = individual->route[i + 1];
        totalDistance += calculateDistance(cities[cityIndex1], cities[cityIndex2]);
    }
    // Add distance back to the starting city
    totalDistance += calculateDistance(cities[individual->route[numCities - 1]], cities[individual->route[0]]);
    individual->fitness = 1.0 / totalDistance; // Higher fitness for shorter route
    return totalDistance;
}

void crossover(Individual *parent1, Individual *parent2, Individual *child, int numCities) {
    // Perform Order Crossover (OX)
    int start = rand() % (numCities - 1);
    int end = start + rand() % (numCities - start);
    
    // Copy the segment from parent1 to child
    memcpy(child->route + start, parent1->route + start, (end - start + 1) * sizeof(int));
    
    // Mark used cities in the child's route
    int usedCities[numCities];
    memset(usedCities, 0, numCities * sizeof(int));
    for (int i = start; i <= end; i++) {
        usedCities[child->route[i]] = 1;
    }
    
    // Fill the rest of child's route with cities from parent2
    int index = 0;
    for (int i = 0; i < numCities; i++) {
        if (index == start) {
            index = end + 1;
        }
        if (!usedCities[parent2->route[i]]) {
            child->route[index++] = parent2->route[i];
        }
    }
}

void mutate(Individual *individual, int numCities) {
    // Apply Swap Mutation
    for (int i = 1; i < numCities - 1; i++) {
        if (rand() < RAND_MAX * MUTATION_RATE) {
            int j = i + 1 + rand() % (numCities - i - 1);
            int temp = individual->route[i];
            individual->route[i] = individual->route[j];
            individual->route[j] = temp;
        }
    }
}

void geneticAlgorithm(City cities[], int numCities, int startLocation) {
    srand(time(NULL));
    Individual population[POP_SIZE];
    
    // Initialize population
    initializePopulation(population, POP_SIZE, numCities);
    
    clock_t start = clock();
    double bestDistance = INFINITY;
    Individual *bestIndividual = NULL;

    for (int gen = 0; gen < MAX_GENERATIONS; gen++) {
        // Evaluate fitness of each individual
        for (int i = 0; i < POP_SIZE; i++) {
            evaluateFitness(&population[i], cities, numCities);
            if (population[i].fitness > 0 && 1.0 / population[i].fitness < bestDistance) {
                bestDistance = 1.0 / population[i].fitness;
                bestIndividual = &population[i];
            }
        }

        // Create next generation
        Individual nextGeneration[POP_SIZE];
        for (int i = 0; i < POP_SIZE; i++) {
            // Select parents using tournament selection
            Individual *parent1 = &population[rand() % POP_SIZE];
            Individual *parent2 = &population[rand() % POP_SIZE];
            
            // Crossover
            crossover(parent1, parent2, &nextGeneration[i], numCities);
            
            // Mutate
            mutate(&nextGeneration[i], numCities);
        }

        // Replace the old population with the new generation
        memcpy(population, nextGeneration, POP_SIZE * sizeof(Individual));
    }

    clock_t end = clock();
    double timeElapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Best route found: ");
    for (int i = 0; i < numCities; i++) {
        printf("%s", cities[bestIndividual->route[i]].name);
        if (i < numCities - 1) {
            printf(" -> ");
        }
    }
    
    // Print the starting point (Bandung) at the end of the route
    printf(" -> %s\n", cities[startLocation].name);

    printf("Best route distance: %.6f km\n", bestDistance);
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

    geneticAlgorithm(cities, numCities, startLocation);

    return 0;
}