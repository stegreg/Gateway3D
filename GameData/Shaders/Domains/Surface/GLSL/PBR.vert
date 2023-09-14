layout(location = 0) in vec3   VERTEX_POSITION;
layout(location = 1) in vec3   VERTEX_NORMAL;
layout(location = 2) in vec3   VERTEX_TANGENT;
layout(location = 3) in vec2   VERTEX_UV;

layout(location = 4) in mat4   INSTANCE_WORLD_MATRIX;

#pragma __USER_CODE__

layout(std140, binding = 0) uniform CameraBlock
{
   mat4  uView;
   mat4  uProjection;
};

out VS_OUT
{
   vec3 position_WS;    // World space position
   vec3 position_VS;    // View space position

   vec3 normal_WS;      // World space normal

   vec3 tangent_WS;     // World space tangent
   vec3 bitangent_WS;   // World space bitangent

   vec2 uv;             // uv coordinate 0
} vsOut;

void main()
{
   vec4 worldPos        = ( vec4(VERTEX_POSITION, 1.0)*INSTANCE_WORLD_MATRIX );
   vec4 viewPos         = worldPos * uView;
   vec4 clipPos         = viewPos  * uProjection;

   vsOut.normal_WS      = normalize(( vec4(VERTEX_NORMAL, 0.0)  * INSTANCE_WORLD_MATRIX ).xyz);
   
   vsOut.tangent_WS     = normalize(( vec4(VERTEX_TANGENT, 0.0) * INSTANCE_WORLD_MATRIX ).xyz);
   vsOut.bitangent_WS   = normalize(cross(vsOut.tangent_WS, vsOut.normal_WS));

   vsOut.position_WS    = worldPos.xyz;
   vsOut.position_VS    = viewPos.xyz;

   vsOut.uv             = VERTEX_UV;

   gl_Position          = clipPos;
}