uniform mat4 u_modelMatrix;
uniform mat4 u_lookAtMatrix;
uniform mat4 u_perspectiveMatrix;

float u_keyId;
uniform int isKeyboard;

varying vec2 v_texCoord;

void main() {
    gl_Position = u_perspectiveMatrix * u_lookAtMatrix * u_modelMatrix * gl_Vertex;
    
    v_texCoord = gl_MultiTexCoord0.xy;
}
