//
// Screen.fsh
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

in vec2 v_texCoord;

//uniform int u_numLights;
//uniform vec3 u_lightPositions[128];
//uniform vec3 u_lightColors[128];

uniform vec4 u_lensPos[10];
uniform int u_lensCount;
uniform vec3 u_lensMM[10];
uniform int masterlens;

uniform vec3 u_eyePos;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
//uniform isampler2D texture3; //NewGBuffer


uniform float u_UniMin;
uniform float u_UniMax;

uniform sampler1D colorMap;

uniform int banded;
uniform int num_bands;

out vec4 finalImage;

vec4 mapLookupValueToColor(in float lookupValue){
	vec4 mappedColor;
	float bandedValue;

	if(banded!=0){
		bandedValue = floor(lookupValue*num_bands) / (num_bands - 1.0);
	}else{
		bandedValue=lookupValue;
	}

	mappedColor = texture(colorMap, bandedValue);           
	return mappedColor;
}

void main()
{
	//handel min max input
	float globalMax;
	float globalMin;

	if(masterlens==1 && u_lensCount>0){
		globalMax=u_lensMM[0].x;
		globalMin=u_lensMM[0].y;
	}else{
		globalMax=u_UniMax;
		globalMin=u_UniMin;
	}

	//handel gbuffers
	vec4 gbuf0=texture2D(texture0, v_texCoord);
	vec4 pos = vec4(gbuf0.xyz,1.0);
	float occ = gbuf0.w;//customValA

	vec4 gbuf1=texture2D(texture1, v_texCoord);
	vec4 normal = vec4(gbuf1.xyz,0.0);//könnte man noch mit normalize verbinden
	//float cValB = gbuf1.w;//customValB

	vec4 gbuf2=texture2D(texture2, v_texCoord);
	vec4 albedo = vec4(gbuf2.xyz,1.0);
	float value = gbuf2.w;//zu falschfarbender wert

	// normalize custom value between 0.0 and 1.0
	//global
	float normValue;
	float minmax = globalMax-globalMin;
	if (minmax!=0){
		normValue = ((value-globalMin)/(minmax))+1/256; //+1/256 um genau im texel zu landen
	}else{
		normValue = 0.0;
	}

	//local pro linse, überschreibt global, kann optimiert werden
	for (int i=0;i<u_lensCount;i++){
		//coordinaten der linse eingrenzen
		if (v_texCoord.x >= u_lensPos[i].x && v_texCoord.x <= u_lensPos[i].x+u_lensPos[i].z){
			if (v_texCoord.y >= u_lensPos[i].y && v_texCoord.y <= u_lensPos[i].y+u_lensPos[i].w){
				//normCustomVal für den bereuch bestimmen, sonst wird der globale normCustomVal genommen
				minmax = u_lensMM[i].x-u_lensMM[i].y;
				if (minmax!=0){
					normValue = (value-u_lensMM[i].y)/(u_lensMM[i].x-u_lensMM[i].y);
				}else{
					normValue=0.0;
				}
			}
		}
	}

	//fabwert nach falschfarbenscala bestimmen mit dem normalisierten Value
	vec4 ffalbedo;//falschfarb albedo
	//farbscala am oberen Bildschirmrand
	if (v_texCoord.y > 0.98){
		//vec4 ffband = texture(colorMap, v_texCoord.x);
		vec4 ffband = mapLookupValueToColor(v_texCoord.x);
		finalImage=ffband;
	}else{
		ffalbedo = mapLookupValueToColor(normValue);
		//ffalbedo = texture(colorMap, normValue);
		if(albedo.x == 0.0){
			finalImage = vec4(0.0,0.0,0.0,0.0);
		}else{
			finalImage=ffalbedo*occ; //occ intensität sollte noch einstellbar sein

			//um nur das model dazustellen
			//vec4 occ_color=mix(vec4(0.0,0.0,0.0,1.0),vec4(1.0,1.0,1.0,1.0),occ);
			//finalImage=occ_color;
		}
	}
}


	//vec3 eyeDir = normalize(u_eyePos - pos);
	// calculate lights.
	//vec4 light = vec4(0, 0, 0, 1);

	//for (int i = 0; i < u_numLights; ++i)
	//{
	//	vec3 lightDir = normalize(u_lightPositions[i] - pos);
	//	vec3 halfVec = normalize(lightDir + eyeDir);
	//	float diffuseTerm = max(0.0, dot(normal, lightDir));
	//	float specularTerm = pow(max(0.5, dot(normal, halfVec)), 64.0);

	//	light.rgb += u_lightColors[i] * (diffuseTerm + specularTerm * 0.5) * 0.66;
	//}
	//finalImage = mix(light * albedo, vec4(1, 1, 1, 1), material);
	//finalImage = mix(vec4(0,0,0,0)+normCustomVal, vec4(1, 1, 1, 1), material);
	//finalImage = mix(vec4(0,0,0,0)+customVal, vec4(1, 1, 1, 1), material);