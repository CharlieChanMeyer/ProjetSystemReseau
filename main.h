#ifndef __MAIN_H__
#define __MAIN_H__

/* include librairies */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

typedef struct Calculator {
    char name[30];
    int timeExec;
    int partialSum;
} clc;



clc monitor[];

#endif