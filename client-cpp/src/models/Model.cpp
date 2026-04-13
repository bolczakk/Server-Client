#include "Model.h"

Model::Model(Material mat, glm::vec3 pos, glm::vec3 size) : pos(pos), size(size), material(mat) {}

void Model::render(Shader &shader) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::scale(model, size);

    shader.setMat4("model", model);
    shader.setVec3("material.diffuse", material.diffuse);
    shader.setVec3("material.specular", material.specular);
    shader.setVec3("material.ambient", material.ambient);
    shader.setFloat("material.shininess", material.shininess);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

void Model::cleanup() {
    if (VAO)
        glDeleteVertexArrays(1, &VAO);
    if (VBO)
        glDeleteBuffers(1, &VBO);
}

void Model::change_position(glm::vec3 Pos) { pos = Pos; }
