//
// Gbuffer.vsh
//
// Copyright (c) 2011 Juha Pellinen
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#version 330

out vec3 v_pos;
out vec2 v_texCoord;
out mat3 v_normalMat;

out float u_CustomFloatAVar;	//value
out float u_CustomFloatBVar;	//auch value(gleich)
out float u_CustomFloatCVar;	//als id
//out float u_CustomFloatDVar;

//out flat int u_CustomIntAVar;
//out flat int u_CustomIntBVar;
//out flat int u_CustomIntCVar;
//out flat int u_CustomIntDVar;

uniform mat4 u_modelMatrix;
uniform mat4 u_modelViewProjectionMatrix;

in float u_CustomFloatA;
in float u_CustomFloatB;
in float u_CustomFloatC;
//in float u_CustomFloatD;

//noch versuchen als vektor zu definieren, oder besser auch nicht ?
//in int u_CustomIntA;
//in int u_CustomIntB;
//in int u_CustomIntC;
//in int u_CustomIntD;

in vec4 gl_Vertex;
in vec2 gl_MultiTexCoord0;

void main()
{
	gl_Position = u_modelViewProjectionMatrix * gl_Vertex;
	
	v_pos = (u_modelMatrix * gl_Vertex).xyz;
	v_texCoord = gl_MultiTexCoord0.xy;
	
	// NOTE: for some reason MacOSX seems to need it in this form.
	// v_normalMat = mat3(u_modelMatrix);
	v_normalMat = mat3(u_modelMatrix[0].xyz,
					   u_modelMatrix[1].xyz,
					   u_modelMatrix[2].xyz);
					   
	u_CustomFloatAVar=u_CustomFloatA;
	u_CustomFloatBVar=u_CustomFloatB;
	u_CustomFloatCVar=u_CustomFloatC;
	//u_CustomFloatDVar=u_CustomFloatD;
	
	
	//u_CustomIntAVar=u_CustomIntA;
	//u_CustomIntBVar=u_CustomIntB;
	//u_CustomIntCVar=u_CustomIntC;
	//u_CustomIntDVar=u_CustomIntD;
}
