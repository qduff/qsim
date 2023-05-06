#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D osdtex;

void main()
{   
    vec4 texColor = texture(osdtex, TexCoord);
    if (texColor.a == 0.){
        discard;
    }else{
        FragColor = texColor;
    }
}
