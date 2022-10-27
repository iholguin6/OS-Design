/************************************************************************************************
    ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    Ismael Holguin  
    Operating system design assignment 2 
    Univeersity of Texas at El Paso
    Department of Computer Engineering 
        FALL 2022
    ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    Write a multi-threaded program that computes the Catalan number sequence 
    and writes those numbers in fixed point format to a file called “catalan.dat”.
    Your program should take two command line arguments: the first specifies the
    number of Catalan numbers to generate and the second specifies the number of 
    threads to use to compute the Catalan number sequence; you can assume a maximum 
    of four threads. Create a way to balance the load among the threads. 
 
    You must use the “long double” type to get the largest range possible; the Catalan numbers 
    grow very quickly. Output must be in ascending order (fixed point format) in the file 
    “catalan.dat”. 
************************************************************************************************/

/*
    C_n+1 = (4n + 2)/(n+2) *Cn
*/

/*
    Personal notes:
    to run: gcc -o a2 Assignment2.c -lpthread
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_THREADS 4
#define MIN_THREADS 1


long double compute_CatNum(int n)
{ 
     return n < 1 ? 1 : compute_CatNum(n - 1) * (4*n + 2)  / (n + 2);
}

void *Output_CatalanNumbers(void *argument)
{
    int *data = argument;   //type cast argument from void pointer to int
   
    //iterate from the low and high of the sequence
    char buffer[14];    //buffer for the file names
    
    /*simplest way to sort in ascending order is for each thread to write to designated files then read them 
    since the threads have load bounds */
    sprintf(buffer,"thread%i.dat",data[0]); //set thread # to file name
    FILE *threadFile = fopen(buffer,"w");   //create a file in write mode  
    for(int i = data[1]; i < data[2]; i++) // data[1] = low bound data[2] = high bound
        fprintf(threadFile,"%d: %Lf\n",i + 1,compute_CatNum(i));    //writes to the file offset by one to start at 1 
    fclose(threadFile); //close the thread file for housekeeping 
    pthread_exit(NULL); //kill the thread 
}

int main(int argc, char *argv[])
{
    if(argc <=2)
    { 
        printf("ERROR: expected three arguments (executable, nth, total_threads) \n");
        return -1;
    }
    int nth_nums = atoi(argv[1]); //convert array to integer 
    int num_Threads = atoi(argv[2]); //convert array to integer 
    FILE *file = fopen("catalan.dat", "w"); //create a file, open a file as "write"
    if(file == NULL)
    {
        printf("ERROR: On open file\n");
        return -1;             
    }

    //if total threads from input is not enough set total to min 
    if(num_Threads< MIN_THREADS) 
        num_Threads = MIN_THREADS;   
    
    //if total threads from input is not enough set total to max
    if(num_Threads > MAX_THREADS) 
        num_Threads = MAX_THREADS; 
    
    // divides up the total terms by the total  threads, type cast to int to get the floor of the average
    int balance_load = (int)nth_nums/num_Threads; 
    printf("the average load for thread: %d\n", balance_load);
    
    //create a 2D array of thread i and the [task,minBound,maxBound,] 
    int array[num_Threads][3];

    //distribute the balance_load among the threads 
    for(int i = 0; i < num_Threads; i++){
        array[i][0]= i; //[i] thread #, [0] thread #
        array[i][1] = i * balance_load; //lower bound for thread i
        array[i][2] = (i + 1) * balance_load; //upper bound for thread i
    }
    //set the last thread to count to the last number Max bound == nth number 
    array[num_Threads - 1][2] = nth_nums;
     //create the threads, default attr, printCatalan function, and pass the structure 
    pthread_t thread[MAX_THREADS]; // messed up should have been num_threads
    pthread_attr_t attr; //pthread default attribute no idea why
    pthread_attr_init(&attr);
    //create the threads 
    for(int i = 0; i  < num_Threads; i++)
        // void pointer because just need a generic mem addy and typecast and pass the addresss of the elemenet i for array 
        pthread_create(&thread[i],&attr, Output_CatalanNumbers,(void*)&array[i]);
    
    //wait for all threads to finish
    for(int i =0; i < num_Threads; i++)
        pthread_join(thread[i], NULL);

    //combine all files into 1
    for(int i =0; i < num_Threads; i++){
        char buffer[14];
        sprintf(buffer,"thread%i.dat",i); //write to buffer file name
        FILE *threadFile = fopen(buffer,"r"); //open file name as read file 
        if(threadFile == NULL){
            printf("Error: on opening the file\n");
            return -1; 
        }
        char c = fgetc(threadFile);  //get the character of the file 
        while (c != EOF) {  // keeps outputing until not End Of File 
            fputc(c, file);     //write to catalan file 
            c = fgetc(threadFile);  //get new character
        } 
        fclose(threadFile); //close the file 
    }
    fclose(file); //close the Caralan.Dat file 
    printf("Numbers transferred to catalan.dat\n");
    return 0;
}
