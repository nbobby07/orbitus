#version 430 core
out vec4 FragColor;
in vec3 ParticleColor;

uniform int colorTheme;

void main() {
    vec2 coord = gl_PointCoord - vec2(0.5);
    if(length(coord) > 0.5) {
        discard;
    }

    vec3 finalColor = ParticleColor;

    if(colorTheme == 1) {
        finalColor = vec3(ParticleColor.b, ParticleColor.g * 0.5, ParticleColor.r * 0.2);
    } else if(colorTheme==2) {
        finalColor = vec3(ParticleColor.r * 0.2, ParticleColor.b, ParticleColor.g * 0.2);
    }

    FragColor = vec4(finalColor, 1.0);
}