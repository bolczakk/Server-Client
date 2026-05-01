#ifndef FRIEND
#define FRIEND

#include <string>
#include <glm/glm.hpp>

struct Packet {
    glm::vec3 pos;
};

class Friend {
  public:
    Friend(std::string nickname, int id);

    std::string nickname;
    glm::vec3 getPos() const;
    void setPos(const glm::vec3 newPos);

    int id;

  private:
    Packet packet;
};

#endif
