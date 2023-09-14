uniform sampler2D uColorTexture;

vec3 GetOutputColor()
{
    vec3 sampleColor = texture(uColorTexture, iUV).rgb;

    float exposure   = 0.5;
    vec3 ldr         = vec3(1.0) - exp(-sampleColor * exposure);
    ldr              = pow(ldr, vec3(1.0/2.2));

    return ldr;
}