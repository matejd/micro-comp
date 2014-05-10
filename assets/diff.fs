varying vec2 vuv;
uniform sampler2D fb;

void main()
{
    vec3 linearLeft  = texture2D(fb, vuv*vec2(0.5, 1.0)).rgb;
    vec3 linearRight = texture2D(fb, vuv*vec2(0.5, 1.0)+vec2(0.5, 0.0)).rgb;
    vec3 color = 10.0*abs(linearRight-linearLeft);
    gl_FragColor.rgb = color;
}
