#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include "blake3.h"
#define HASH_SIZE 8
#define MAX_CHILDREN 256

const int BLOCK_SIZE =4;

// const int HASH_SIZE = 8;
//const int COUNT = 67108864;   //1gb file
const int COUNT = 1000;
//const int COUNT = 268435456;
const int nonce_size=8;

struct hashObject
{
    char byteArray[HASH_SIZE];
    long int nonce;
};

struct leaf
{
    struct hashObject* hash_block;
    int size_of_block_index;
    int* block_index;
    int hash_block_index;
};

struct child1
{
    struct leaf* children[MAX_CHILDREN];
};

struct root
{
    struct child1* children[MAX_CHILDREN];
};

struct hashObject *hashflushblock;
struct root *hashtree;
int arraySize=65536;

int main()
{

    //Print output to a file
    // FILE *fp;
    // fp = freopen("output.txt", "w", stdout);
    // if (fp == NULL) {
    // // handle error
    // }

    // do something that prints to stdout

    


    FILE *file=fopen("hashetree_1.bin","wb");

    struct timeval start_time, end_time, time_result;
    printf("Creating hash tree structure\n");
    gettimeofday(&start_time, NULL);
    hashtree = (struct root *)malloc(sizeof(struct root));
    if (hashtree == NULL)
    {
        perror("Error allocating memory : root");
        return 1;
    }
    //printf("Creating child \n");
    for (int i =0;i<MAX_CHILDREN;i++)
    {
        hashtree->children[i]=(struct child1 *)malloc(sizeof(struct child1));
        if (hashtree->children[i] == NULL)
        {
            perror("Error allocating memory : child 1");
            return 1;
        }
        //printf("Creating leaf\n");
        for (int j=0;j<256;j++)
        {
            hashtree->children[i]->children[j]=(struct leaf *)malloc(sizeof(struct leaf));
            if (hashtree->children[i]->children[j] == NULL)
            {
                perror("Error allocating memory: child 2");
                return 1;
            }
            //printf("Creating hash block\n");
            hashtree->children[i]->children[j]->size_of_block_index=0;
            //int size=hashtree->children[i]->children[j]->size_of_block_index;
            hashtree->children[i]->children[j]->block_index=malloc(0*(sizeof(int)));
            hashtree->children[i]->children[j]->hash_block=(struct hashObject*)malloc(BLOCK_SIZE * sizeof(struct hashObject));
            if (hashtree->children[i]->children[j]->hash_block == NULL)
            {
                perror("Error allocating memory: hash_block");
                return 1;
            }
            hashtree->children[i]->children[j]->hash_block_index=0;
            
            
        }
    }



    gettimeofday(&end_time, NULL);

    timersub(&end_time, &start_time, &time_result);
    printf("Elapsed time tree creation: %ld.%06ld\n", (long int) time_result.tv_sec, (long int) time_result.tv_usec);
    printf("No errors?\n");
    srand(time(NULL));

    gettimeofday(&start_time,NULL);
    printf("Generating Hashes\n");

    long block_offset=0;
    for (int i=0;i<COUNT;i++)
    {
        char random_data[8];
        for (size_t j = 0; j < HASH_SIZE; j++)
        {
            random_data[j] = rand() % 256;
            //random_data[j] = k % 256;
        }
        unsigned char byteArray[HASH_SIZE];
        //struct hashObject *hash=(struct hashObject*)malloc(sizeof(struct hashObject));
        // char byteArray[HASH_SIZE];
        blake3_hasher hasher;
        blake3_hasher_init(&hasher);
        blake3_hasher_update(&hasher, random_data, HASH_SIZE);
        blake3_hasher_finalize(&hasher,byteArray, HASH_SIZE);

        unsigned char bit1 = byteArray[0];
        unsigned char bit2 = byteArray[1];
        // // unsigned char bit3 = byteArray[2];
        int c=(hashtree->children[bit1]->children[bit2]->hash_block_index)+1;
        //hashtree->children[bit1]->children[bit2]->children[bit3]->hash_block[c].byteArray=byteArray;
        memcpy(hashtree->children[bit1]->children[bit2]->hash_block[c-1].byteArray,byteArray,sizeof(8));
        hashtree->children[bit1]->children[bit2]->hash_block->nonce= (long int)(i);

        hashtree->children[bit1]->children[bit2]->hash_block_index++;

        if(c==BLOCK_SIZE)
        {

            printf( "Writing to file - bit1 - %d, bit2 - %d, count - %d\n", bit1, bit2, c );
            size_t bytesWritten=fwrite(&hashtree->children[bit1]->children[bit2]->hash_block[0],1,(BLOCK_SIZE*sizeof(struct hashObject)),file);
            if (bytesWritten != (BLOCK_SIZE*sizeof(struct hashObject)))
            {
                perror("Error writing to file: while generating\n");
                fclose(file);
                return 1;
            }
            free(hashtree->children[bit1]->children[bit2]->hash_block);
            hashtree->children[bit1]->children[bit2]->hash_block_index=0;
            hashtree->children[bit1]->children[bit2]->hash_block=(struct hashObject*)malloc(BLOCK_SIZE * sizeof(struct hashObject));
            if (hashtree->children[bit1]->children[bit2]->hash_block == NULL)
            {
                perror("Error allocating memory: hash_block : Generating Hashes\n");
                return 1;
            }
            


            int newSize=hashtree->children[bit1]->children[bit2]->size_of_block_index+1;
            int *newArray = (int *)malloc(newSize * sizeof(int));

            for (int k=0;k<hashtree->children[bit1]->children[bit2]->size_of_block_index;k++)
            {
                newArray[k]=hashtree->children[bit1]->children[bit2]->block_index[k];
            }

            newArray[newSize-1]=block_offset;
            free(hashtree->children[bit1]->children[bit2]->block_index);
            hashtree->children[bit1]->children[bit2]->size_of_block_index=newSize;
            hashtree->children[bit1]->children[bit2]->block_index=newArray;
            block_offset++;

        }


    }

    for (int i=0;i<MAX_CHILDREN;i++)
    {
        for(int j=0;j<MAX_CHILDREN;j++)
        {
            if (hashtree->children[i]->children[j]->hash_block_index!=0)
            {
                printf( "Writing to file - bit1 - %d, bit2 - %d, count - %ld\n", i, j, block_offset );
                size_t bytesWritten=fwrite(&hashtree->children[i]->children[j]->hash_block[0],1,(BLOCK_SIZE*sizeof(struct hashObject)),file);
                if (bytesWritten != (BLOCK_SIZE*sizeof(struct hashObject)))
                {
                    perror("Error writing to file when dumping\n");
                    fclose(file);
                    return 1;
                }
                free(hashtree->children[i]->children[j]->hash_block);
                hashtree->children[i]->children[j]->hash_block_index=0;

                int newSize=hashtree->children[i]->children[j]->size_of_block_index+1;
                int *newArray = (int *)malloc(newSize * sizeof(int));

                for (int k=0;k<hashtree->children[i]->children[j]->size_of_block_index;k++)
                {
                    newArray[k]=hashtree->children[i]->children[j]->block_index[k];
                }

                newArray[newSize-1]=block_offset;
                free(hashtree->children[i]->children[j]->block_index);
                hashtree->children[i]->children[j]->size_of_block_index=newSize;
                hashtree->children[i]->children[j]->block_index=newArray;
                block_offset++;
            }
            
        }
    }

    gettimeofday(&end_time, NULL);

    timersub(&end_time, &start_time, &time_result);
    printf("Elapsed time hash Generation: %ld.%06ld\n", (long int) time_result.tv_sec, (long int) time_result.tv_usec);
    printf("No errors?\n");


    int array[arraySize];
    int one_d_array_index_counter = 0;

    for (int i=0;i<MAX_CHILDREN;i++)
    {
        for (int j =0;j<MAX_CHILDREN;j++)
        {
            array[one_d_array_index_counter]=hashtree->children[i]->children[j]->size_of_block_index;
            one_d_array_index_counter++;
        }
    }


    int** prefix_index = (int**)malloc(arraySize * sizeof(int*));
    for (int i = 0; i < arraySize; i++) {
        prefix_index[i] = (int*)malloc(array[i] * sizeof(int));
    }

    for (int i=0;i<MAX_CHILDREN;i++)
    {
        for (int j=0;j<MAX_CHILDREN;j++)
        {
            prefix_index[(i*MAX_CHILDREN)+j]=hashtree->children[i]->children[j]->block_index;
        }
    }

    // print prefix_index
    // for (int i =0;i<arraySize-1;i++)
    // {
    //     for (int j=0;j<array[i];j++)
    //     {
            
    //         printf("%d , ",prefix_index[i][j]);
            
    //     }

    //     printf("\n");

    // }

    int offset_start[arraySize];
    int sum=0;
    for (int i=0;i<arraySize;i++)
    {
        offset_start[i]=sum;
        sum+=array[i];
    }


    int count=0;
    long block_size=BLOCK_SIZE;
    int targetSum=block_offset-1;
    //Sorting algo I thought
    printf("Starting sorting\n");
    // struct timeval start_time, end_time, time_result;
    // gettimeofday(&start_time, NULL);
    int swap_count=targetSum;
    FILE *hash_file=fopen("random_data.bin","r+");
    char buffer[block_size];
    fread(buffer,block_size,1,hash_file);
    // fclose(hash_file);
    // FILE *hash_file=fopen("random_data.bin","r+");
    fseek(hash_file,0,SEEK_SET);
    for (int i=0;i<swap_count;i++)
    {
        if (i%100000==0)
        {  
            printf("%d\n",i);
            gettimeofday(&end_time, NULL);
            timersub(&end_time, &start_time, &time_result);
            printf("Elapsed time: %ld.%06ld\n", (long int) time_result.tv_sec,(long int) time_result.tv_usec);
        }
        long current_offset=ftell(hash_file)/block_size;
        int offset_start_index;
        int movement_from_offset_start_index;
        short flag=0;
        for (int j=0;j<arraySize;j++)
        {
            for (int k=0;k<array[j];k++)
            {
                if (prefix_index[j][k]==current_offset)
                {
                    offset_start_index=j;
                    movement_from_offset_start_index=k;
                    flag=1;
                    break;
                }       
                else if (prefix_index[j][k]>current_offset)
                {
                    flag=2;
                    break;
                }
            }
            if(flag==2)
            {
                continue;
            }
            if (flag==1)
            {
                break;
            }
        }

        int offset_to_be_moved_to=offset_start[offset_start_index]+movement_from_offset_start_index;
        char temporary[block_size];
        if(fseek(hash_file,(offset_to_be_moved_to*block_size),SEEK_SET)!=0)
            printf("Error 1");
        fread(temporary,block_size,1,hash_file);
        if(fseek(hash_file,(offset_to_be_moved_to*block_size),SEEK_SET)!=0)
            printf("Error 2");
        fwrite(buffer,(block_size),1,hash_file);
        memcpy(buffer,temporary,block_size);
        count++;
    }


    fclose(hash_file);
    gettimeofday(&end_time, NULL);
    
    printf("No. of swaps: %d\n",count);
    timersub(&end_time, &start_time, &time_result);
    printf("Elapsed time: %ld.%06ld\n", (long int) time_result.tv_sec,(long int) time_result.tv_usec);
    printf("No errors?\n");



    free(hashtree);
    fclose(file);
    //fclose(fp);

    return 0;
}
