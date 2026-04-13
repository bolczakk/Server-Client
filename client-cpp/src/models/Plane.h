#ifndef PLANE_H
#define PLANE_H

#include "Model.h"

class Plane : public Model {
public:
	Plane(Material mat, glm::vec3 pos = glm::vec3(0.0f, -0.51f, 0.0f), glm::vec3 size = glm::vec3(6.0f));

private:
	void setupMesh() override;
};

#endif