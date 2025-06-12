/**
 * @file clamp.h
 * @brief Utility functions for clamping values within a specified range.
 *
 * Provides overloaded clamp functions for int and double types to restrict values to a given interval.
 */
#ifndef CLAMP_H
#define CLAMP_H

/**
 * @brief Clamps an integer value between lower and higher bounds.
 * @param value The value to clamp.
 * @param lower The lower bound.
 * @param higher The upper bound.
 * @return The clamped integer value.
 */
int clamp(int value, int lower, int higher);

/**
 * @brief Clamps a double value between lower and higher bounds.
 * @param value The value to clamp.
 * @param lower The lower bound.
 * @param higher The upper bound.
 * @return The clamped double value.
 */
double clamp(double value, double lower, double higher);

#endif