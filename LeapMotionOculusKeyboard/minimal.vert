//// minimal vertex shader
//// www.lighthouse3d.com
//
//uniform mat4 u_modelMatrix;
//uniform mat4 u_modelViewProjectionMatrix;
//
//in vec4 gl_Vertex;
//in vec2 gl_MultiTexCoord0;
//
//void main()
//{	
//
//	// the following three lines provide the same result
//
////	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
////	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
//	//gl_Position = ftransform();
//    
//    // transform the vertex position
//    
//    // transform the texture coordinates
//    
//    // forward the vertex color
//    gl_FrontColor = gl_Color;
//    
//    
//    gl_TexCoord[0] = gl_MultiTexCoord0;
//    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
//}

void main() {
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
