/************************************************************************** 
    ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    Ismael Holguin  
    Operating system design assignment 1 
    Univeersity of Texas at El Paso
    Department of Computer Engineering 
    FALL 2022
    ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    Write a program that creates three processes to execute 
    the following three programs with command line arguments:
    1. “cat /proc/meminfo”
    2. “uname -a”
    3. “ls -l”
    and then waits for these three processes to complete before 
    creating a process to execute “echo 4th process“ and
    immediately printing “Goodbye!” and exiting; not waiting for   
    the last process to complete
*************************************************************************/
#include <sys/wait.h> 
#include <unistd.h>
#include <stdio.h>
int main()
{
// variable for the 4 processes
pid_t child[4]; 
char *argument1[] = {"cat","uname","ls", "echo"}; 
char *argument2[]= {"/proc/meminfo","-a","-l", "4th process"}; 
for(int i =0; i < 3; i++)
{ 
//fork a child
child[i] = fork();
//if child at i is negative, exit program
if(child[i] == -1)
{
printf("Error in fork()"); 
return -1;
}
else if(child[i] == 0)
//if the return value from fork 0, then execute program
execlp(argument1[i], argument1[i], argument2[i], NULL);
waitpid(child[i] ,NULL, 0); 
}
//fork last child
child[3] = fork(); 
//if negative an error has occured
if(child[3] < 0)
{
printf("Error in fork()"); 
return -1;
}
//if the last child pid is 0, then execute program
if( child[3] == 0)
execlp(argument1[3],argument1[3], argument2[3],NULL);
printf("Good bye!\n"); //dont wait for the last process to end
return 0;
}