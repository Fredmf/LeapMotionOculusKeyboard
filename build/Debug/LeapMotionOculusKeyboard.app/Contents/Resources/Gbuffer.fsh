//
// GBuffer.fsh
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

in vec3 v_pos;
in vec2 v_texCoord;
in vec3 v_normal;
in float v_material;

in float u_CustomFloatAVar;
in float u_CustomFloatBVar;
in float u_CustomFloatCVar;
//in float u_CustomFloatDVar;

//in flat int u_CustomIntAVar;
//in flat int u_CustomIntBVar;
//in flat int u_CustomIntCVar;
//in flat int u_CustomIntDVar;

//uniform sampler2D texture;
//uniform sampler2D normalmap;
//uniform float u_material;

out  vec4 gbuf0;
out  vec4 gbuf1;
out  vec4 gbuf2;
//out ivec4 gbuf3;

void main()
{
	// read normal from object space normal map.
	//vec3 normal = texture2D(normalmap, v_texCoord).xyz * 2.0 - 1.0;


	gbuf0 = vec4(v_pos, u_CustomFloatAVar);
	gbuf1 = vec4(v_normal, u_CustomFloatBVar);
	gbuf2 = vec4(vec3(1.0,1.0,1.0), u_CustomFloatCVar);
	//gbuf0 = vec4(vec3(1.0,0.0,0.0), 1);
	//gbuf1 = vec4(vec3(0.0,1.0,0.0), 1);
	//gbuf2 = vec4(vec3(0.0,0.0,1.0), 1);
	//gbuf3 = ivec4(u_CustomIntAVar, u_CustomIntBVar, u_CustomIntCVar, u_CustomIntDVar); //NewGBuffer
}
