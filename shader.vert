#version 330 core
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec2 TexCoord_In;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(Pos, 1.0f);;
    TexCoord = vec2(TexCoord_In.x, TexCoord_In.y);
}