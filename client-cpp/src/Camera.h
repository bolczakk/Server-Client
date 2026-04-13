#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CAMDIR { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

class Camera {
  public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw = -90.0f;
    float Pitch = 0.0f;
    float MovementSpeed = 10.0f;
    float MouseSensitivity = 0.08f;
    float Zoom = 45.0f;
    Camera(glm::vec3 position, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f);
    void ProcessMouseScroll(float yoffset);
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    void ProcessKeyboard(CAMDIR direction, float deltaTime);
    glm::mat4 GetViewMatrix();

  private:
    void updateCameraVectors();
};
