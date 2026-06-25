#version 430 core
out vec4 FragColor;
in vec3 ParticleColor;

void main() {
    vec2 coord = gl_PointCoord - vec2(0.5);
    if(length(coord) > 0.5) {
        discard;
    }
    FragColor = vec4(ParticleColor, 1.0);
}