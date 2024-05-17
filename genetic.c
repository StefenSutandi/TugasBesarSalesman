/*EL2208 Praktikum Pemecahan Masalah dengan C 2023/2024
*Hari dan Tanggal : Senin, 13 Mei 2024
*Nama (NIM)       : Afardhan Putra Fashadani (13222000)
*Asisten (NIM)    : Isnaini Azhar Ramadhan Wijaya (1832016)
*Nama File        : genetic.c
*Deskripsi        : Menyelesaikan TSP dengan Algoritma genetika
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define POP_SIZE 100
#define GEN_MAX 1000
#define MUTATION_RATE 0.01
#define PI 3.14159265358979323846
#define R 6371

typedef struct {
    char name[50];
    double latitude;
    double longitude;
} City;

typedef struct {
    int path[15]; // Asumsikan maksimum 15 kota
    double fitness;
} Individual;

City cities[15]; // Asumsikan maksimum 15 kota
int num_cities;
Individual population[POP_SIZE];
Individual new_population[POP_SIZE];

double toRadians(double degree) {
    return degree * (PI / 180);
}

double haversine(double lat1, double lon1, double lat2, double lon2) {
    double dLat = toRadians(lat2 - lat1);
    double dLon = toRadians(lon2 - lon1);
    lat1 = toRadians(lat1);
    lat2 = toRadians(lat2);

    double a = sin(dLat / 2) * sin(dLat / 2) +
               cos(lat1) * cos(lat2) *
               sin(dLon / 2) * sin(dLon / 2);
    return R * (2 * atan2(sqrt(a), sqrt(1 - a)));
}

double totalDistance(int path[]) {
    double distance = 0;
    for (int i = 0; i < num_cities - 1; i++) {
        int from = path[i];
        int to = path[i + 1];
        distance += haversine(cities[from].latitude, cities[from].longitude,
                              cities[to].latitude, cities[to].longitude);
    }
    distance += haversine(cities[path[num_cities - 1]].latitude, cities[path[num_cities - 1]].longitude,
                          cities[path[0]].latitude, cities[path[0]].longitude);
    return distance;
}

void initPopulation(int startCityIndex) {
    for (int i = 0; i < POP_SIZE; i++) {
        population[i].path[0] = startCityIndex;
        for (int j = 1, k = 0; k < num_cities; k++) {
            if (k != startCityIndex) population[i].path[j++] = k;
        }
        
        for (int j = 1; j < num_cities; j++) {
            int k = rand() % (num_cities - 1) + 1;
            int temp = population[i].path[j];
            population[i].path[j] = population[i].path[k];
            population[i].path[k] = temp;
        }
        
        population[i].fitness = totalDistance(population[i].path);
    }
}

void selection() {
    for (int i = 0; i < POP_SIZE; i++) {
        int a = rand() % POP_SIZE;
        int b = rand() % POP_SIZE;
        new_population[i] = population[a].fitness < population[b].fitness ? population[a] : population[b];
    }
}

void crossover(int parent1[], int parent2[], int child[]) {
    int start = rand() % (num_cities - 1) + 1;
    int end = rand() % (num_cities - 1) + 1;
    if (start > end) { int temp = start; start = end; end = temp; }

    int inChild[15] = {0}; // Asumsikan maksimum 15 kota
    for (int i = 0; i < num_cities; i++) child[i] = -1;
    for (int i = start; i <= end; i++) {
        child[i] = parent1[i];
        inChild[child[i]] = 1;
    }

    int currentIndex = 1;
    for (int i = 1; i < num_cities; i++) {
        if (!inChild[parent2[i]]) {
            while (child[currentIndex] != -1) currentIndex++;
            child[currentIndex] = parent2[i];
        }
    }

    child[0] = parent1[0];
}

void mutate(int path[]) {
    for (int i = 1; i < num_cities; i++) {
        if ((double)rand() / RAND_MAX < MUTATION_RATE) {
            int j = rand() % (num_cities - 1) + 1;
            int temp = path[i];
            path[i] = path[j];
            path[j] = temp;
        }
    }
}

void evolve() {
    selection();
    for (int i = 0; i < POP_SIZE; i += 2) {
        crossover(new_population[i].path, new_population[i + 1].path, population[i].path);
        crossover(new_population[i + 1].path, new_population[i].path, population[i + 1].path);
        mutate(population[i].path);
        mutate(population[i + 1].path);
        population[i].fitness = totalDistance(population[i].path);
        population[i + 1].fitness = totalDistance(population[i + 1].path);
    }
}

void loadCitiesFromFile(char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        exit(1);
    }

    char line[256];
    num_cities = 0;

    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^,],%lf,%lf", cities[num_cities].name, &cities[num_cities].latitude, &cities[num_cities].longitude);
        num_cities++;
    }
    fclose(file);
}

int findCityIndex(char *cityName) {
    for (int i = 0; i < num_cities; i++) {
        if (strcmp(cities[i].name, cityName) == 0) return i;
    }
    return -1;
}

int main() {
    srand(time(NULL));

    char filename[100], startCity[50];

    printf("Enter list of cities file name: ");
    scanf("%s", filename);
    printf("Enter starting point: ");
    scanf("%s", startCity);

    loadCitiesFromFile(filename);

    int startCityIndex = findCityIndex(startCity);
    if (startCityIndex == -1) {
        printf("Error: Starting city not found in the list.\n");
        return 1;
    }

    initPopulation(startCityIndex);

    clock_t start_time = clock();

    for (int generation = 0; generation < GEN_MAX; generation++) {
        evolve();
    }

    clock_t end_time = clock();
    double time_elapsed = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    Individual best = population[0];
    for (int i = 1; i < POP_SIZE; i++) {
        if (population[i].fitness < best.fitness) best = population[i];
    }

    printf("Best route found:\n");
    for (int i = 0; i < num_cities; i++) {
        printf("%s", cities[best.path[i]].name);
        if (i < num_cities - 1) printf(" -> ");
    }
    printf(" -> %s\n", cities[best.path[0]].name);
    printf("Best route distance: %lf km\n", best.fitness);
    printf("Time elapsed: %lf s\n", time_elapsed);

    return 0;
}
