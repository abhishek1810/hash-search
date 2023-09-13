#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "blake3.h"

int main()
{
    const size_t num_hashes = 500000000; 
    const size_t data_length = 12;       // 12 bytes
    double total_time = 0;

    // Open the file for writing
    FILE *file = fopen("../hashes_1.txt", "w");
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    // Seed the random number generator
    srand(42);

    // start time

    for (int index = 0; index < 1; index++)
    {

        // Allocate memory for hash results
        uint8_t(*hash_results)[BLAKE3_OUT_LEN] = malloc(num_hashes * sizeof(*hash_results));
        if (hash_results == NULL)
        {
            perror("Error allocating memory");
            return 1;
        }

        clock_t start_time = clock();

        // Generate and hash random data
        for (size_t i = 0; i < num_hashes; i++)
        {
            uint8_t random_data[data_length];
            for (size_t j = 0; j < data_length; j++)
            {
                random_data[j] = rand() & 0xFF;
            }

            blake3_hasher hasher;
            blake3_hasher_init(&hasher);
            blake3_hasher_update(&hasher, random_data, data_length);
            blake3_hasher_finalize(&hasher, hash_results[i], 12);
            // printf("%02x", hash_results[i][j]);
            // if ( (i+1) % 250000000 == 0) {
            //     printf("Created 250M hashed");
            // }
        }
        // Record the end time
        clock_t end_time = clock();
        // Calculate the elapsed time in seconds
        double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

        total_time += elapsed_time;

        // Print the results
        printf("Elapsed time: %lf seconds\n", elapsed_time);

        start_time = clock();

        // Write the hash results to the file
        for (size_t i = 0; i < num_hashes; i++)
        {
            for (size_t j = 0; j < 12; j++)
            {
                fprintf(file, "%02x", hash_results[i][j]);
                // printf("%02x", hash_results[i][j]);
            }
            fprintf(file, " ");
            fprintf(file, "%010zu", i);
            // printf("%010zu", i);
            fprintf(file, "\n");
            // printf("\n");
            // if ( (i+1) % 250000000 == 0) {
            //     printf("Wrote 250M hashed");
            // }
        }
        // Record the end time
        end_time = clock();
        // Calculate the elapsed time in seconds
        elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

        total_time += elapsed_time;

        // Print the results
        printf("Elapsed time: %lf seconds\n", elapsed_time);

        // Free allocated memory
        free(hash_results);
    }

    printf("Total time: %lf seconds\n", total_time);

    // Close the file
    fclose(file);

    printf("Hashes written to hashes.txt\n");

    return 0;
}
