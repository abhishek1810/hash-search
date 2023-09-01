#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "blake3.h"

int main() {
    const size_t num_hashes = 1000;  // 100 Million
    const size_t data_length = 12;  // 12 bytes

    // Allocate memory for hash results
    uint8_t (*hash_results)[BLAKE3_OUT_LEN] = malloc(num_hashes * sizeof(*hash_results));
    if (hash_results == NULL) {
        perror("Error allocating memory");
        return 1;
    }

    // Seed the random number generator
    srand(42);

    // Generate and hash random data
    for (size_t i = 0; i < num_hashes; i++) {
        uint8_t random_data[data_length];
        for (size_t j = 0; j < data_length; j++) {
            random_data[j] = rand() & 0xFF;
        }

        blake3_hasher hasher;
        blake3_hasher_init(&hasher);
        blake3_hasher_update(&hasher, random_data, data_length);
        blake3_hasher_finalize(&hasher, hash_results[i], 12);
        // printf("%02x", hash_results[i][j]);
    }

    // Open the file for writing
    FILE *file = fopen("hashes.txt", "w");
    if (file == NULL) {
        perror("Error opening file");
        free(hash_results);  // Free allocated memory before exiting
        return 1;
    }

    // Write the hash results to the file
    for (size_t i = 0; i < num_hashes; i++) {
        for (size_t j = 0; j < 12; j++) {
            fprintf(file, "%02x", hash_results[i][j]);
            // printf("%02x", hash_results[i][j]);
        }
        fprintf(file, " ");
        fprintf(file, "%010zu", i);
        // printf("%010zu", i);
        fprintf(file, "\n");
        // printf("\n");
    }

    // Close the file
    fclose(file);

    // Free allocated memory
    free(hash_results);

    printf("Hashes written to hashes.txt\n");

    return 0;
}
