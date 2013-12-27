uniform sampler2D texture;

varying vec2 v_texCoord;
void main()
{
    // lookup the pixel in the texture
    vec4 pixel = texture2D(texture, v_texCoord);
    
    // multiply it by the color
    //and visualize texCoords for debug
    gl_FragColor = pixel;
}