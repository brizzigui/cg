#include "clamp.h"

int clamp(int value, int lower, int higher)
{
    int clamped = value;
    if (value < lower)
    {
        clamped = lower;
    }
    else if (value > higher)
    {
        clamped = higher;
    }
    
    return clamped;
}

double clamp(double value, double lower, double higher)
{
    double clamped = value;
    if (value < lower)
    {
        clamped = lower;
    }
    else if (value > higher)
    {
        clamped = higher;
    }
    
    return clamped;
}