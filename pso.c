/*EL2208 Praktikum Pemecahan Masalah dengan C 2023/2024
*Hari dan Tanggal : Minggu, 12 Mei 2024
*Nama (NIM)       : Geraldo Valentino De Jesus (13222090)
*Asisten (NIM)    : Isnaini Azhar Ramadhan Wijaya (1832016)
*Nama File        : pso.c
*Deskripsi        : Menyelesaikan TSP dengan Algoritma Particle Swarm Optimization
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

typedef struct {
    int position[15];
    double fitness;
    int personal_best[15];
    double personal_fitness;
} Particle;

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

double route_distance(City cities[], int route[], int sum_cities) {
    double total_distance = 0.0;
    for (int i = 0; i < sum_cities - 1; i++) {
        int index_1 = route[i];
        int index_2 = route[i + 1];
        total_distance += distance(cities[index_1], cities[index_2]);
    }
    total_distance += distance(cities[route[sum_cities - 1]], cities[route[0]]);
    return total_distance;
}

void initialize_particles(Particle particles[], int sum_particles, int sum_cities, City cities[]) {
    for (int i = 0; i < sum_particles; i++) {
        for (int j = 0; j < sum_cities; j++) {
            particles[i].position[j] = j;
        }
        for (int j = 0; j < sum_cities; j++) {
            int temp = particles[i].position[j];
            int randomIndex = rand() % sum_cities;
            particles[i].position[j] = particles[i].position[randomIndex];
            particles[i].position[randomIndex] = temp;
        }
        particles[i].fitness = route_distance(cities, particles[i].position, sum_cities);
        memcpy(particles[i].personal_best, particles[i].position, sum_cities * sizeof(int));
        particles[i].personal_fitness = particles[i].fitness;
    }
}

void update_particle(Particle *particle, int global_best[], double global_fitness, int sum_cities, City cities[]) {
    int initial_position[15];
    memcpy(initial_position, particle->position, sum_cities * sizeof(int));

    for (int i = 0; i < sum_cities; i++) {
        double r1 = (double)rand() / RAND_MAX;
        double r2 = (double)rand() / RAND_MAX;

        particle->position[i] = global_best[i];  // Update position towards global best

        particle->fitness = route_distance(cities, particle->position, sum_cities);  // Recalculate fitness

        if (particle->fitness < particle->personal_fitness) {
            memcpy(particle->personal_best, particle->position, sum_cities * sizeof(int));
            particle->personal_fitness = particle->fitness;
        }

        memcpy(particle->position, initial_position, sum_cities * sizeof(int));  // Restore position
    }
}

void local_search(int route[], int sum_cities, City cities[]) {
    double current_distance = route_distance(cities, route, sum_cities);
    int improvement = 1;

    while (improvement) {
        improvement = 0;
        for (int i = 1; i < sum_cities - 1; i++) {
            for (int k = i + 1; k < sum_cities; k++) {
                // Apply 2-opt swap
                int temp = route[i];
                route[i] = route[k];
                route[k] = temp;

                double new_distance = route_distance(cities, route, sum_cities);
                if (new_distance < current_distance) {
                    current_distance = new_distance;
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

void shortest_route(City cities[], int sum_cities, const char start_city[]) {
    Particle particles[50];
    int global_route[15];
    double global_fitness = INFINITY;
    clock_t start, end;
    double timeElapsed;

    srand(time(NULL));

    initialize_particles(particles, 50, sum_cities, cities);

    start = clock();

    for (int iter = 0; iter < 1000; iter++) {
        // Update global best route and fitness
        for (int i = 0; i < 50; i++) {
            if (particles[i].fitness < global_fitness) {
                global_fitness = particles[i].fitness;
                memcpy(global_route, particles[i].position, sum_cities * sizeof(int));
            }
        }

        // Update particles' positions based on global best route
        for (int i = 0; i < 50; i++) {
            update_particle(&particles[i], global_route, global_fitness, sum_cities, cities);
        }
    }

    end = clock();
    timeElapsed = (double)(end - start) / CLOCKS_PER_SEC;

    // Apply 2-opt local search to further optimize the global best route
    local_search(global_route, sum_cities, cities);

    // Find the index of the starting city in the global best route
    int start_index = -1;
    for (int i = 0; i < sum_cities; i++) {
        if (strcmp(cities[global_route[i]].name, start_city) == 0) {
            start_index = i;
            break;
        }
    }

    // Construct the final best route starting and ending at the specified city
    int final_route[15];
    int route_index = 0;

    // Starting from the identified start index (excluding the duplicate start)
    for (int i = start_index; i < sum_cities + start_index; i++) {
        int index = i % sum_cities;
        if (strcmp(cities[global_route[index]].name, start_city) != 0) {
            final_route[route_index++] = global_route[index];
        }
    }

    // Include the starting city at the end of the final route
    final_route[route_index++] = global_route[start_index];

    // Print the final best route after local search
    printf("Best route found: %s -> ", start_city);
    for (int i = 0; i < route_index; i++) {
        printf("%s", cities[final_route[i]].name);
        if (i < route_index - 1) {
            printf(" -> ");
        }
    }
    printf("\n");

    // Calculate the distance of the final best route
    double best_route = route_distance(cities, final_route, route_index);

    printf("Best route distance: %.6f km\n", best_route);
    printf("Time elapsed: %.9f s\n", timeElapsed);
}

void init_cities(const char filename[], City cities[], int *sum_cities) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Cannot open file %s\n", filename);
        *sum_cities = 0;
        return;
    }

    *sum_cities = 0;
    char line[256];
    while (fgets(line, sizeof(line), file) && *sum_cities < 15) {
        char city_name[50];
        double latitude, longitude;
        if (sscanf(line, "%49[^,],%lf,%lf", city_name, &latitude, &longitude) == 3) {
            strcpy(cities[*sum_cities].name, city_name);
            cities[*sum_cities].latitude = latitude;
            cities[*sum_cities].longitude = longitude;
            (*sum_cities)++;
        }
    }

    fclose(file);
}

int main() {
    char filename[50];
    char start_city[50];
    City cities[15];
    int sum_cities = 0;

    printf("Enter list of cities file name: ");
    scanf("%s", filename);
    printf("Enter starting point: ");
    scanf("%s", start_city);

    init_cities(filename, cities, &sum_cities);

    if (sum_cities == 0) {
        printf("Error: No cities data available.\n");
        return 1;
    }

    shortest_route(cities, sum_cities, start_city);

    return 0;
}