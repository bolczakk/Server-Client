#ifndef PYRAMID_H
#define PYRAMID_H

#include "Model.h"
#include "../Material.h"

class Pyramid : public Model {
public:
    Pyramid(Material mat, glm::vec3 pos = glm::vec3(0.0f), glm::vec3 size = glm::vec3(1.0f));

private:
    void setupMesh() override;
}; 

#endif