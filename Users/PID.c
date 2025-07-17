#include "PID.h"

#ifndef PID13
#define PID13
#define error 0
int calcu_tran[2];
float cal_time = CYCLE_TIME; // it should be better when set to 10 for PID
float calcu_result[2]={0,0};


struct PID PID_red={//PID运算结构体 结构体内容在pid.h里
    1.5,
    0,
    0,
    {0,0},
    {0,0},
    {0,0},
    1,
    {500,2500},
    {-1,-1}

};
struct PID PID_green={
    12.0,
    0.0005,
    4,
    {0,0},
    {0,0},
    {0,0},
    2,
    {500,2500},
    {1,1}
};



int * calcu(int *value,int *rotation,PID_ptr PID_data){
    int temp;

    if (value[1] == 0 && value[0] == 0)
        return 0;
    for (int i = 0; i <= 1; i++)
    {
        temp = value[i];
        if (temp >= -error && temp <= error)
            PID_data->PID_recent[i] = 0;
        else
            PID_data->PID_recent[i] = temp;

        
        calcu_result[i] = PID_data->Kp * PID_data->PID_recent[i] / cal_time + PID_data->Kd * (PID_data->PID_recent[i] - PID_data->PID_last[i]) / cal_time;
        if (temp >= -20 && temp <= 20)//积分运算
        {
            PID_data->PID_I_sum[i] += PID_data->PID_recent[i];
            if (PID_data->PID_I_sum[i] > 500)
                PID_data->PID_I_sum[i] = 500;
            calcu_result[i]+=PID_data->Ki *PID_data->PID_I_sum[i];
        }
        calcu_tran[i]=(int)calcu_result[i];


        if(i==0)//输出值取反 跟舵机旋转方向有关
            calcu_tran[i]*=PID_data->negative[0];
        if(i==1)
            calcu_tran[i]*=PID_data->negative[1];
        if(abs(value[1])<50&&abs(value[0])<50){//限速 如果距离过大将限速限制变小
            if(calcu_tran[i]>PID_data->max_speed)
                calcu_tran[i] = PID_data->max_speed;
            if (calcu_tran[i] < -PID_data->max_speed)
                calcu_tran[i] = -PID_data->max_speed;
        }else{
            int temp_speed=2;
            if(calcu_tran[i]>temp_speed)
                calcu_tran[i] = temp_speed;
            if (calcu_tran[i] < -temp_speed)
                calcu_tran[i] = -temp_speed;
        }
        
            PID_data->PID_last[i] = PID_data->PID_recent[i];
        if (rotation[i] + calcu_tran[i] > PID_data->max_rotate[1])//旋转角度限制
            rotation[i] = PID_data->max_rotate[1];
        else if (rotation[i] + calcu_tran[i] < PID_data->max_rotate[0])
            rotation[i] = PID_data->max_rotate[0];
        else
            rotation[i] += calcu_tran[i];
    }
    
}
#endif