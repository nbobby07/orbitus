#version 430 core

struct Particle {
    vec2 position;
    vec2 velocity;
    vec3 color;
    float mass;
};

layout(std430, binding = 0) buffer ParticleBuffer {
    Particle particles[];
};

uniform vec2 cameraOffset;
uniform float cameraZoom;

out vec3 ParticleColor;

void main() {
    Particle p = particles[gl_VertexID];

    vec2 finalPos = (p.position * cameraZoom) + cameraOffset;
    
    gl_Position = vec4(finalPos, 0.0, 1.0);
    ParticleColor = p.color;
    
    gl_PointSize = clamp(p.mass * 2.0  * cameraZoom, 2.0, 20.0);
}