#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{   
    vec4 texColor = texture(ourTexture, TexCoord);
    if (texColor.a == 0.){
        discard;
    }else{
        FragColor = texColor;
    }
}
