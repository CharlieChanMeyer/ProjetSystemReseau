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
  int timeStart[NB_CALCULATOR];
  int timeExec[NB_CALCULATOR];
  bool loop[NB_CALCULATOR+1];
  int timeBeforeKill;
} clc;


clc monitor; 
pthread_t callThd[NB_CALCULATOR+1];
pthread_mutex_t mutexsum;
