#pragma once

/* include librairies */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <string>
#include <iostream>
#include <time.h>
#include <chrono>
#include <sys/types.h>

using namespace std;

//Define stuct and global variables
#define NB_CALCULATOR 5
#define NB_LISTE 50

/* monitor */
typedef struct {
  int      *a;
  int     sum[NB_CALCULATOR]; 
  int     veclen; 
  int timeExec[NB_CALCULATOR];
} clc;


clc monitor; 
pthread_t callThd[NB_CALCULATOR];
pthread_mutex_t mutexsum;
