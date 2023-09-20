#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "common.c"

bool DEBUG = false;

int main() {

    size_t numBuckets = (int)pow(2, PREFIX_SIZE * 8);
    const size_t numberOfBucketsToSort = (hashesPerBucketRead / numberOfHashesInBucket) * numBuckets;

    printf("Number of bucket : %zu\n", numBuckets);
    printf("Number of hashes per bucket to sort : %zu\n", numberOfHashesInBucket);
    printf("Number of bucket to sort : %zu\n", numberOfBucketsToSort);

    const size_t bucketSizeInBytes = numberOfHashesInBucket * sizeof(struct hashObject);
    struct hashObject *bucket = (struct hashObject *) malloc( bucketSizeInBytes );

    printf("fopen()...\n");

    FILE *file = fopen("plot.memo", "rb+"); // Open for appending
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    printf("Starting Sort.... \n");
    clock_t start_time = clock();

    for (size_t i = 0; i < numberOfBucketsToSort; i++)
    {
        //STEP 1 : Seek to location
        size_t bucket_location = i * bucketSizeInBytes;
        if (fseeko(file, bucket_location, SEEK_SET) != 0)
        {
            perror("Error seeking in file");
            fclose(file);
            return 1;
        }
        if (DEBUG) {
            printf("Bucket Location : %zu\n", bucket_location/(1024*1024));
        }

        //STEP 2 : Read the data from the file
        size_t bytesRead = fread(bucket, 1, bucketSizeInBytes, file);
        if (bytesRead != numberOfHashesInBucket * sizeof(struct hashObject))
        {
            perror("Error reading file");
            fclose(file);
            return 1;
        }

        //STEP 3 : Sort the bucket
        qsort(bucket, numberOfHashesInBucket, sizeof(struct hashObject), compareHashObjects);

        //STEP 4 : Seek to the start of the bucket
        if (fseeko(file, bucket_location, SEEK_SET) != 0)
        {
            perror("Error seeking in file");
            fclose(file);
            return 1;
        }
        if (DEBUG) {
            printf("Bucket Location : %zu\n", bucket_location/(1024*1024));
        }

        //STEP 5 : Update bucket in file
        size_t bytesWritten = fwrite(bucket, 1, bucketSizeInBytes, file);
        if (bytesWritten != bucketSizeInBytes)
        {
            perror("Error writing to file");
            fclose(file);
            return 1;
        }


        if ( i % 32 == 0 )
            printf("Buckets sorted : %zu\n", i);
    }

    printf("Ending Sort.... \n");
    clock_t end_time = clock();

    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Elapsed time: %lf seconds \n", elapsed_time);

    fclose(file);
    free(bucket);

    return 0;
}