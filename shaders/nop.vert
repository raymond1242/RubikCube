# version 330 core
// Do not modify the above version directive to anything older than 330, as
// modern OpenGL will not work properly due to it not being core at the time.

// Shader inputs
layout (location = 0) in vec3 position;

// Uniform variables
uniform mat4 modelview;
uniform mat4 projection;

void main() {
    gl_Position = projection * modelview * vec4(position, 1.0f);
}

