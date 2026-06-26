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
    glm::vec2 velocity;
    glm::vec3 color;
    float mass;
};

float cameraZoom = 1.0f;
float cameraOffsetX = 0.0f;
float cameraOffsetY = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0,0,width,height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    cameraZoom += (float)yoffset * 0.1f;
    if (cameraZoom < 0.1f) cameraZoom = 0.1f;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1024, 768, "Orbitus Engine - V0.3", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetScrollCallback(window, scroll_callback);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glEnable(GL_PROGRAM_POINT_SIZE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    Shader particleShader("C:/Users/Noel/Desktop/orbitus/src/particle.vert", 
                          "C:/Users/Noel/Desktop/orbitus/src/particle.frag");

    Shader computeShader("C:/Users/Noel/Desktop/orbitus/src/physics.comp");

    Shader screenShader("C:/Users/Noel/Desktop/orbitus/src/screen.vert", "C:/Users/Noel/Desktop/orbitus/src/screen.frag");
    
    const int NUM_PARTICLES = 1000;
    std::vector<Particle> particles(NUM_PARTICLES);

    std::mt19937 gen(1337);
    std::uniform_real_distribution<float> randAngle(0.0f, glm::two_pi<float>());
    std::uniform_real_distribution<float> randRadius(0.0f, 0.9f);

    std::uniform_real_distribution<float> randMass(0.5f, 2.0f);

    for(int i = 0; i < NUM_PARTICLES; ++i) {
        float angle = randAngle(gen);
        float r = sqrt(randRadius(gen));

        float mass = (i == 0) ? 5000.0f : randMass(gen);
        r = (i == 0) ? 0.0f : r;

        particles[i].position = glm::vec2(cos(angle) * r, sin(angle) * r);
        particles[i].mass = mass;

        float velMag = (i == 0) ? 0.0f : sqrt(0.00005f * 5000.0f / r);
        particles[i].velocity = glm::vec2(-sin(angle), cos(angle)) * velMag *  0.8f;

        float heat = 1.0f - r;
        particles[i].color = glm::vec3(heat, 0.5f + (heat*0.5f), 1.0f);
        if (i == 0) particles[i].color = glm::vec3(1.0f, 1.0f, 1.0f);
    }

    unsigned int ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, particles.size() * sizeof(Particle), particles.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    unsigned int vao;
    glGenVertexArrays(1, &vao);

    unsigned int pingpongFBO[2];
    unsigned int pingpongColorbuffers[2];
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    
    for(unsigned int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 768, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float quadVertices[] = {
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f, 
        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    float dt = 0.016f;
    int colorTheme = 0;

    bool first_iteration = true;
    bool writeToFirst = true;

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        float panSpeed = 0.02f / cameraZoom; 
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraOffsetY -= panSpeed;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraOffsetY += panSpeed;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cameraOffsetX += panSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cameraOffsetX -= panSpeed;

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) colorTheme = 0; // Default
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) colorTheme = 1; // Fire
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) colorTheme = 2; // Matrix

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        float normX = (mouseX / width) * 2.0f - 1.0f;
        float normY = -((mouseY / height) * 2.0f - 1.0f); 
        float mouseWorldX = (normX - cameraOffsetX) / cameraZoom;
        float mouseWorldY = (normY - cameraOffsetY) / cameraZoom;

        float mouseMass = 0.0f;
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            mouseMass = 150000.0f;
        }

        computeShader.use();
        glUniform1f(glGetUniformLocation(computeShader.ID, "dt"), dt);
        glUniform2f(glGetUniformLocation(computeShader.ID, "mousePos"), mouseWorldX, mouseWorldY);
        glUniform1f(glGetUniformLocation(computeShader.ID, "mouseMass"), mouseMass);
        
        GLuint numGroups = (NUM_PARTICLES + 255) / 256;
        glDispatchCompute(numGroups, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[writeToFirst ? 1 : 0]);

        if (first_iteration) {
            glClearColor(0.0f, 0.0f, 0.05f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            first_iteration = false;
        }

        screenShader.use();
        glDisable(GL_BLEND);
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[writeToFirst ? 0 : 1]);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        particleShader.use();
        glUniform2f(glGetUniformLocation(particleShader.ID, "cameraOffset"), cameraOffsetX, cameraOffsetY);
        glUniform1f(glGetUniformLocation(particleShader.ID, "cameraZoom"), cameraZoom);
        glUniform1i(glGetUniformLocation(particleShader.ID, "colorTheme"), colorTheme);

        glEnable(GL_BLEND);
        glBindVertexArray(vao);
        glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, pingpongFBO[writeToFirst ? 1 : 0]);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, 1024, 768, 0, 0, 1024, 768, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        writeToFirst = !writeToFirst;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;

}