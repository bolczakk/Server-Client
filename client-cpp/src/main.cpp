// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <glm/glm.hpp>
#include <iostream>
#include <asio.hpp>
#include <thread>

#include "Camera.h"
#include "Light.h"
#include "Player.h"
#include "Material.h"
#include "Shader.h"
#include "net/ClientUdp.h"
#include "models/Cube.h"
#include "models/Plane.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height) { glViewport(0, 0, width, height); }

void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
// void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void processInput(GLFWwindow *window, float deltaTime);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Camera camera1(glm::vec3(0.0f, 0.0f, 10.0f));
// Camera camera2(glm::vec3(10.0f, 0.0f, 0.0f));
Player player(glm::vec3(0.0f, 0.0f, 10.0f));
Camera *camera = &player.camera;

bool isDragging = false;
double lastX, lastY;
float rotationX, rotationY;
bool firstMouse = true;

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Window", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // glfwSetMouseButtonCallback(window, mouse_button_callback);
    // glfwSetKeyCallback(window, Keyboard::keyCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    Shader shader("assets/vertex.glsl", "assets/fragment.glsl");
    shader.use();

    DirLight dirLight = {glm::vec3(-0.5f, -0.8f, -0.3f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.7f, 0.7f, 0.7f),
                         glm::vec3(0.3f, 0.3f, 0.3f)};
    Plane plane(Material::silver);
    Cube cube(Material::emerald);
    player.initModel();

    std::vector<Friend> local_friends;
    local_friends.reserve(32);

    ClientUdp client("127.0.0.1", "8080");

    client.async_send();
    client.async_receive();
    std::thread t([&client]() { client.io_context.run(); });

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float x = glfwGetTime() / 4;
        dirLight.direction.x = sin(x);
        dirLight.direction.z = cos(x);

        processInput(window, deltaTime);

        client.update_packet(camera->Position);

        client.allocFriends(local_friends);

        glm::mat4 projection = glm::perspective(
            glm::radians(camera->Zoom), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);

        glm::mat4 view = camera->GetViewMatrix();

        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 0.5f, far_plane = 20.0f;
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(-dirLight.direction * 5.0f, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        lightSpaceMatrix = lightProjection * lightView;

        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("viewPos", camera->Position);

        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        dirLight.render(shader);
        // cube.render(shader);
        plane.render(shader);
        // player.render(shader);

        for (auto &f : local_friends) {
            if (f.id != client.getMyId()) {
                cube.change_position(f.getPos());
                cube.render(shader);
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    client.io_context.stop();
    t.join();
    cube.cleanup();
    plane.cleanup();
    player.cleanup();

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera->ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera->ProcessKeyboard(DOWN, deltaTime);
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera->ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera->ProcessMouseScroll(static_cast<float>(yoffset));
}
