#ifndef CUBE_H
#define CUBE_H

#include "Model.h"
#include "../Material.h"

class Cube : public Model {
public:
    Cube(Material mat, glm::vec3 pos = glm::vec3(0.0f), glm::vec3 size = glm::vec3(1.0f));

private:
    void setupMesh() override;
};

#endif
