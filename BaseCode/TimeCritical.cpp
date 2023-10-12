#include "TimeCritical.h"
#include <iostream>

using namespace std;

// Calculates the Triangles Per Second (TPS) 
double calculateTPS(std::vector<TC_Model> &models, float totalTime) {
    double totalTriangles = 0;
    for (auto &model : models) {
        totalTriangles += model.triangleCount[model.LOD];
    }
    
    return totalTriangles / totalTime;
}

// Calculates the MaxCost based on TPS and FPS
double calculateMaxCost(double TPS, float FPS) {
    return TPS / FPS;
}

// Calculates the Benefit for each model based on clustering level, distance to viewpoint and object's bounding box diagonal
// Formula: Benefit = 1 - d / (2^L * D)
void calculateBenefits(std::vector<TC_Model> &models, const double *distances, const double *diagonals, int clusteringLevel) {
    for (size_t i = 0; i < models.size(); i++) {
        auto &model = models[i];
        double D = distances[i]; // distance between object and viewpoint
        double d = diagonals[i]; // diagonal of the object's bounding box
        int L = clusteringLevel; // clustering level
        model.benefit = 1.0 - d / (std::pow(2.0, L) * D);
        model.benefit = std::max(0.0, model.benefit); // clamp the benefit to a minimum of 0
    }
}

// Calculates the TotalCost and adjusts LODs if necessary
void adjustLODs(std::vector<TC_Model> &models, double maxCost) {
    // Reset totalCost to 0
    double totalCost = 0;

    // For every TC_Model, compute totalCost (sum all LODs' triangles)
    for (auto &model : models) {
        totalCost += model.triangleCount[model.LOD];
    }
    
    // While the total cost is greater than the max cost, find the model with the least benefit and reduce its LOD
    while (totalCost > maxCost) {
        double minBenefit = std::numeric_limits<double>::max();
        TC_Model* minBenefitModel = nullptr;

        for (auto &model : models) {
            if (model.benefit < minBenefit && model.LOD < 3) {
                minBenefit = model.benefit;
                minBenefitModel = &model;
            }
        }

        if (minBenefitModel) {
            totalCost -= minBenefitModel->triangleCount[minBenefitModel->LOD];
            minBenefitModel->LOD++;
            totalCost += minBenefitModel->triangleCount[minBenefitModel->LOD];
        } else {
            // No models can be further degraded. 
            break;
        }
    }
}
