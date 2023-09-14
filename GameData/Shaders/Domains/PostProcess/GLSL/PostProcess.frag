out vec4 oColor;

layout(location = 0) in vec2 iUV;

#pragma __USER_CODE__

void main()
{
    vec3 color  = GetOutputColor();

    oColor      = vec4(color, 1.0);
}