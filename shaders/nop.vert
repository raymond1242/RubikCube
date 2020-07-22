# version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;

uniform mat4 modelview;
uniform mat4 projection;
uniform mat4 transform;
uniform mat4 disloc;

out vec3 ourColor;
out vec2 v_TexCoord;

void main() {
    gl_Position = projection * modelview * disloc * transform * vec4(position, 1.0f);
    ourColor = color;
    v_TexCoord = texCoord;
}

