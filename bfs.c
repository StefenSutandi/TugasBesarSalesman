/*EL2208 Praktikum Pemecahan Masalah dengan C 2023/2024
*Hari dan Tanggal : Senin, 13 Mei 2024
*Nama (NIM)       : Stefen Sutandi (13222091)
*Asisten (NIM)    : Isnaini Azhar Ramadhan Wijaya (1832016)
*Nama File        : bnb.c
*Deskripsi        : Menyelesaikan TSP dengan Algoritma Branch and Bound
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>

typedef struct {
    char name[50];
    double latitude;
    double longitude;
    int numNeighbors;
    int neighbors[15]; // Array of neighbor indices
    double distances[15]; // Array of distances to neighbors
} City;

typedef struct {
    int toCityIndex;
    double distance;
} Edge;

// Queue implementation for BFS
typedef struct {
    int items[15];
    int front;
    int rear;
} Queue;

void initQueue(Queue *q) {
    q->front = -1;
    q->rear = -1;
}

bool isEmpty(Queue *q) {
    return (q->front == -1 && q->rear == -1);
}

void enqueue(Queue *q, int item) {
    if (isEmpty(q)) {
        q->front = 0;
        q->rear = 0;
    } else {
        q->rear = (q->rear + 1) % 15;
    }
    q->items[q->rear] = item;
}

int dequeue(Queue *q) {
    int item = q->items[q->front];
    if (q->front == q->rear) {
        q->front = -1;
        q->rear = -1;
    } else {
        q->front = (q->front + 1) % 15;
    }
    return item;
}

double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    // Convert degrees to radians
    double lat1_rad = lat1 * 3.14159265358979323846 / 180.0;
    double lon1_rad = lon1 * 3.14159265358979323846 / 180.0;
    double lat2_rad = lat2 * 3.14159265358979323846 / 180.0;
    double lon2_rad = lon2 * 3.14159265358979323846 / 180.0;

    // Haversine formula
    double dlon = lon2_rad - lon1_rad;
    double dlat = lat2_rad - lat1_rad;
    double a = pow(sin(dlat / 2), 2) + cos(lat1_rad) * cos(lat2_rad) * pow(sin(dlon / 2), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double distance = 6371 * c; // Radius of Earth in km

    return distance;
}

void bfsShortestPath(City cities[], int numCities, const char startCityName[]) {
    int startLocation = -1;
    for (int i = 0; i < numCities; i++) {
        if (strcmp(cities[i].name, startCityName) == 0) {
            startLocation = i;
            break;
        }
    }

    if (startLocation == -1) {
        printf("Error: Starting city '%s' not found in the list.\n", startCityName);
        return;
    }

    Queue q;
    initQueue(&q);
    enqueue(&q, startLocation);

    double distances[15];
    for (int i = 0; i < numCities; i++) {
        distances[i] = INFINITY;
    }
    distances[startLocation] = 0;

    int previous[15];
    memset(previous, -1, sizeof(previous));

    while (!isEmpty(&q)) {
        int current = dequeue(&q);

        for (int i = 0; i < cities[current].numNeighbors; i++) {
            int neighborIndex = cities[current].neighbors[i];
            double dist = cities[current].distances[i];
            
            if (distances[current] + dist < distances[neighborIndex]) {
                distances[neighborIndex] = distances[current] + dist;
                previous[neighborIndex] = current;
                enqueue(&q, neighborIndex);
            }
        }
    }

    // Print the best route found
    printf("Best route found: %s -> ", cities[startLocation].name);
    int current = startLocation;
    while (current != -1) {
        printf("%s", cities[current].name);
        current = previous[current];
        if (current != -1) {
            printf(" -> ");
        }
    }
    printf("\n");

    // Print the best route distance
    printf("Best route distance: %.6f km\n", distances[startLocation]);
}

int main() {
    char filename[50];
    char startCityName[50];
    FILE *file;
    City cities[15];
    int numCities = 0;

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
        numCities++;
        if (numCities >= 15) {
            break;
        }
    }

    fclose(file);

    bfsShortestPath(cities, numCities, startCityName);

    return 0;
}