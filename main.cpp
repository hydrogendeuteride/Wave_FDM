#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "shader.h"
#include <vector>
#include "Wave_FDM_Solver.h"

glm::vec3 CamPos = glm::vec3 (0.0f, 0.0f, 2.0f);
glm::vec3 CamFront = glm::vec3 (0.0f, 0.0f, -1.0f);
glm::vec3 CamUp = glm::vec3(0.0f, 1.0f, 0.0f);

float fov = 60.0;

constexpr unsigned int SCR_WIDTH = 1920, SCR_HEIGHT = 1080;

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraspeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        CamPos += cameraspeed * CamUp;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        CamPos -= cameraspeed * CamUp;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        CamPos -= glm::normalize(glm::cross(CamFront, CamUp)) * cameraspeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        CamPos += glm::normalize(glm::cross(CamFront, CamUp)) * cameraspeed;
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 60.0f)
        fov = 60.0f;
}

template <typename T>
T clip(const T& n, const T& lower, const T& upper)
{
    return std::max(lower, std::min(n, upper));
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Wave FDM", nullptr, nullptr);

    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    std::vector<float> vertices = {
            //pos                   //tex coord
            0.7f, 0.7f, 0.0f,       1.0f, 1.0f,
            0.7f, -0.7f, 0.0f,      1.0f, 0.0f,
            -0.7f, -0.7f, 0.0f,     0.0f, 0.0f,
            -0.7f, 0.7f, 0.0f,      0.0f, 1.0f,
    };

    std::vector<int> indicies = {
            0, 1, 3,
            1, 2, 3
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof (float), static_cast<void*>(vertices.data()), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof (float), static_cast<void*>(indicies.data()), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof (float), (void*)0 );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof (float), (void*)(3 * sizeof (float )) );
    glEnableVertexAttribArray(1);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Wave_FDM wave(1024, 2, 0.001); //   2 / 1024 = 0.0019

    int width = 1024;
    int height = 1024;
    auto *data = new unsigned char [width * height * 3];

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    Shader shader("../shader.vert", "../shader.frag");

    //wave.Random_Source(255.0);
    wave.Space(512, 512, 0) = 255;
    wave.Space(513, 512, 0) = 255;
    wave.Space(511, 512, 0) = 255;
    wave.Space(512, 513, 0) = 255;
    wave.Space(512, 511, 0) = 255;

    wave.Space(768, 768, 0) = 255;
    wave.Space(767, 768, 0) = 255;
    wave.Space(769, 768, 0) = 255;
    wave.Space(768, 767, 0) = 255;
    wave.Space(768, 769, 0) = 255;

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(fov), (float )SCR_WIDTH / (float )SCR_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("projection", projection);

        glm::mat4 view = glm::lookAt(CamPos, CamPos + CamFront, CamUp);
        shader.setMat4("view", view);

        auto initial = wave.Update_Fast(1.0);

        glm::mat4 model = glm::mat4 (1.0f);
        shader.setMat4("model", model);
        for (int i = height- 1; i >= 0; --i)
        {
            for (int j = 0; j < width; ++j)
            {
                data[(i * width + j) * 3 + 1] = static_cast<int>(std::abs(std::round(
                        clip(initial(j, i, 1) * 4.0 + 0.0, 0.0, 255.0))));
                data[(i * width + j) * 3 + 0] = static_cast<int>(std::abs(std::round(
                        clip(initial(j, i, 0) * 4.0 + 0.0, 0.0, 255.0))));
                data[(i * width + j) * 3 + 2] = static_cast<int>(std::abs(std::round(
                        clip(initial(j, i, 2) * 4.0 + 0.0, 0.0, 255.0))));
            }
        }

        data[(512 * 1024 + 512) * 3] = 255;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        glBindTexture(GL_TEXTURE_2D, texture);

        shader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    delete[] data;

    glfwTerminate();
    return 0;
}
