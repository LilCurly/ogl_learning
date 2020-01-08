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
#include "Camera.h"
#include "shapes/Cube.h"

const GLint WIDTH = 800, HEIGHT = 600;

float mixValue = 0.0f;

Camera cam;
bool firstMouse = true;
float lastX, lastY;

// Delta Time
float deltatime = 0.0f;
float lastframe = 0.0f;

// Projection
glm::mat4 projection;

// Configuration
int nbrCubes = 100;
int cubesPerRow = 10;
int cubesPerCol = 10;

boost::filesystem::path find_executable()
{
  unsigned int bufferSize = 512;
  std::vector<char> buffer(bufferSize + 1);

#if defined(_WIN32)
  std::vector<wchar_t> pathBuf; 
  DWORD copied = 0;
  do {
      pathBuf.resize(pathBuf.size()+MAX_PATH);
      copied = GetModuleFileNameW(0, &pathBuf.at(0), pathBuf.size());
  } while( copied >= pathBuf.size() );

  pathBuf.resize(copied);

  buffer.assign((wchar_t*) &pathBuf.front(), (wchar_t*) (&pathBuf.back() + 1));


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

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    cam.ProcessMouseMovement(xoffset, yoffset);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    cam.ProcessMouseScroll(yoffset);

    glm::perspective(glm::radians(cam.Zoom), (float) WIDTH/ (float) HEIGHT, 0.1f, 100.0f);
}

void processInput(GLFWwindow* window)
{
    float yaw = -90.0f;
    float speed = 2.5 * deltatime;
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        if(mixValue < 1.0f)
        {
            mixValue += 0.001f;
        }
    }
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        if(mixValue > 0.0f)
        {
            mixValue -= 0.001f;
        }
    }
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cam.ProcessKeyboard(Camera_Movement::FORWARD, deltatime);
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cam.ProcessKeyboard(Camera_Movement::BACKWARD, deltatime);
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cam.ProcessKeyboard(Camera_Movement::LEFT, deltatime);
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cam.ProcessKeyboard(Camera_Movement::RIGHT, deltatime);
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

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Prog3", nullptr, nullptr);

    if(window == nullptr) 
    {
        std::cout << "Failed to create a GLFW window" << std::endl;

        glfwTerminate();
        return EXIT_FAILURE;
    }

    int w_width, w_height;
    glfwGetFramebufferSize(window, &w_width, &w_height);

    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    glewExperimental = GL_TRUE;

    if(GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize glew" << std::endl;

        glfwTerminate();
        return EXIT_FAILURE;
    }

    glViewport(0, 0, w_width, w_height);
    glEnable(GL_DEPTH_TEST);

    cam = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

    // SHADER PART

    Shader firstShader(find_executable().parent_path().append("../../src/shader/vertex/vertex.vs").string().c_str(), find_executable().parent_path().append("../../src/shader/fragment/fragment_texture.fs").string().c_str());
    Texture2D sideDirt(find_executable().parent_path().append("../../res/textures/side_dirt.jpg").string().c_str());
    Texture2D::SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    Texture2D::SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    Texture2D topDirt(find_executable().parent_path().append("../../res/textures/top_dirt.png").string().c_str());
    Texture2D::SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    Texture2D::SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    std::vector<Cube> cubesVector;
    
    for (int i = 0; i < nbrCubes; i++) {
        cubesVector.push_back(Cube((float) (i % cubesPerRow), (float) floor(rand() % 3), (float) floor(i / cubesPerCol) * -1, 0.5f));
    }

    GLuint VBO, VAO;

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubesVector[0].vertices) + (sizeof(GLfloat) * cubesVector[0].vertices.size()) , &cubesVector[0].vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));

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

        float currentframe = glfwGetTime();
        deltatime = currentframe - lastframe;
        lastframe = currentframe;
        
        glClearColor(0.3f, 0.4f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        firstShader.setFloat("mixParameter", mixValue);

        firstShader.Use();
        glBindVertexArray(VAO); 
        sideDirt.Bind(GL_TEXTURE0);
        topDirt.Bind(GL_TEXTURE1);
        const float radius = 15.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;
        glm::mat4 view = glm::mat4(1.0f);
        view = cam.GetViewMatrix();
        firstShader.setMatrix("view", glm::value_ptr(view));
        projection = glm::perspective(glm::radians(cam.Zoom), (float) WIDTH/ (float) HEIGHT, 0.1f, 100.0f);
        firstShader.setMatrix("projection", glm::value_ptr(projection));

        for (int i = 0; i < nbrCubes; i++) {
          glm::mat4 model = glm::mat4(1.0f);
          model = glm::translate(model, cubesVector[i].position);
          firstShader.setMatrix("model", glm::value_ptr(model));

          glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();

    return EXIT_SUCCESS;
}
