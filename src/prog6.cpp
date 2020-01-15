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

    Shader objShader(find_executable().parent_path().append("../../src/shader/vertex/prog5.vertex.vs").string().c_str(), find_executable().parent_path().append("../../src/shader/fragment/prog6.fragment.fs").string().c_str());
    
    // Texture loading

    Texture2D containerTex = Texture2D(find_executable().parent_path().append("../../res/textures/container2.png").string().c_str());
    Texture2D specularContainerTex = Texture2D(find_executable().parent_path().append("../../res/textures/container2_specular.png").string().c_str());

    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    GLuint VBO, objVAO;

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &objVAO);

    glBindVertexArray(objVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(6 * sizeof(GLfloat)));
    
    GLuint lightVAO;

    glGenVertexArrays(1, &lightVAO);

    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 objColor = glm::vec3(0.9f, 0.5f, 0.1f);
    objShader.Use();
    objShader.setVec3("objColor", objColor);

    objShader.setVec3("mat.ambient", objColor * glm::vec3(0.9f));
    //objShader.setVec3("mat.diffusal", objColor * glm::vec3(0.8f));
    objShader.setVec3("mat.specular", 1.0f, 1.0f, 1.0f);
    objShader.setFloat("mat.shininess", 32.0f);

    objShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    objShader.setVec3("light.diffuse", lightColor * glm::vec3(0.8f));
    objShader.setVec3("light.ambient", lightColor * glm::vec3(0.2f));
    objShader.setFloat("light.constant", 1.0f);
    objShader.setFloat("light.linear", 0.09f);
    objShader.setFloat("light.quadratic", 0.032f);
    objShader.setFloat("light.innerAngle", glm::cos(glm::radians(12.5f)));
    objShader.setFloat("light.outerAngle", glm::cos(glm::radians(17.5f)));

    objShader.setInt("mat.diffusal", 0);
    objShader.setInt("mat.specular", 1);
    objShader.setInt("mat.emission", 2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, containerTex.Texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularContainerTex.Texture);

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
        

        objShader.setVec3("light.position", cam.Position);
        objShader.setVec3("light.direction", cam.Front);

        objShader.setMatrix("view", glm::value_ptr(view));
        objShader.setMatrix("projection", glm::value_ptr(projection));
        objShader.setVec3("viewPos", cam.Position);
        
        glBindVertexArray(objVAO);
        for (int i = 0; i < 10; i++) {
            glm::mat4 objModel = glm::mat4(1.0f);
            objModel = glm::translate(objModel, cubePositions[i]);
            objModel = glm::rotate(objModel, (float) glm::radians(glfwGetTime()) * 15, glm::vec3(1.0f, 0.0f, 1.0f));
            objShader.setMatrix("model", glm::value_ptr(objModel));
            objShader.setMatrix("modelNormal", glm::value_ptr(glm::transpose(glm::inverse(objModel))));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
    }


    glDeleteVertexArrays(1, &objVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();

    return EXIT_SUCCESS;
}