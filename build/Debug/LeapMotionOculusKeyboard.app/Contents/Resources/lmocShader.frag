uniform sampler2D texture;

varying vec2 v_texCoord;

varying vec4 v_debugTest;

void main()
{
    // lookup the pixel in the texture
    vec4 pixel = texture2D(texture, v_texCoord);
    
    //and draw the texture color, no lighting
    gl_FragColor = pixel*0.5+v_debugTest*0.5;
}