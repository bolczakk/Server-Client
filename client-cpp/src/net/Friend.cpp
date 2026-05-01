#include "Friend.h"
#include "glm/fwd.hpp"

Friend::Friend(std::string nickname, int id) : nickname(nickname), id(id), packet{glm::vec3(0.0f, 0.0f, 0.0f)} {}

glm::vec3 Friend::getPos() const { return packet.pos; }

void Friend::setPos(const glm::vec3 newPos) { packet.pos = newPos; }
