uniform mat4 u_modelMatrix;
uniform mat4 u_lookAtMatrix;
uniform mat4 u_perspectiveMatrix;

void main() {
    gl_Position = u_perspectiveMatrix * u_lookAtMatrix * u_modelMatrix * gl_Vertex;

    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
}
