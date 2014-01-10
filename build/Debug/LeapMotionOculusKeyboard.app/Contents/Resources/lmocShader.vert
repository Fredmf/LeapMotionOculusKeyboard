uniform mat4 u_modelMatrix;
uniform mat4 u_lookAtMatrix;
uniform mat4 u_perspectiveMatrix;

varying float u_keyId;
uniform int isKeyboard;

varying vec2 v_texCoord;
varying vec4 v_debugTest;

void main() {
    gl_Position = u_perspectiveMatrix * u_lookAtMatrix * u_modelMatrix * gl_Vertex;
    v_debugTest=vec4(u_keyId/48.0,0.0,0.0,0.0);
    v_texCoord = gl_MultiTexCoord0.xy;
}
