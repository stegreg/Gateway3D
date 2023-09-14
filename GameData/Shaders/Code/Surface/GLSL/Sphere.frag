uniform vec3 uBaseColor;

void Main()
{
    surface.baseColor   = uBaseColor;
    surface.roughness   = 1.0;
    surface.metallic    = 0.0;
    surface.opacity     = 1.0;
}