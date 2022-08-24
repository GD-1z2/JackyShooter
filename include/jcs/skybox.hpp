#pragma once

#include <jacky-common/defs.hpp>

class Renderer;

class SkyBox {
public:
    SkyBox(Renderer &renderer, uint texture);

    void draw(const glm::mat4 &view) const;

private:
    const Renderer &renderer;
    uint vao{}, texture{};
};
