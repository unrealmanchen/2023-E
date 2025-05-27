#pragma once
#ifndef PID_H
#define PID_H

#define milis_time 40

#include <string.h>
#include "main.h"
struct PID{
    float Kp;
    float Ki;
    float Kd;
    float PID_recent[2];
    int PID_last[2];
    int PID_I_sum[2];
    int max_speed;
    int max_rotate[2];
    int negative[2];
};

typedef struct PID * PID_ptr;
extern struct PID PID_green;
extern struct PID PID_red;
int * calcu(int *,int *,struct PID*);
void reset();

#endif

