#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    char nama[50];
    double koordinatLintang;
    double koordinatBujur;
} Kota;

Kota kota[] = {
    {"Jakarta", -6.2088, 106.8456},
    {"Surabaya", -7.2575, 112.7521},
    {"Bandung", -6.9175, 107.6191},
    {"Yogyakarta", -7.7956, 110.3695},
    {"Semarang", -6.9667, 110.4167},
    {"Medan", 3.5897, 98.6731},
    {"Makassar", -5.1477, 119.4327},
    {"Denpasar", -8.6500, 115.2167},
    {"Palembang", -2.9761, 104.7759},
    {"Balikpapan", -1.2675, 116.8289},
    {"Pekanbaru", 0.5071, 101.4476},
    {"Manado", 1.4748, 124.8426},
    {"Padang", -0.9492, 100.3543},
    {"Pontianak", -0.0226, 109.3307},
    {"Ambon", -3.6954, 128.1814},
    {"Jayapura", -2.5489, 140.7181},
    {"Mataram", -8.5833, 116.1167},
    {"Banda Aceh", 5.5480, 95.3190},
    {"Banjarmasin", -3.3199, 114.5907},
    {"Samarinda", -0.4916, 117.1454}
};

int jumlahKotaPerFile[] = {6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

int main() {
    srand(time(NULL));
    for (int i = 0; i < 10; i++) {
        char filename[20];
        sprintf(filename, "kota_%02d.csv", i + 1);
        int numCities = jumlahKotaPerFile[i];
        FILE *file = fopen(filename, "w");
        if (file == NULL) {
            printf("Gagal membuka file %s untuk ditulis.\n", filename);
            return 1;
        }
        for (int j = 0; j < numCities; j++) {
            int randomIndex = rand() % (sizeof(kota) / sizeof(kota[0]));
            Kota chosenCity = kota[randomIndex];
            fprintf(file, "%s,%.4lf,%.4lf\n", chosenCity.nama, chosenCity.koordinatLintang, chosenCity.koordinatBujur);
        }

        fclose(file);
        printf("File %s telah dibuat dengan %d kota.\n", filename, numCities);
    }

    return 0;
}