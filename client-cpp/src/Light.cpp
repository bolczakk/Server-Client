#include "Light.h"

const void DirLight::render(Shader& shader) const {
	shader.setVec3("dirLight.direction", direction);
	shader.setVec3("dirLight.ambient", ambient);
	shader.setVec3("dirLight.diffuse", diffuse);
	shader.setVec3("dirLight.specular", specular);
}