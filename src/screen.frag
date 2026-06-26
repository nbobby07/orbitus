#version 430 core
out vec4 FragColor;

in vec2 TexCoords;
uniform sampler2D screenTexture;

void main() {
    vec4 texColor = texture(screenTexture, TexCoords);

    FragColor = vec4(texColor.rgb * 0.95, 1.0);
}