layout(location = 0) out vec2 oUV;

void main()
{
    switch(gl_VertexID)
    {
    case 0:
        gl_Position = vec4(-1.0, -1.0, 0.0, 1.0);
        oUV = vec2(0.0, 0.0);
        break;
    case 1:
        gl_Position = vec4(3.0, -1.0, 0.0, 1.0);
        oUV = vec2(2.0, 0.0);
        break;
    case 2:
        gl_Position = vec4(-1.0, 3.0, 0.0, 1.0);
        oUV = vec2(0.0, 2.0);
        break;
    }
}