#if defined(_WIN32)
  #include <windows.h>
#elif defined(__linux__)
  #include <sstream>
  #include <unistd.h>
#elif defined(__APPLE__)
  #include <mach-o/dyld.h>
#endif

#include <vector>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <boost/filesystem.hpp>

#include "Shader.h"
#include "Texture2D.h"

const GLint WIDTH = 800, HEIGHT = 600;
float mixValue = 0.0f;

boost::filesystem::path find_executable()
{
  unsigned int bufferSize = 512;
  std::vector<char> buffer(bufferSize + 1);

#if defined(_WIN32)
  ::GetModuleFileName(NULL, &buffer[0], bufferSize);

#elif defined(__linux__)
  int pid = getpid();

  std::ostringstream oss;
  oss << "/proc/" << pid << "/exe";
  std::string link = oss.str();

  int count = readlink(link.c_str(), &buffer[0], bufferSize);
  if(count == -1) throw std::runtime_error("Could not read symbolic link");
  buffer[count] = '\0';

#elif defined(__APPLE__)
  if(_NSGetExecutablePath(&buffer[0], &bufferSize))
  {
    buffer.resize(bufferSize);
    _NSGetExecutablePath(&buffer[0], &bufferSize);
  }

#else
  #error Cannot yet find the executable on this platform
#endif

  std::string s = &buffer[0];
  return s;
}

void processInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE))
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if(glfwGetKey(window, GLFW_KEY_UP))
    {
        if(mixValue < 1.0f)
        {
            mixValue += 0.001f;
        }
    }
    if(glfwGetKey(window, GLFW_KEY_DOWN))
    {
        if(mixValue > 0.0f)
        {
            mixValue -= 0.001f;
        }
    }
}

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Prog1", nullptr, nullptr);

    if(window == nullptr) 
    {
        std::cout << "Failed to create a GLFW window" << std::endl;

        glfwTerminate();
        return EXIT_FAILURE;
    }

    int w_width, w_height;
    glfwGetFramebufferSize(window, &w_width, &w_height);

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;

    if(GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize glew" << std::endl;

        glfwTerminate();
        return EXIT_FAILURE;
    }

    glViewport(0, 0, w_width, w_height);

    // SHADER PART
    
    Shader firstShader(find_executable().parent_path().append("../../src/shader/vertex/vertex.vs").c_str(), find_executable().parent_path().append("../../src/shader/fragment/fragment_texture.fs").c_str());
    Texture2D crateTex(find_executable().parent_path().append("../../res/textures/container.jpg").c_str());
    Texture2D::SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    Texture2D::SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    Texture2D otherTex(find_executable().parent_path().append("../../res/textures/5039.jpg").c_str());
    Texture2D::SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    Texture2D::SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    GLfloat triangle[] = 
    {
        -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // TOP LEFT
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // BOTTOM LEFT
        0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // BOTTOM RIGHT
        0.5f, 0.5f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f // TOP RIGHT
    };

    GLuint indices[] =
    {
        0, 1, 2,
        0, 3, 2
    };

    GLuint VBO, VAO, EBO;

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(6 * sizeof(GL_FLOAT)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    firstShader.Use();
    firstShader.setInt("firstTex", 0);
    firstShader.setInt("secondTex", 1);

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        processInput(window);
        
        glClearColor(0.3f, 0.4f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        firstShader.setFloat("mixParameter", mixValue);
        
        firstShader.Use();
        glBindVertexArray(VAO);
        crateTex.Bind(GL_TEXTURE0);
        otherTex.Bind(GL_TEXTURE1);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();

    return EXIT_SUCCESS;
}
