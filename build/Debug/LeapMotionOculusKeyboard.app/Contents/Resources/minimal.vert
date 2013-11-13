//void main() {
//    // transform the vertex position
//    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
//    
//    // transform the texture coordinates
//    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
//}

varying vec3 N;
varying vec3 I;
//varying vec4 Cs;

void main()
{
	vec4 P = gl_ModelViewMatrix * gl_Vertex;
	I  = P.xyz - vec3 (0);
	N  = gl_NormalMatrix * gl_Normal;
	//Cs = gl_Color;
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}