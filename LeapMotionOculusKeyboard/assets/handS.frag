// vertex to fragment shader io
varying vec3 N;
varying vec3 I;

uniform sampler2D texture;

// globals
uniform float edgefalloff;
uniform float intensity;
uniform float ambient;

// entry point
void main()
{
    float opac = dot(normalize(-N), normalize(-I));
    opac = abs(opac);
    opac = ambient + intensity*(1.0-pow(opac, edgefalloff));
    //opac = 1.0 - opac;
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
    gl_FragColor =  opac * pixel;
    gl_FragColor.a = opac;
}