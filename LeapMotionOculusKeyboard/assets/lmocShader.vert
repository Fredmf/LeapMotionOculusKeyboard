uniform mat4 u_modelMatrix;

uniform mat4 u_ViewMatrixL;
uniform mat4 u_ProjectionMatrixL;

uniform mat4 u_ViewMatrixR;
uniform mat4 u_ProjectionMatrixR;

// 4*4 enought space for informations about 16 buttons, so you can press 16 buttons at the same time
uniform mat4 pressedButtons;

attribute float u_keyId;
uniform int isKeyboard;
uniform int isLeft;

varying vec2 v_texCoord;
varying vec4 v_debugTest;

void main() {
    //change button color while pressen (keyboard only)
    
    int buttonFound=0;
    if (isKeyboard != 0) {
        for (int i = 0; i<4; i++) {
            for (int j = 0; j<4; j++) {
                if (pressedButtons[i][j] != 0.0 && u_keyId > pressedButtons[i][j]-0.5 && u_keyId < pressedButtons[i][j]+0.5) {
                    v_debugTest=vec4(0.56,0.0,1.0,0.0);
                    mat4 newModelMatrix = u_modelMatrix;
                    newModelMatrix[3][1]=-0.3;
                    if (isLeft==1) {
                        gl_Position = u_ProjectionMatrixL * u_ViewMatrixL * newModelMatrix * gl_Vertex;
                    }else{
                        gl_Position = u_ProjectionMatrixR * u_ViewMatrixR * newModelMatrix * gl_Vertex;
                    }
                    buttonFound=1;
                }
            }
        }
    }
    if (buttonFound == 0) {
        v_debugTest=vec4(0.0,0.0,0.0,0.0);
        if (isLeft==1) {
            gl_Position = u_ProjectionMatrixL * u_ViewMatrixL * u_modelMatrix * gl_Vertex;
        }else{
            gl_Position = u_ProjectionMatrixR * u_ViewMatrixR * u_modelMatrix * gl_Vertex;
        }
    }
    v_texCoord = gl_MultiTexCoord0.xy;
}
