#include <cstdio>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>

static float v = 0.f;

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        v -= 0.0001f;

    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        v += 0.0001f;
}

int main(int argc, char const *argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    
    
    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(sinf(v), cosf(v), tanf(v), 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();    
    }


    glfwTerminate();
    return 0;

    /* code */
    return 67;
}
