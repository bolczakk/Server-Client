#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Shader.h"
#include "../Material.h"

class Model {
  public:
    Model(Material mat, glm::vec3 pos = glm::vec3(0.0f), glm::vec3 size = glm::vec3(1.0f));
    virtual ~Model() = default;

    void render(Shader &shader);
    void cleanup();
    void change_position(glm::vec3 Pos);

  protected:
    unsigned int VAO = 0, VBO = 0;
    glm::vec3 pos;
    glm::vec3 size;
    int vertexCount = 0;
    Material material;
    virtual void setupMesh() = 0;
};

#endif
