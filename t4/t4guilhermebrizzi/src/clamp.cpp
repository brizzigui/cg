/**
 * @file clamp.cpp
 * @brief Implementation of utility clamp functions for value restriction.
 *
 * Provides the implementation for clamping integer and double values within a specified range.
 */
#include "clamp.h"

/**
 * @brief Clamps an integer value between lower and higher bounds.
 * @param value The value to clamp.
 * @param lower The lower bound.
 * @param higher The upper bound.
 * @return The clamped integer value.
 */
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

/**
 * @brief Clamps a double value between lower and higher bounds.
 * @param value The value to clamp.
 * @param lower The lower bound.
 * @param higher The upper bound.
 * @return The clamped double value.
 */
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