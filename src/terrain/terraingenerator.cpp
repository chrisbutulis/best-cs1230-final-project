#include "terraingenerator.h"
#include "coral.h"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "../models/model-loader.h"
#include "glm/gtx/transform.hpp"
#include "tallcoral.h"
#include "braincoral.h"
#include <cmath>
#include <random>

TerrainGenerator::TerrainGenerator() {
    tallCoralData = modelloader::LoadGLBVerticesNormals("../../src/models/3d-models/tall_coral.glb");
    brainCoralData = modelloader::LoadGLBVerticesNormals("../../src/models/3d-models/brain_coral.glb");
}

glm::mat4 getCTM(float xTransform, float zTransform) {
    glm::mat4 ctm = glm::mat4(1.0f); // Start with identity matrix

    // Translation: Move the coral to (0, 0, -5)
    ctm = glm::translate(ctm, glm::vec3(xTransform-2.0f, -0.4f, zTransform-2.0f));

    // Rotation: Rotate 45 degrees around Y, 15 degrees around X
    ctm = glm::rotate(ctm, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Y-axis rotation
    ctm = glm::rotate(ctm, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // X-axis rotation

    // Scaling: Uniformly scale to half size
    ctm = glm::scale(ctm, glm::vec3(2.0f, 2.0f, 2.0f));

    return ctm;
}


std::vector<glm::vec2> getPoissonDiscSamples(float width, float height, float radius, float numSamples) {
    float cellSize = radius / std::sqrt(2.0f); // cell size is bounded by radius / sqrt(n) where n is num dimensions

    int numCols = ceil(width / cellSize);
    int numRows = ceil(height / cellSize);

    // initialize the grid (-1 indicates no sample)
    std::vector<int> grid(numRows * numCols, -1);

    std::random_device rd; // Non-deterministic seed
    std::mt19937 gen(rd()); // Mersenne Twister engine

    // Distributions for uniform random sampling
    std::uniform_real_distribution<double> distX(0.0f, width);
    std::uniform_real_distribution<double> distY(0.0f, height);
    std::uniform_real_distribution<double> distRadius(radius, 2.0f * radius);
    std::uniform_real_distribution<double> distAngle(0.0, 2.0 * M_PI);

    auto randActiveIndex = [&](int activeSize) {
        std::uniform_int_distribution<int> distIndex(0, activeSize - 1);
        return distIndex(gen);
    };

    auto getCell = [&](float x, float y) {
        return glm::vec2(int(x / cellSize), int(y / cellSize));
    };

    auto isValid = [&](float x, float y, std::vector<glm::vec2>& points) {
        // Check domain bounds
        if (x < 0 || x >= width || y < 0 || y >= height) {
            return false;
        }

        glm::vec2 cell = getCell(x, y);

        // We only need to check a small neighborhood around (cx, cy)
        int startX = std::max(int(cell.x) - 2, 0);
        int endX   = std::min(int(cell.x) + 2, numCols - 1);
        int startY = std::max(int(cell.y) - 2, 0);
        int endY   = std::min(int(cell.y) + 2, numRows - 1);

        for (int ny = startY; ny <= endY; ++ny) {
            for (int nx = startX; nx <= endX; ++nx) {
                int idx = grid[ny * numCols + nx];
                if (idx != -1) {
                    // Check distance to existing point
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

    // Start: place one initial random point
    std::vector<glm::vec2> points;
    glm::vec2 initialPoint(distX(gen), distY(gen));
    points.push_back(initialPoint);

    glm::vec2 initialCell = getCell(initialPoint.x, initialPoint.y);
    grid[initialCell.y * numCols + initialCell.x] = 0; // store the index of the the initial cell

    // Active list: indices of points that can spawn new points
    std::vector<int> active = {0};

    // Main loop
    while (!active.empty()) {
        // Pick a random active point
        int i = active[randActiveIndex(active.size())];
        glm::vec2 randomPoint = points[i];

        bool found = false;
        // Try k times to place a new point
        for (int attempt = 0; attempt < numSamples; attempt++) {
            double angle = distAngle(gen);
            double radius = distRadius(gen);

            double xNew = randomPoint.x + radius * cos(angle);
            double yNew = randomPoint.y + radius * sin(angle);

            if (isValid(xNew, yNew, points)) {
                // Add the new point
                glm::vec2 newPoint(xNew, yNew);
                int newIndex = (int)points.size();
                points.push_back(newPoint);

                // Update the grid
                glm::vec2 newCell = getCell(xNew, yNew);
                grid[newCell.y * numCols + newCell.x] = newIndex;

                // Add to active list
                active.push_back(newIndex);

                found = true;
                break; // Stop after successfully placing one new point
            }
        }

        if (!found) {
            // If no point found, remove i from active
            // Since we picked i from active by value, we must find it again
            auto it = std::find(active.begin(), active.end(), i);
            if (it != active.end()) {
                active.erase(it);
            }
        }
    }
    return points;
}

std::vector<coral*> TerrainGenerator::generateCoralClusters() {
    std::random_device rd;  // Seed source
    std::mt19937 gen(rd()); // Mersenne Twister PRNG
    std::uniform_int_distribution<int> dist(1, 2);

    std::vector<coral*> toReturn;

    for(glm::vec2 sample : getPoissonDiscSamples(5.0f, 5.0f, 1.0f, 10)) {
        coral* toAdd = {};
        int rand = dist(gen);
        if(rand == 1) {
            toAdd = new TallCoral();
            toAdd->coralData = tallCoralData;
        } else {
            toAdd = new BrainCoral();
            toAdd->coralData = brainCoralData;
        }
        toAdd->ctm = getCTM(sample.x, sample.y);
        toReturn.push_back(toAdd);

        modelloader::loadArrayToVBO(toAdd->vbo, toAdd->vao, toAdd->coralData);
    }
    return toReturn;
}
