uniform sampler2D texture;

varying vec2 v_texCoord;

varying vec4 v_debugTest;

uniform int isKeyboard;

void main()
{
    // lookup the pixel in the texture
    vec4 pixel = texture2D(texture, v_texCoord);
    
    //and draw the texture color, no lighting
    if (isKeyboard==1) {
        gl_FragColor = pixel+v_debugTest*0.3;
    }else{
        gl_FragColor = pixel*0.6;
    }
    
    
    //gl_FragColor = v_debugTest;
}