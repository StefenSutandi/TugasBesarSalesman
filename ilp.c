#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glpk.h>

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

    double distance = 6371 * c;
    return distance;
}

void findShortestRoute(City cities[], int numCities, int startLocation) {
    glp_prob *lp;
    int *ia, *ja;
    double *ar;
    int n, m, nnz, i, j, k;
    double minDistance = 0.0;

    // Create a linear programming problem
    lp = glp_create_prob();
    glp_set_prob_name(lp, "ShortestRoute");
    glp_set_obj_dir(lp, GLP_MIN);

    n = numCities;
    m = n * n;  // Maximum number of constraints
    nnz = n * (n - 1);  // Number of non-zero coefficients

    ia = (int *)malloc((1 + nnz) * sizeof(int));
    ja = (int *)malloc((1 + nnz) * sizeof(int));
    ar = (double *)malloc((1 + nnz) * sizeof(double));

    k = 1;
    // Objective function: minimize total distance
    for (i = 1; i <= n; i++) {
        for (j = 1; j <= n; j++) {
            if (i != j) {
                ia[k] = 1;
                ja[k] = (i - 1) * n + j;
                ar[k] = calculateDistance(cities[i - 1], cities[j - 1]);
                k++;
            }
        }
    }

    // Load the LP problem data
    glp_load_matrix(lp, nnz, ia, ja, ar);
    free(ia);
    free(ja);
    free(ar);

    // Add constraints: each city is visited exactly once
    for (i = 1; i <= n; i++) {
        for (j = 1; j <= n; j++) {
            if (i != j) {
                glp_set_row_bnds(lp, (i - 1) * n + j, GLP_FX, 1.0, 1.0); // Each row is a constraint
            }
        }
    }

    // Solve the LP problem
    glp_simplex(lp, NULL);

    // Retrieve the solution
    minDistance = glp_get_obj_val(lp);

    // Clean up
    glp_delete_prob(lp);
    glp_free_env();

    printf("Best route distance: %.6f km\n", minDistance);
}

int main() {
    char filename[50];
    char startCityName[50];
    FILE *file;
    City cities[15];
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
        if (numCities >= 15) {
            break;
        }
    }

    fclose(file);

    if (startLocation == -1) {
        printf("Error: Starting city not found in the list.\n");
        return 1;
    }

    findShortestRoute(cities, numCities, startLocation);

    return 0;
}
