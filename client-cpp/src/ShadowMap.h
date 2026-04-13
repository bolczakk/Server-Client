#ifndef SHADOWMAP_H
#define SHADOWMAP_H

#include <glad/glad.h>

class ShadowMap {
public:
	ShadowMap();
    unsigned int FBO, depthMap;
    const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;

    void bind();
    void unbind();
private:
};

#endif