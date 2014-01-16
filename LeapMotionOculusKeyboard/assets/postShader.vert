varying vec2 u_texCoord;

uniform mat4 u_modelViewProjectionMatrix;

void main()
{
	u_texCoord = gl_MultiTexCoord0.xy;
	gl_Position = u_modelViewProjectionMatrix*gl_Vertex;
}
