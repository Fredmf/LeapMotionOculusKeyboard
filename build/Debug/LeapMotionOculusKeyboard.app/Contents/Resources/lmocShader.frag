uniform sampler2D texture;

varying vec2 v_texCoord;
void main()
{
    // lookup the pixel in the texture
    vec4 pixel = texture2D(texture, v_texCoord);
    
    //and draw the texture color, no lighting
    gl_FragColor = pixel;
}