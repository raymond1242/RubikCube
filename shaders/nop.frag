# version 330 core

out vec4 fragColor;

in vec3 ourColor;
in vec2 v_TexCoord;

uniform vec4 Color;
uniform sampler2D u_Texture;

void main (void) 
{        
	fragColor = texture(u_Texture, v_TexCoord) * vec4(ourColor, 1.0f);
}
