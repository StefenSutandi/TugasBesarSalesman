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

double deg_rad(double degrees) {
    return degrees * 3.14159265358979323846 / 180.0;
}

double distance(City city1, City city2) {
    double lat1 = deg_rad(city1.latitude);
    double lon1 = deg_rad(city1.longitude);
    double lat2 = deg_rad(city2.latitude);
    double lon2 = deg_rad(city2.longitude);

    double delta_lat = lat2 - lat1;
    double delta_lon = lon2 - lon1;

    double a = sin(delta_lat / 2) * sin(delta_lat / 2) +
               cos(lat1) * cos(lat2) *
               sin(delta_lon / 2) * sin(delta_lon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    double distance = 6371 * c;
    return distance;
}

void update_pheromones(double pheromones[15][15], int sum_cities, int route[], double total_distance) {
    // Evaporate pheromones
    for (int i = 0; i < sum_cities; i++) {
        for (int j = 0; j < sum_cities; j++) {
            if (i != j) {
                pheromones[i][j] *= (1.0 - 0.5);
                if (pheromones[i][j] < 0.0) {
                    pheromones[i][j] = 1.0;  // Prevent negative pheromone values
                }
            }
        }
    }

    // Deposit new pheromones
    for (int i = 0; i < sum_cities - 1; i++) {
        int city1 = route[i];
        int city2 = route[i + 1];
        pheromones[city1][city2] += (100 / total_distance);
        pheromones[city2][city1] += (100 / total_distance);
    }
}

void shortest_route(City cities[], int sum_cities, int start_point) {
    double pheromones[15][15];
    double distances[15][15];
    int best_route[15];
    double min_distance = INFINITY;
    int ant, i, j;

    // Initialize pheromone levels and distances
    for (i = 0; i < sum_cities; i++) {
        for (j = 0; j < sum_cities; j++) {
            if (i != j) {
                pheromones[i][j] = 1.0;  // Initialize pheromones to a small value
                distances[i][j] = distance(cities[i], cities[j]);
            } else {
                pheromones[i][j] = 0.0;  // No pheromone on same city
                distances[i][j] = 0.0;    // No distance between same city
            }
        }
    }

    clock_t start = clock();
    
    // ACO main loop
    for (ant = 0; ant < sum_cities; ant++) {
        int visited[15] = {0};
        int route[15];
        double total_distance = 0.0;

        // Ant's initial city
        int current_city = start_point;
        visited[current_city] = 1;
        route[0] = current_city;

        // Build the route for the current ant
        for (i = 1; i < sum_cities; i++) {
            double probabilities[15] = {0.0};
            double sum = 0.0;

            // Calculate probabilities for the next city
            for (j = 0; j < sum_cities; j++) {
                if (!visited[j]) {
                    probabilities[j] = pow(pheromones[current_city][j], 1) * pow(1.0 / distances[current_city][j], 5);
                    sum += probabilities[j];
                }
            }

            // Roulette wheel selection
            double roulette = (double)rand() / RAND_MAX * sum;
            double prob = 0.0;

            for (j = 0; j < sum_cities; j++) {
                if (!visited[j]) {
                    prob += probabilities[j];
                    if (prob >= roulette) {
                        current_city = j;
                        break;
                    }
                }
            }

            visited[current_city] = 1;
            route[i] = current_city;
            total_distance += distances[route[i - 1]][current_city];
        }

        total_distance += distances[route[sum_cities - 1]][start_point];

        // Update pheromone levels
        update_pheromones(pheromones, sum_cities, route, total_distance);

        // Check if the current ant found a better route
        if (total_distance < min_distance) {
            min_distance = total_distance;
            memcpy(best_route, route, sizeof(route));
        }
    }

    clock_t end = clock();
    double timeElapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Best route found: %s", cities[best_route[0]].name);
    for (i = 1; i < sum_cities; i++) {
        printf(" -> %s", cities[best_route[i]].name);
    }
    printf(" -> %s\n", cities[best_route[0]].name);  // Return to the starting point

    printf("Best route distance: %.6f km\n", min_distance);
    printf("Time elapsed: %.6f s\n", timeElapsed);
}

int main() {
    char filename[50];
    char startCityName[50];
    FILE *file;
    City cities[15];
    int sum_cities = 0;
    int start_point = -1;

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

    while (fscanf(file, "%[^,],%lf,%lf\n", cities[sum_cities].name, &cities[sum_cities].latitude, &cities[sum_cities].longitude) != EOF) {
        if (strcmp(cities[sum_cities].name, startCityName) == 0) {
            start_point = sum_cities;
        }
        sum_cities++;
        if (sum_cities >= 15) {
            break;
        }
    }

    fclose(file);

    if (start_point == -1) {
        printf("Error: Starting city not found in the list.\n");
        return 1;
    }

    shortest_route(cities, sum_cities, start_point);

    return 0;
}