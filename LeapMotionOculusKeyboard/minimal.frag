//// minimal fragment shader
//// www.lighthouse3d.com
//uniform sampler2D texture;
//
//in vec3 v_pos;
//in vec2 v_texCoord;
//in vec3 v_normal;
//in float v_material;
//
//void main()
//{
//    // lookup the pixel in the texture
//    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
//    
//    // multiply it by the color
//    gl_FragColor = gl_Color * pixel;
//}


void main() {
   gl_FragColor = vec4(1,0,0,1);
}