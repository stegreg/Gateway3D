layout(binding = 0) uniform sampler2D   uBaseColorMap;
layout(binding = 1) uniform sampler2D   uRoughnessMap;
layout(binding = 2) uniform sampler2D   uNormalMap;
layout(binding = 3) uniform sampler2D   uMetallicMap;
layout(binding = 4) uniform sampler2D   uDisplacementMap;

uniform vec2        uMapScale;
uniform float       uHeightScale;

void Main()
{
    vec2 uv = ParallaxOcclusionMapping(uDisplacementMap, uCameraPosition_WS, uMapScale, uHeightScale);

    surface.baseColor   = texture(uBaseColorMap, uv * uMapScale).rgb;
    surface.roughness   = texture(uRoughnessMap, uv * uMapScale).r;
    surface.metallic    = texture(uMetallicMap, uv * uMapScale).r;
    surface.opacity     = 1.0;

    mat3 TNB            = mat3(normalize(fsIn.tangent_WS), normalize(fsIn.normal_WS), normalize(fsIn.bitangent_WS));
    vec3 normalSample   = texture(uNormalMap, uv * uMapScale).rbg;
    normalSample        = (normalSample*2.0) - 1.0;
    normalSample.xz     *= 0.3; //https://computergraphics.stackexchange.com/questions/5411/correct-way-to-set-normal-strength
    
    surface.normal      = normalize(TNB * normalSample);
}