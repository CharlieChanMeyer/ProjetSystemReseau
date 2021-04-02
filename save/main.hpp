#pragma once

/* include librairies */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string>
#include <iostream>
#include <time.h>
#include <chrono>
#include <sys/types.h>
#include "calculator.hpp"
/* define global variable*/
#define NB_CALCULATOR 3
#define NB_LISTE 15

using namespace std;

typedef struct Calculator {
    string name;
    int timeExec[NB_CALCULATOR];
    int partialSum;
    int *list;
} clc;

static clc monitor[NB_CALCULATOR];
static pthread_t callThd[NB_CALCULATOR];
static pthread_mutex_t mutexsum;

void static report(clc monitor[]);

int main(void);
