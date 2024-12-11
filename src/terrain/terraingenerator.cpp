#include "terraingenerator.h"
#include "coral.h"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "../models/model-loader.h"
#include "glm/gtx/transform.hpp"
#include "tallcoral.h"
#include "braincoral.h"
#include "genericcoral.h"
#include <cmath>
#include <iostream>
#include <random>

TerrainGenerator::TerrainGenerator() {
    tallCoralData = modelloader::LoadGLBVerticesNormals("../../src/models/3d-models/tall_coral.glb");
    brainCoralData = modelloader::LoadGLBVerticesNormals("../../src/models/3d-models/brain_coral.glb");
    genericCoralData = modelloader::LoadGLBVerticesNormals("../../src/models/3d-models/generic_coral.glb");
}

glm::mat4 getCTM(float xTransform, float zTransform) {
    glm::mat4 ctm = glm::mat4(1.0f);

    // Translation: Move the coral to (xTransform, -0.4, zTransform)
    ctm = glm::translate(ctm, glm::vec3(xTransform, -0.4f, zTransform));

    // Rotation: Rotate randomly around Y-axis for natural orientation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> rotationDist(0.0f, 360.0f);
    float rotationY = glm::radians(rotationDist(gen));
    ctm = glm::rotate(ctm, rotationY, glm::vec3(0.0f, 1.0f, 0.0f));

    // Slight tilts around X and Z axes for natural variation
    std::uniform_real_distribution<float> tiltDist(-10.0f, 10.0f);
    float tiltX = glm::radians(tiltDist(gen));
    float tiltZ = glm::radians(tiltDist(gen));
    ctm = glm::rotate(ctm, tiltX, glm::vec3(1.0f, 0.0f, 0.0f));
    ctm = glm::rotate(ctm, tiltZ, glm::vec3(0.0f, 0.0f, 1.0f));

    // Scaling
    ctm = glm::scale(ctm, glm::vec3(5.0f, 5.0f, 5.0f));

    return ctm;
}
std::vector<glm::vec2> generateClusterCenters(float width, float height, int numClusters, float minDistance, int maxAttempts) {
    std::vector<glm::vec2> centers;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distX(-width / 2.0f + minDistance, width / 2.0f - minDistance);
    std::uniform_real_distribution<float> distY(-height / 2.0f + minDistance, height / 2.0f - minDistance);

    int attempts = 0;
    while (centers.size() < numClusters && attempts < maxAttempts) {
        glm::vec2 candidate(distX(gen), distY(gen));
        bool valid = true;
        for (const auto& center : centers) {
            float distance = glm::distance(candidate, center);
            if (distance < minDistance) {
                valid = false;
                break;
            }
        }
        if (valid) {
            centers.push_back(candidate);
        }
        attempts++;
    }

    if (centers.size() < numClusters) {
        std::cerr << "Warning: Only placed " << centers.size() << " out of " << numClusters
                  << " clusters due to space constraints.\n";
    }

    return centers;
}

std::vector<glm::vec2> getPoissonDiscSamplesInCluster(glm::vec2 center, float clusterRadius, float coralRadius, int numSamples) {
    float width = clusterRadius * 2.0f;
    float height = clusterRadius * 2.0f;
    float radius = coralRadius;

    float cellSize = radius / std::sqrt(2.0f);
    int numCols = std::ceil(width / cellSize);
    int numRows = std::ceil(height / cellSize);
    std::vector<int> grid(numRows * numCols, -1);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> distX(0.0f, width);
    std::uniform_real_distribution<double> distY(0.0f, height);
    std::uniform_real_distribution<double> distRadius(radius, 2.0f * radius);
    std::uniform_real_distribution<double> distAngle(0.0, 2.0 * M_PI);

    auto randActiveIndex = [&](int activeSize) -> int {
        std::uniform_int_distribution<int> distIndex(0, activeSize - 1);
        return distIndex(gen);
    };

    auto getCell = [&](float x, float y) -> glm::vec2 {
        return glm::vec2(int(x / cellSize), int(y / cellSize));
    };

    auto isValid = [&](float x, float y, const std::vector<glm::vec2>& points) -> bool {
        if (x < 0.0f || x >= width || y < 0.0f || y >= height) {
            return false;
        }

        glm::vec2 cell = getCell(x, y);

        int startX = std::max(int(cell.x) - 2, 0);
        int endX   = std::min(int(cell.x) + 2, numCols - 1);
        int startY = std::max(int(cell.y) - 2, 0);
        int endY   = std::min(int(cell.y) + 2, numRows - 1);

        for (int ny = startY; ny <= endY; ++ny) {
            for (int nx = startX; nx <= endX; ++nx) {
                int idx = grid[ny * numCols + nx];
                if (idx != -1) {
                    double dx = x - points[idx].x;
                    double dy = y - points[idx].y;
                    if ((dx * dx + dy * dy) < (radius * radius)) {
                        return false;
                    }
                }
            }
        }

        return true;
    };

    std::vector<glm::vec2> points;
    glm::vec2 initialPoint(distX(gen), distY(gen));
    points.push_back(initialPoint);

    glm::vec2 initialCell = getCell(initialPoint.x, initialPoint.y);
    grid[int(initialCell.y) * numCols + int(initialCell.x)] = 0;

    std::vector<int> active = {0};

    while (!active.empty()) {
        int i = active[randActiveIndex(active.size())];
        glm::vec2 randomPoint = points[i];

        bool found = false;
        for (int attempt = 0; attempt < numSamples; attempt++) {
            double angle = distAngle(gen);
            double rad = distRadius(gen);

            double xNew = randomPoint.x + rad * std::cos(angle);
            double yNew = randomPoint.y + rad * std::sin(angle);

            if (isValid(xNew, yNew, points)) {
                glm::vec2 newPoint(xNew, yNew);
                int newIndex = static_cast<int>(points.size());
                points.push_back(newPoint);

                glm::vec2 newCell = getCell(xNew, yNew);
                grid[int(newCell.y) * numCols + int(newCell.x)] = newIndex;

                active.push_back(newIndex);

                found = true;
                break;
            }
        }

        if (!found) {
            auto it = std::find(active.begin(), active.end(), i);
            if (it != active.end()) {
                active.erase(it);
            }
        }
    }

    std::vector<glm::vec2> adjustedPoints;
    for (const auto& p : points) {
        glm::vec2 offset = p - glm::vec2(width / 2.0f, height / 2.0f);
        if (glm::length(offset) <= clusterRadius) {
            adjustedPoints.push_back(center + offset);
        }
    }

    return adjustedPoints;
}

std::vector<coral*> TerrainGenerator::generateCoralClusters() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 2);

    std::vector<coral*> toReturn;
    float tallCoralRadius = 1.5f;
    float brainCoralRadius = 1.0f;
    float genericCoralRadius = 1.0f;

    const int numClusters = 15;
    const float clusterRadius = 2.0f;
    const float minClusterDistance = 4.0f;

    const int numSamplesPerCoral = 10;

    float terrainWidth = 20.0f;
    float terrainHeight = 20.0f;

    std::vector<glm::vec2> clusterCenters = generateClusterCenters(terrainWidth, terrainHeight, numClusters, minClusterDistance, numSamplesPerCoral);

    for (const auto& center : clusterCenters) {
        std::vector<glm::vec2> tallCoralSamples = getPoissonDiscSamplesInCluster(center, clusterRadius, tallCoralRadius, numSamplesPerCoral);

        for (const auto& sample : tallCoralSamples) {
            TallCoral* toAdd = new TallCoral();
            toAdd->coralData = tallCoralData;
            toAdd->ctm = getCTM(sample.x, sample.y);
            toAdd->ctm = glm::scale(toAdd->ctm, glm::vec3(5.0f, 5.0f, 5.0f));
            toReturn.push_back(toAdd);

            modelloader::loadArrayToVBO(toAdd->vbo, toAdd->vao, toAdd->coralData);
        }

        std::vector<glm::vec2> genericCoralSamples = getPoissonDiscSamplesInCluster(center, clusterRadius, genericCoralRadius, numSamplesPerCoral);

        for (const auto& sample : genericCoralSamples) {
            GenericCoral* toAdd = new GenericCoral();
            toAdd->coralData = genericCoralData;
            toAdd->ctm = getCTM(sample.x, sample.y);
            toReturn.push_back(toAdd);

            modelloader::loadArrayToVBO(toAdd->vbo, toAdd->vao, toAdd->coralData);
        }

        std::vector<glm::vec2> brainCoralSamples = getPoissonDiscSamplesInCluster(center, clusterRadius, brainCoralRadius, numSamplesPerCoral);

        for (const auto& sample : brainCoralSamples) {
            BrainCoral* toAdd = new BrainCoral();
            toAdd->coralData = brainCoralData;
            toAdd->ctm = getCTM(sample.x, sample.y);
            toReturn.push_back(toAdd);

            modelloader::loadArrayToVBO(toAdd->vbo, toAdd->vao, toAdd->coralData);
        }
    }

    return toReturn;
}
