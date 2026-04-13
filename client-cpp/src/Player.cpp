#include "Player.h"

Player::Player(glm::vec3 pos) : camera(pos), model(nullptr), pos(pos) {}

void Player::initModel() {
    model = std::make_unique<Cube>(Material::green_plastic);
    model->change_position(this->pos);
}

void Player::render(Shader &shader) { model->render(shader); }

void Player::cleanup() { model->cleanup(); }
