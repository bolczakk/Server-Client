#include "Plane.h"

Plane::Plane(Material mat, glm::vec3 pos, glm::vec3 size)
    : Model(mat, pos, size) {
    setupMesh();
}

void Plane::setupMesh() {
    float vertices[] = {
        // positions            // normals
        -0.5f, 0.0f, -0.5f,     0.0f, 1.0f, 0.0f,
         0.5f, 0.0f, -0.5f,     0.0f, 1.0f, 0.0f,
         0.5f, 0.0f,  0.5f,     0.0f, 1.0f, 0.0f,

        -0.5f, 0.0f, -0.5f,     0.0f, 1.0f, 0.0f,
         0.5f, 0.0f,  0.5f,     0.0f, 1.0f, 0.0f,
        -0.5f, 0.0f,  0.5f,     0.0f, 1.0f, 0.0f
    };

    vertexCount = 6;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // aPos (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // aNormal (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

}
