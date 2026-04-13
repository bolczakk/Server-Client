#ifndef PLAYER_H
#define PLAYER_H

#include <glm/glm.hpp>
#include <memory>
#include "Camera.h"
#include "models/Cube.h"
#include "Material.h"

class Player {
  public:
    Player(glm::vec3 position);
    void initModel();
    Camera camera;
    void render(Shader &shader);
    void cleanup();

  private:
    std::unique_ptr<Cube> model;
    glm::vec3 pos;
};

#endif
