layout(binding = 0) uniform sampler2D   uBaseColorMap;

void Main()
{
    surface.baseColor   = texture(uBaseColorMap, fsIn.uv).rgb;
    surface.roughness   = 1.0;
    surface.metallic    = 0.0;
    surface.opacity     = 1.0;
}