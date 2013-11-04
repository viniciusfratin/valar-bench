#include "movingavg.h"

template <class T> 
T calculate_avg(T * ip, int N)
{
    float sum = 0.0f;
    for(int i = 0; i < N; i++)
        sum = sum + float(ip[i]);
    return (T)sum;
}

template <class T> 
T update_avg(T new_value, T old_value, float weight)
{

}
