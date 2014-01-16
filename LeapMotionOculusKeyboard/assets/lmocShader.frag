uniform sampler2D texture;

varying vec2 v_texCoord;

varying vec4 v_buttonColor;

uniform int isKeyboard;
uniform int isTransparent;

void main()
{
    // lookup the pixel in the texture
    vec4 pixel = texture2D(texture, v_texCoord);
    
    //and draw the texture color, no lighting
    if (isKeyboard==1) {
        gl_FragData[0] = pixel+v_buttonColor*0.3;
    }else if(isTransparent==1){
        gl_FragData[0] = pixel*0.6;
    }else{
        gl_FragData[0] = pixel;
    }
    
    //gl_FragColor = v_debugTest;
}