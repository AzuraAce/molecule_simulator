#version 330 core
out vec4 FragColor;

uniform vec3 u_color;
uniform float u_alpha;

void main() {
    FragColor = vec4(u_color, u_alpha);
}

