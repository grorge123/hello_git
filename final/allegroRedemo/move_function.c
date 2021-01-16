#include "move_function.h"
#include <math.h>

float type_1_change_v_x(MovableObject now, double data[]){
    return (data[0] + cos(now.y / 10) * 30) - now.x;
}
float type_2_change_v_y(MovableObject now, double data[]){
    return (data[0] + cos(now.x / 10) * 30) - now.y;
}
float type_3_change_v_y(MovableObject now, double data[]){
    return (data[0] + cos(now.x / 10) * 30) - now.y;
}
float type_4_change_v_x(MovableObject now, double data[]){
    int mo = rand() % 100;
    mo = (mo == 0?100:0);
    if(now.x + data[0] * mo > SCREEN_W || now.x + data[0] * mo < 0)data[0] = (data[0]==1?-1:1);
    return data[0] * mo;
}
float type_6_change_v_x(MovableObject now, double data[]){
    if(now.x + data[0] < 0 || now.x + data[0] > SCREEN_W){
        data[0] *= -1;
        data[2]--;
    }
    return data[0];
}
float type_6_change_v_y(MovableObject now, double data[]){
    if(now.y + data[1] < 0 || now.y + data[1] > SCREEN_H){
        data[1] *= -1;
        data[3]--;
    }
    return data[1];
}
