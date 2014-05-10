varying vec3 vnormal;
varying vec3 vview;
uniform vec3 kd;
uniform vec3 F0;
uniform float roughness;

const float PI = 3.14159265;

void main()
{
    vec3 wn = normalize(vnormal);
    vec3 wo = normalize(vview);
    vec3 wi = normalize(vec3(0.35, 0.45, 0.9));
    vec3 wh = normalize(wo + wi);

    float eps = 0.01;
    float din = max(dot(wi, wn), eps);
    float don = max(dot(wo, wn), eps);
    float dih = max(dot(wi, wh), eps);

    float D = evaluateD(wh, wn, roughness);
    float G = evaluateG(wi, wo, wh, wn, roughness);
    vec3 F  = evaluateF(F0, dih);
    vec3 spec = D * G * F / (4.0 * din * don);

    vec3 irradiance = vec3(2.0) * PI * din;
    vec3 radiance = (kd/PI + spec) * irradiance;
    gl_FragColor.rgb = radiance;
}
