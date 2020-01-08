#include <vector>

#include <glm/glm.hpp>

class Cube
{
public:
    glm::vec3 position;
    std::vector<float> vertices;

    Cube(float posX, float posY, float posZ, float size)
    {
        position.x = posX;
        position.y = posY;
        position.z = posZ;

        vertices = getVertices(size);
    }
    
    static std::vector<float> getVertices(float size) {
        std::vector<float> vertices = {
            -size, -size, -size,  0.0f, 0.0f,
            size, -size, -size,  1.0f, 0.0f,
            size,  size, -size,  1.0f, 1.0f,
            size,  size, -size,  1.0f, 1.0f,
            -size, size, -size,  0.0f, 1.0f,
            -size, -size, -size,  0.0f, 0.0f,

            -size, -size,  size,  0.0f, 0.0f,
            size, -size,  size,  1.0f, 0.0f,
            size,  size,  size,  1.0f, 1.0f,
            size,  size,  size,  1.0f, 1.0f,
            -size,  size,  size,  0.0f, 1.0f,
            -size, -size,  size,  0.0f, 0.0f,

            -size,  size,  size,  1.0f, 0.0f,
            -size,  size, -size,  1.0f, 1.0f,
            -size, -size, -size,  0.0f, 1.0f,
            -size, -size, -size,  0.0f, 1.0f,
            -size, -size,  size,  0.0f, 0.0f,
            -size,  size,  size,  1.0f, 0.0f,

            size,  size,  size,  1.0f, 0.0f,
            size,  size, -size,  1.0f, 1.0f,
            size, -size, -size,  0.0f, 1.0f,
            size, -size, -size,  0.0f, 1.0f,
            size, -size,  size,  0.0f, 0.0f,
            size,  size,  size,  1.0f, 0.0f,

            -size, -size, -size,  0.0f, 1.0f,
            size, -size, -size,  1.0f, 1.0f,
            size, -size,  size,  1.0f, 0.0f,
            size, -size,  size,  1.0f, 0.0f,
            -size, -size,  size,  0.0f, 0.0f,
            -size, -size, -size,  0.0f, 1.0f,

            -size,  size, -size,  0.0f, 1.0f,
            size,  size, -size,  1.0f, 1.0f,
            size,  size,  size,  1.0f, 0.0f,
            size,  size,  size,  1.0f, 0.0f,
            -size,  size,  size,  0.0f, 0.0f,
            -size,  size, -size,  0.0f, 1.0f
        };

        return vertices;
    }
};