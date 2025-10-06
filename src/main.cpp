#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"

// Declarations
void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
glm::vec4 findRanges(std::vector<float> &data);
void setupBuffers(unsigned int &VAO, unsigned int &VBO, unsigned int &EBO);
void cleanupBuffers(unsigned int &VAO, unsigned int &VBO, unsigned int &EBO);

// Constants
const int SCREEN_WIDTH {1080};
const int SCREEN_HEIGHT {1080};
const char *WINDOW_NAME {"Mandelbrot"};

// Camera state
struct CameraState
{
    float center_x {0.0f};
    float center_y {0.0f};
    float zoom {4.0f};
}camera;

// Geometry data
const float vertices[] =
{
    -1.0f, -1.0f, -0.0f,
     1.0f,  1.0f, -0.0f,
    -1.0f,  1.0f, -0.0f,
     1.0f, -1.0f, -0.0f
};

const unsigned int indices[] =
{
    0, 1, 2, // First triangle
    0, 3, 1  // Second triangle
};


int main()
{
    // Initialize GLFW and window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_NAME,  NULL, NULL);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window!\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed initializing GLAD\n";
        return -1;
    }

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Setup buffers and shader
    unsigned int VAO, VBO, EBO;
    setupBuffers(VAO, VBO, EBO);
    Shader our_shader("./shaders/shader.vert", "./shaders/shader.frag");

    glEnable(GL_DEPTH_TEST);

    // Rendering state
    std::vector<float> pixel_data(SCREEN_WIDTH * SCREEN_HEIGHT, 0.0f);
    glm::vec4 ranges = glm::vec4(0.0001f, 0.33333f, 0.66667f, 1.00f);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Input
        processInput(window);

        // Render
        glClearColor(0.2f, 0.0f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        our_shader.use_shader();
        our_shader.setFloat("zoom", camera.zoom);
        our_shader.setFloat("center_x", camera.center_x);
        our_shader.setFloat("center_y", camera.center_y);
        our_shader.setVec4("color_ranges", ranges);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Update color ranges
        glReadPixels(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_DEPTH_COMPONENT, GL_FLOAT, pixel_data.data());
        ranges = findRanges(pixel_data);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    // Cleanup
    glfwDestroyWindow(window);
    cleanupBuffers(VAO, VBO, EBO);

    glfwTerminate();
    return 0;
}

void setupBuffers(unsigned int &VAO, unsigned int &VBO, unsigned int &EBO)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(0));
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void cleanupBuffers(unsigned int &VAO, unsigned int &VBO, unsigned int &EBO)
{
    if (VAO != 0)
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    if (VBO != 0)
    {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (EBO != 0)
    {
        glDeleteBuffers(1, &EBO);
        EBO = 0;
    }
}


void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    // Movement
    float move_speed = 0.005 * camera.zoom;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.center_y = std::min(camera.center_y + move_speed, 1.0f);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.center_y = std::max(camera.center_y - move_speed, -1.0f);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.center_x = std::max(camera.center_x - move_speed, -1.0f);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.center_x = std::min(camera.center_x + move_speed, 1.0f);
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        camera.zoom = std::min(camera.zoom * 1.02f, 1.0f);
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        camera.zoom = std::max(camera.zoom * 0.98f, 0.00001f);
    }
}


void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}


glm::vec4 findRanges(std::vector<float> &data)
{
    std::vector<float> copy = data;
    std::sort(copy.begin(), copy.end());

    int lowest = 0;
    int size = copy.size();

    // Skip leading zeros safely
    while (lowest < size && copy[lowest] == 0.0f)
    {
        ++lowest;
    }

    if (lowest >= size) {
        return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    int length = size - lowest;
    return glm::vec4(
        copy[lowest],
        copy[lowest + length * 3 / 4 - 1],
        copy[lowest + length * 7 / 8 - 1],
        copy[size - 1]
    );
}

