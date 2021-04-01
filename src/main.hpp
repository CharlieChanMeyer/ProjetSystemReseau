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
#include "monitor.hpp"
/* define global variable*/
#define NB_CALCULATOR 3
#define NB_LISTE 15

using namespace std;

typedef struct Calculator {
    string name;
    int timeExec;
    int partialSum;
    int list[NB_LISTE/NB_CALCULATOR];
} clc;

static clc monitor[NB_CALCULATOR];

void static report(clc monitor[]);

int main(void);
