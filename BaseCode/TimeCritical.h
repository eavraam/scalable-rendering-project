#ifndef TIME_CRITICAL_H
#define TIME_CRITICAL_H

#include <stdlib.h>
#include <cmath>
#include <limits>
#include <vector>

// Struct to represent a Model
struct TC_Model {
    int LOD;                // Level of Detail
    int triangleCount[4];   // Triangle count at each LOD
    double benefit;         // Benefit of this model
    double timeToRender = 1.0;    // Time needed to render the LOD
};

// Calculates the Triangles Per Second (TPS)
double calculateTPS(std::vector<TC_Model> &models, float totalTime);

// Calculates the MaxCost based on TPS and FPS
double calculateMaxCost(double TPS, float FPS);

// Calculates the Benefit for each model
void calculateBenefits(std::vector<TC_Model> &models);

// Calculates the TotalCost and adjusts LODs if necessary
void adjustLODs(std::vector<TC_Model> &models, double maxCost);

#endif //TIME_CRITICAL_H
