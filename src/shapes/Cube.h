#include <vector>

#include <glm/glm.hpp>

class Cube
{
public:
    glm::vec3 position;
    float csize;

    Cube(float posX, float posY, float posZ, float size)
    {
        position.x = posX;
        position.y = posY;
        position.z = posZ;

        csize = size;
    }
    
    std::vector<float> getVertices() {
        std::vector<float> vertices = {
            -csize, -csize, -csize,  0.0f, 0.0f,
            csize, -csize, -csize,  1.0f, 0.0f,
            csize,  csize, -csize,  1.0f, 1.0f,
            csize,  csize, -csize,  1.0f, 1.0f,
            -csize, csize, -csize,  0.0f, 1.0f,
            -csize, -csize, -csize,  0.0f, 0.0f,

            -csize, -csize,  csize,  0.0f, 0.0f,
            csize, -csize,  csize,  1.0f, 0.0f,
            csize,  csize,  csize,  1.0f, 1.0f,
            csize,  csize,  csize,  1.0f, 1.0f,
            -csize,  csize,  csize,  0.0f, 1.0f,
            -csize, -csize,  csize,  0.0f, 0.0f,

            -csize,  csize,  csize,  1.0f, 0.0f,
            -csize,  csize, -csize,  1.0f, 1.0f,
            -csize, -csize, -csize,  0.0f, 1.0f,
            -csize, -csize, -csize,  0.0f, 1.0f,
            -csize, -csize,  csize,  0.0f, 0.0f,
            -csize,  csize,  csize,  1.0f, 0.0f,

            csize,  csize,  csize,  1.0f, 0.0f,
            csize,  csize, -csize,  1.0f, 1.0f,
            csize, -csize, -csize,  0.0f, 1.0f,
            csize, -csize, -csize,  0.0f, 1.0f,
            csize, -csize,  csize,  0.0f, 0.0f,
            csize,  csize,  csize,  1.0f, 0.0f,

            -csize, -csize, -csize,  0.0f, 1.0f,
            csize, -csize, -csize,  1.0f, 1.0f,
            csize, -csize,  csize,  1.0f, 0.0f,
            csize, -csize,  csize,  1.0f, 0.0f,
            -csize, -csize,  csize,  0.0f, 0.0f,
            -csize, -csize, -csize,  0.0f, 1.0f,

            -csize,  csize, -csize,  0.0f, 1.0f,
            csize,  csize, -csize,  1.0f, 1.0f,
            csize,  csize,  csize,  1.0f, 0.0f,
            csize,  csize,  csize,  1.0f, 0.0f,
            -csize,  csize,  csize,  0.0f, 0.0f,
            -csize,  csize, -csize,  0.0f, 1.0f
        };

        return vertices;
    }
};