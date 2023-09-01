#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 36
#define HASH_SIZE 24


// Function to perform binary search on the file
int binarySearch(FILE *file, const char *target) {
    fseek(file, 0, SEEK_END); // Move to the end of the file to get its size
    long fileSize = ftell(file);
    long recordSize = MAX_LINE_LENGTH; // Assuming fixed record size

    int low = 0;
    int high = (fileSize / recordSize) - 1;
    printf("%d\n", high);
    
    while (low <= high) {
        int mid = low + (high - low) / 2;
        long offset = mid * recordSize;
        
        char hash[HASH_SIZE+1];
        fseek(file, offset, SEEK_SET); // Move to the middle record
        char line[MAX_LINE_LENGTH];
        fgets(line, MAX_LINE_LENGTH, file); // Read the record
        
        // printf("%s", line);
        memcpy(hash, line, HASH_SIZE+1);
        hash[HASH_SIZE] = '\0';
        int cmp = strcmp(hash, target);
        
        if (cmp == 0) {
            return mid; // Found the record
        } else if (cmp < 0) {
            low = mid + 1; // Target is in the right half
        } else {
            high = mid - 1; // Target is in the left half
        }
    }
    
    return -1; // Record not found
}

int main() {
    FILE *file = fopen("sorted_hashes_1.txt", "r");
    if (file == NULL) {
        perror("Unable to open the file");
        return 1;
    }

    char target[HASH_SIZE] = "006295a77805d8da686a1328";
    // printf("Enter the record to search: ");
    // fgets(target, HASH_SIZE, stdin);
    int result = binarySearch(file, target);

    if (result != -1) {
        printf("Record found at line %d\n", result + 1); // Line numbers start from 1
    } else {
        printf("Record not found\n");
    }

    fclose(file);
    return 0;
}
