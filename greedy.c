/*EL2208 Praktikum Pemecahan Masalah dengan C 2023/2024
*Hari dan Tanggal : Senin, 13 Mei 2024
*Nama (NIM)       : Stefen Sutandi (13222091)
*Asisten (NIM)    : Isnaini Azhar Ramadhan Wijaya (1832016)
*Nama File        : bnb.c
*Deskripsi        : Menyelesaikan TSP dengan Algoritma Greedy
*/

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

void shortest_route(City cities[], int sum_cities, int start_point) {
    int visited[15] = {0};
    int route[15];
    double min_distance = 0.0;

    visited[start_point] = 1;
    route[0] = start_point;

    clock_t start = clock();
    int current_city = start_point;
    for (int i = 1; i < sum_cities; i++) {
        double min_dist = INFINITY;
        int next_city = -1;

        // Cari kota berikutnya yang belum dikunjungi dengan jarak terdekat
        for (int j = 0; j < sum_cities; j++) {
            if (!visited[j]) {
                double dist = distance(cities[current_city], cities[j]);
                if (dist < min_dist) {
                    min_dist = dist;
                    next_city = j;
                }
            }
        }

        route[i] = next_city;
        visited[next_city] = 1;
        current_city = next_city;
        min_distance += min_dist;
    }

    // Kembali ke kota awal
    min_distance += distance(cities[route[sum_cities - 1]], cities[start_point]);

    clock_t end = clock();
    double timeElapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Best route found: %s -> ", cities[start_point].name);
    for (int i = 1; i < sum_cities; i++) {
        printf("%s", cities[route[i]].name);
        if (i < sum_cities - 1) {
            printf(" -> ");
        }
    }
    printf(" -> %s\n", cities[start_point].name);

    printf("Best route distance: %.6f km\n", min_distance);
    printf("Time elapsed: %.10f s\n", timeElapsed);
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

    shortest_route(cities, sum_cities, start_point);

    return 0;
}