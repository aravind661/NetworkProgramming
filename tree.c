/*

ALLAMNENI ARAVIND
2012C6PS661P
Undergrad @ BITS Pilani

Write a program tree.c for the following requirement.
1) Take input n on command-line.
2) Parent creates n  children at level1.
3) Each child in-turn creates n-1 children at level2. Each chile at level2 creates n-2 children at level3.This continues until n reaches 0.
4) Each child prints it level, its pid, parent's pid, its position i.e. 2 in if it is P2, and those many dots ended by new line.
Then it exits.
5) Every process waits until all its children are exited and only then it will print. Parent exits printing "\nAll Children Exited\n".  

Example output: Level  pid  ppid Position dots
                2     2300  2287 3  ...   1   
                2290  2286 5  ..... 1 ….  
Files Expected: tree.c and makefile to compile your program.

*/

//tree.c
#include<sys/types.h>
#include<unistd.h>
#include<sys/wait.h>
#include<stdio.h>
int main(int argc, char *argv[]){
    int n;
    n = atoi(argv[1]);
    printf("Level\tpid\tppid\tPosition\tdots\n");
    int i, j, status;
    j = n;
    pid_t pid, pid1;
    label:
    for(i=1; i<=j; i++){
        pid = fork();
        if(pid == 0){
            j = j-1;
            goto label;
        }else if(pid > 0){
            continue;
        }
    }
    while((pid1 = wait(&status))>0);
    //
    if(n == j)
        return;
        printf("\nAll Children Exited\n");
    printf("%d\t%d\t%d\t%d\t\t", n-j, getpid(), getppid(), i);
    int k;
    for(k = 0; k<i; k++){printf(".");}
    printf("\n");

}
