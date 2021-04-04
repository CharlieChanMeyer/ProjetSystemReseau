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
#define NB_CALCULATOR 5 //Number of calculator threads
#define NB_LISTE 50     //Number of integer to Sum in each thread

/* monitor */
typedef struct {
  int      *a;                  //List of int to sum
  int     sum[NB_CALCULATOR];   //List of partialSum
  int     veclen;               //Size of vector to sum
  int timeStart[NB_CALCULATOR]; //List of threads start time 
  int timeExec[NB_CALCULATOR];  //List of threads execution time
  bool loop[NB_CALCULATOR+1];   //+1 with EvilMonkey
  int timeBeforeKill;           //Time before EvilMonkey action
} clc;


clc monitor; 
pthread_t callThd[NB_CALCULATOR+1];   //+1 with EvilMonkey
pthread_mutex_t mutexsum;
