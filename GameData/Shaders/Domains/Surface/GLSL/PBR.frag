#define PI 3.14159265359

in VS_OUT
{
   vec3 position_WS; // World space position
   vec3 position_VS; // View space position

   vec3 normal_WS;   // World space normal

   vec3 tangent_WS;  // World space tangent
   vec3 bitangent_WS; // World space bitangent

   vec2 uv;
} fsIn;

layout(std140, binding = 1) uniform SceneBlock
{
   vec3     uCameraPosition_WS;
   float    uCameraNearPlane;
   float    uCameraFarPlane;
};

struct Surface
{
   vec3  baseColor;
   float metallic;
   float roughness;
   float opacity;
   vec3  normal;
} surface;

vec2 ParallaxOcclusionMapping(sampler2D heightmap, vec3 viewPos_WS, vec2 mapScale, float heightScale)
{
    vec2 uv = fsIn.uv;

    vec3 viewDirection_WS   = normalize(viewPos_WS - fsIn.position_WS);
    
    float viewDistance = length(viewPos_WS - fsIn.position_WS);

    mat3 TNB                = mat3(fsIn.tangent_WS, fsIn.normal_WS, fsIn.bitangent_WS);

    vec3 viewDirection_TS   = normalize(inverse(TNB)*viewDirection_WS);

    float minLayers = 8.0;
    float maxLayers = 32.0;
    float numLayers = mix(minLayers, maxLayers, max(dot(vec3(0.0, 1.0, 0.0), viewDirection_TS), 0.0));

    float lod = textureQueryLod(heightmap, uv * mapScale).y;

    float heightStep        = 1.0 / numLayers;
    float sampledHeight     = textureLod(heightmap, uv * mapScale, lod).r;
    float prevSampledHeight = sampledHeight;
    float layerHeight       = 0.0;
    float prevLayerHeight   = layerHeight;

    vec2 prevUV             = uv;
    while(sampledHeight > layerHeight)
    {
        prevLayerHeight     = layerHeight;
        layerHeight         += heightStep;

        vec2 uvOffset_TS    = (viewDirection_TS.xz / viewDirection_TS.y) * layerHeight * 0.0001 * heightScale;
        prevUV              = uv;
        uv                  += uvOffset_TS;

        prevSampledHeight   = sampledHeight;
        sampledHeight       = textureLod(heightmap, uv * mapScale, lod).r;
    }

    float t1     = sampledHeight - layerHeight;
    float t2     = prevSampledHeight - prevLayerHeight;

    float weight = t1 / (t1 - t2);

    uv = mix(uv, prevUV, weight);

    return uv;
}

#pragma __USER_CODE__

struct DirectionalLight
{
    vec3    color;
    float   pad0;

    vec3    direction;
    float   pad4;

    float   intensity;
    float   pad1, pad2, pad3;
};

struct PointLight
{
    vec3    color;
    float   intensity;

    vec3    worldPosition;
    float   range;

    float   falloffFactor;
    float   pad1, pad2, pad3;
};

struct SpotLight
{
    vec3    color;
    float   intensity;

    vec3    worldPosition;
    float   range;

    vec3    direction;
    float   falloffFactor;

    float   outerCosAngle;
    float   innerCosAngle;
    float   pad0, pad1;
};

struct Cluster
{
    uvec4       lightIndexBlock[128];
    uint        lightCount;
    uint        pad0, pad1, pad2;
};

float NormalDistributionFunction(vec3 surfaceNormal, vec3 halfwayDirection, float roughness)
{
    float roughnessSquared = roughness*roughness;
    float NdotH            = max(dot(surfaceNormal, halfwayDirection), 0.0);
    float NdotH2           = NdotH*NdotH;

    float nom              = roughnessSquared;
    float denom            = ( NdotH2*( roughnessSquared-1.0 )+1.0 );
    denom                  = PI*denom*denom;

    return nom/denom;
}

float GeometrySchlickGGX(float NdotV, float k)
{
    float nom      = NdotV;
    float denom    = NdotV*( 1.0-k )+k;

    return nom/denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float k)
{
    float NdotV     = max(dot(N, V), 0.0);
    float NdotL     = max(dot(N, L), 0.0);
    float ggx1     = GeometrySchlickGGX(NdotV, k);
    float ggx2      = GeometrySchlickGGX(NdotL, k);

    return ggx1*ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
   return F0+( 1.0-F0 )*pow(1.0-cosTheta, 5.0);
}

vec3 CookTorranceBRDF(vec3 lightDirection, vec3 viewDirection_WS)
{
    vec3 halfwayDirection   = normalize(viewDirection_WS + lightDirection);

    vec3 F0                 = vec3(0.04);
    F0                      = mix(F0, surface.baseColor, surface.metallic);

    float   D               = NormalDistributionFunction(surface.normal, halfwayDirection, surface.roughness);
    vec3    F               = FresnelSchlick(max(0.0, dot(halfwayDirection, viewDirection_WS)), F0);
    float   G               = GeometrySmith(surface.normal, viewDirection_WS, lightDirection, surface.roughness);

    vec3    fLambert        = surface.baseColor / PI;
    vec3    fSpecular       = (D*F*G) / (4.0 * max(dot(surface.normal, viewDirection_WS), 0.0) * max(dot(surface.normal, lightDirection), 0.0) + 0.0001);

    vec3    kS              = F;
    vec3    kD              = vec3(1.0)-kS;
    kD                      *= 1.0-surface.metallic;

    return ((kD*fLambert) + (fSpecular));
}

layout(std140, binding = 0) buffer ClusterDataBlock
{
    uvec2   uClusterPixelSize;
    uint    uNumSlices;
    uint    uNumClustersPerSlice;

    uint    uNumClustersPerRow;
    
    Cluster uClusters[];
};

layout(std140, binding = 1) buffer DirectionalLightBlock
{
    uint                uDirectionalLightsCount;
//  uint                pad0, pad1, pad2;

    DirectionalLight    uDirectionalLights[];
};

layout(std140, binding = 2) buffer PointLightBlock
{
    uint                uPointLightsCount;
//  uint                pad0, pad1, pad2;

    PointLight          uPointLights[];
};

layout(std140, binding = 3) buffer SpotLightBlock
{
    uint                uSpotLightsCount;
//  uint                pad0, pad1, pad2;

    SpotLight           uSpotLights[];
};

layout(location = 0) out vec4 outTargetColor;

// layout(binding = 8) uniform sampler3D uVoxelTexture;

void main()
{
    surface.baseColor = vec3(0.0, 0.0, 0.0);
    surface.roughness = 1.0;
    surface.metallic  = 0.0;
    surface.opacity   = 1.0;
    surface.normal    = fsIn.normal_WS;

    Main();

    vec3 viewDirection_WS   = normalize(uCameraPosition_WS - fsIn.position_WS);
    vec3 outgoingIrradiance = vec3(0.0);

    for(uint l = 0; l < uDirectionalLightsCount; l++)
    {
        vec3 lightDirection     = normalize(-uDirectionalLights[l].direction);

        vec3 brdf               = CookTorranceBRDF(lightDirection, viewDirection_WS);

        vec3 lightRadiance      = uDirectionalLights[l].color * uDirectionalLights[l].intensity;

#if defined(VOXEL_WRITE)
        
#endif

        outgoingIrradiance      += brdf * lightRadiance * max(0.0, dot(surface.normal, lightDirection));
    }

    float sliceIndex = floor(
        ( log(fsIn.position_VS.z) * ( uNumSlices/log(uCameraFarPlane/uCameraNearPlane) ) )
        -
        ( uNumSlices*log(uCameraNearPlane)/log(uCameraFarPlane/uCameraNearPlane) )
    );

    uint clusterXIdx = uint(floor(gl_FragCoord.x/float(uClusterPixelSize.x)));
    uint clusterYIdx = uint(floor(gl_FragCoord.y/float(uClusterPixelSize.y)));

    uint clusterIndex = ( uint(sliceIndex)*uNumClustersPerSlice ) + (uNumClustersPerRow*clusterYIdx) + clusterXIdx;

    for(uint l = 0; l < uClusters[clusterIndex].lightCount; l++)
    {
        uint vecIdx     = uint(floor(l/4.0));
        uint compIdx    = l % 4;

        uint lightIndex = uClusters[clusterIndex].lightIndexBlock[vecIdx][compIdx];

        PointLight light = uPointLights[lightIndex];

        vec3    lightDirection  = normalize(light.worldPosition - fsIn.position_WS);

        vec3    brdf            = CookTorranceBRDF(lightDirection, viewDirection_WS);

        float   distance        = length(light.worldPosition - fsIn.position_WS);
        float   T1              = max(( -distance/light.range )+1.0, 0.0);
        float   T2              = T1 / ( distance*distance+1.0 );
        float   attenuation     = (light.falloffFactor * T1) + ((1.0 - light.falloffFactor) * T2);

        vec3    lightRadiance   = light.color * light.intensity * attenuation;

        outgoingIrradiance      += brdf * lightRadiance * max(0.0, dot(surface.normal, lightDirection));
    }

    // for(uint l = 0; l < uSpotLightsCount; l++)
    // {
    //     vec3    lightDirection      = normalize(uSpotLights[l].worldPosition - fsIn.position_WS);

    //     vec3    brdf                = CookTorranceBRDF(lightDirection, viewDirection_WS);

    //     float   distance            = length(uSpotLights[l].worldPosition - fsIn.position_WS);
    //     float   T1                  = max(( -distance/uSpotLights[l].range )+1.0, 0.0);
    //     float   T2                  = T1 / ( distance*distance+1.0 );
    //     float   distanceAttenuation = (uSpotLights[l].falloffFactor * T1) + ((1.0 - uSpotLights[l].falloffFactor) * T2);

    //     float   cosAngle            = max(dot(lightDirection, -normalize(uSpotLights[l].direction)), 0.0);        
    //     float   epsilon             = uSpotLights[l].innerCosAngle - uSpotLights[l].outerCosAngle;
    //     float   angleAttenuation    = clamp((cosAngle - uSpotLights[l].outerCosAngle) / (epsilon), 0.0, 1.0);

    //     vec3 lightRadiance          = uSpotLights[l].color * uSpotLights[l].intensity * distanceAttenuation * angleAttenuation;

    //     outgoingIrradiance          += brdf * lightRadiance * max(0.0, dot(surface.normal, lightDirection));
    // }

    outTargetColor = vec4(outgoingIrradiance, surface.opacity);
}