#include <stdlib.h>

#define HASH_SIZE 9
#define PREFIX_SIZE 1
const int SEARCH_COUNT = 1000;
const int hashesPerBucket = 256 * 1024;
const size_t hashesPerBucketRead = 256 * 256 * 32;

struct hashObject
{
    char byteArray[HASH_SIZE - PREFIX_SIZE];
    long int value;
};

void printArray(unsigned char byteArray[HASH_SIZE], int arraySize)
{
    printf("printArray(): ");
    for (size_t i = 0; i < arraySize; i++)
    {
        printf("%02x ", byteArray[i]); // Print each byte in hexadecimal format
    }
    printf("\n");
}
