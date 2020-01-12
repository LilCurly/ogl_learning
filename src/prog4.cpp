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

// Light Postion
glm::vec3 lightPos = glm::vec3(3.0f, 0.0f, -3.0f);

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
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        cam.ProcessKeyboard(Camera_Movement::UP, deltatime);
    }
    if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        cam.ProcessKeyboard(Camera_Movement::BOTTOM, deltatime);
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

    Shader objShader(find_executable().parent_path().append("../../src/shader/vertex/prog4.vertex.vs").string().c_str(), find_executable().parent_path().append("../../src/shader/fragment/prog4.fragment.fs").string().c_str());
    Shader lightShader(find_executable().parent_path().append("../../src/shader/vertex/prog4.vertex.vs").string().c_str(), find_executable().parent_path().append("../../src/shader/fragment/prog4.lightsource.fs").string().c_str());
    
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    GLuint VBO, objVAO;

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &objVAO);

    glBindVertexArray(objVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(3*sizeof(GLfloat)));
    
    GLuint lightVAO;

    glGenVertexArrays(1, &lightVAO);

    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    objShader.Use();
    objShader.setVec3("objColor", 1.0f, 0.5f, 0.31f);
    objShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        processInput(window);

        float currentframe = glfwGetTime();
        deltatime = currentframe - lastframe;
        lastframe = currentframe;
        
        glClearColor(0.3f, 0.4f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::mat4(1.0f);
        view = cam.GetViewMatrix();
        projection = glm::perspective(glm::radians(cam.Zoom), (float) WIDTH/ (float) HEIGHT, 0.1f, 100.0f);

        objShader.Use();

        objShader.setMatrix("view", glm::value_ptr(view));
        objShader.setMatrix("projection", glm::value_ptr(projection));
        objShader.setVec3("lightPos", lightPos);

        glm::mat4 objModel = glm::mat4(1.0f);
        objShader.setMatrix("model", glm::value_ptr(objModel));

        glBindVertexArray(objVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        lightShader.Use();

        lightShader.setMatrix("view", glm::value_ptr(view));
        lightShader.setMatrix("projection", glm::value_ptr(projection));

        glm::mat4 lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, lightPos);
        lightModel = glm::scale(lightModel, glm::vec3(0.2f));
        lightShader.setMatrix("model", glm::value_ptr(lightModel));

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
    }


    glDeleteVertexArrays(1, &objVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();

    return EXIT_SUCCESS;
}
