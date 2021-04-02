#pragma once

/* include librairies */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <time.h>
#include "dotprod_mutex.hpp"
// #include "main.hpp"

void *partialSum(void *i);