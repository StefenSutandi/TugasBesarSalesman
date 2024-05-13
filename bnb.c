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

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

double route_distance(City cities[], int route[], int sum_cities) {
    double total_distance = 0.0;
    for (int i = 0; i < sum_cities - 1; i++) {
        total_distance += distance(cities[route[i]], cities[route[i + 1]]);
    }
    total_distance += distance(cities[route[sum_cities - 1]], cities[route[0]]);
    return total_distance;
}

void branch_and_bound(City cities[], int sum_cities, int start_point) {
    int *init_route = (int *)malloc(sum_cities * sizeof(int));
    for (int i = 0; i < sum_cities; i++) {
        init_route[i] = i;
    }
    
    Node init_node = {init_route, 0, 0.0};
    Node current_node = init_node;

    double min_distance = INT_MAX;
    int *best_route = (int *)malloc(sum_cities * sizeof(int));
    memcpy(best_route, init_route, sum_cities * sizeof(int));

    clock_t start = clock();

    while (current_node.level < sum_cities - 1) {
        for (int i = current_node.level + 1; i < sum_cities; i++) {
            swap(&current_node.route[current_node.level + 1], &current_node.route[i]);

            double current_cost = route_distance(cities, current_node.route, sum_cities);

            if (current_cost < min_distance) {
                memcpy(best_route, current_node.route, sum_cities * sizeof(int));
                min_distance = current_cost;
            }

            swap(&current_node.route[current_node.level + 1], &current_node.route[i]);
        }

        current_node.level++;
    }

    best_route[sum_cities] = start_point; 

    clock_t end = clock();
    double timeElapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Best route found: ");
    for (int i = 0; i <= sum_cities; i++) { // Iterasi hingga sum_cities (termasuk kembali ke kota awal)
        printf("%s", cities[best_route[i]].name);
        if (i < sum_cities) {
            printf(" -> ");
        }
    }
    printf("\nBest route distance: %.6f km\n", min_distance);
    printf("Time elapsed: %.10f s\n", timeElapsed);

    free(init_route);
    free(best_route);
}

int main() {
    char filename[50];
    char startCityName[50];
    FILE *file;
    City cities[15];
    int sum_cities = 0;
    int start_point = -1;

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

    branch_and_bound(cities, sum_cities, start_point);

    return 0;
}