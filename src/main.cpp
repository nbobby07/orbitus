#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>
#include <random>

#include "Shader.h"

struct Particle {
    glm::vec2 position;
    glm::vec3 color;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0,0,width,height);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Orbitus Engine - V0.2", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glEnable(GL_PROGRAM_POINT_SIZE);

    Shader particleShader("C:/Users/Noel/Desktop/orbitus/src/particle.vert", 
                          "C:/Users/Noel/Desktop/orbitus/src/particle.frag");
    
    const int NUM_PARTICLES = 10000;
    std::vector<Particle> particles(NUM_PARTICLES);

    std::mt19937 gen(42);
    std::uniform_real_distribution<float> randAngle(0.0f, glm::two_pi<float>());
    std::uniform_real_distribution<float> randRadius(0.0f, 0.9f);

    for(int i = 0; i < NUM_PARTICLES; ++i) {
        float angle = randAngle(gen);
        float r = sqrt(randRadius(gen));

        particles[i].position = glm::vec2(cos(angle) * r, sin(angle) * r);

        float heat = 1.0f - r;
        particles[i].color = glm::vec3(heat, 0.5f + (heat*0.5f), 1.0f);
    }

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), particles.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(sizeof(glm::vec2)));
    glEnableVertexAttribArray(1);

    while(!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.02f, 0.02f, 0.05f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT);
        particleShader.use();
        glBindVertexArray(VAO);
        
        glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;

}