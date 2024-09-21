#version 330

in vec3 vertexPosition;
in mat4 instanceTransform;

uniform mat4 mvp;

void main() {
    // Compute MVP for current instance
    mat4 mvpi = mvp*instanceTransform;

    gl_Position = mvpi * vec4(vertexPosition, 1.0);
}
