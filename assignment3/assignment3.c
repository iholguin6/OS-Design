/*******************************************************************************
    Ismael Holguin
    Operating system design assignment 3
    Compute the catalan number and balance the computation between number
    of threads given by user
*****************************************************************************/
/*
    C_n+1 = (4n + 2)/(n+2) *Cn
*/


// to run: gcc -o a3 assignment3.c -lpthread

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_THREADS 4
#define MIN_THREADS 1
long double *numss = NULL;
long double compute_CatNum(int n)
{ 
     return n < 1 ? 1 : compute_CatNum(n - 1) * (4*n + 2)  / (n + 2);
}

void *Output_CatalanNumbers(void *argument)
{
    int *data = argument;   //type cast argument from void pointer to int
    
    /*simplest way to sort in ascending order is for each thread to write to designated files then read them 
    since the threads have load bounds */
   
    for(int i = data[1]; i < data[2]; i++) // data[1] = low bound data[2] = high bound
        numss[i]=compute_CatNum(i);
    pthread_exit(NULL); //kill the thread 
}
int main(int argc, char *argv[])
{
    if(argc <=2)
    { 
        printf("ERROR: expected three arguments (executable, nth, total_threads) \n");
        return -1;
    }
    int nth_nums = atoi(argv[1]); //convert     arr to integer 
    int num_Threads = atoi(argv[2]); //convert      arr to integer 
    FILE *file = fopen("catalan.dat", "w"); //create a file, open a file as "write"
    if(file == NULL)
    {
        printf("ERROR: On open file\n");
        return -1;             
    }
    numss = malloc(sizeof(long double)*nth_nums);
    // divides up the total terms by the total  threads, type cast to int to get the floor of the average
    int balance_load = (int)nth_nums/num_Threads;    
    printf("the average load for thread: %d\n",balance_load);
    int **arr =  (int**)malloc(sizeof(int*)*num_Threads);
    //bounds: thread,low,min
    for(int i=0; i<num_Threads; i++)
    {
        arr[i] = malloc(sizeof(int)*3);
    }
    //distribute the balance_load among the threads 
    for(int i = 0; i < num_Threads; i++){
        arr[i][0]= i; //[i] thread #, [0] thread #
        arr[i][1] = i * balance_load; //lower bound for thread i
        arr[i][2] = (i + 1) * balance_load; //upper bound for thread i
    }
    //set the last thread to count to the last number Max bound == nth number 
    arr[num_Threads - 1][2] = nth_nums;
    //create the threads, default attr, printCatalan function, and pass the structure 
    pthread_t *thread = (pthread_t *)malloc(sizeof(pthread_t)*num_Threads);
    pthread_attr_t attr; //pthread default attribute no idea why
    pthread_attr_init(&attr);
    //create the threads 
    for(int i = 0; i  < num_Threads; i++){
        // void pointer because just need a generic mem addy and typecast and pass the addresss of the elemenet i for       arr 
        pthread_create(&thread[i],&attr, Output_CatalanNumbers,(void*)arr[i]);
    }
    //wait for all threads to finish
    for(int i =0; i < num_Threads; i++)
        pthread_join(thread[i], NULL);
    //combine all files into 1
    FILE *my_file = fopen("catalan.dat","w");
    for(int i =0; i< nth_nums; i++)
    {
        fprintf(my_file,"index %d: %Lf\n",i+1, numss[i]);
    }
    fclose(my_file); //close the Caralan.Dat file 
    printf("Numbers transferred to catalan.dat\n");
    for(int i=0; i<num_Threads; i++)
    {
          free(arr[i]);
    }
    free(thread);
    free(numss);
    return 0;
}
