//
// Shader.cpp
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
//

#include "Shader.h"

#include <fstream>
#include <string>
using std::ifstream;

static char *loadFile(const char *filename)
{
	ifstream file(filename, ifstream::binary);

	if (!file.is_open())
		return NULL;

	// file length.
	file.seekg(0, ifstream::end);
	int length = (int)file.tellg();
	file.seekg(0);
	
	char *buf = new char[length+1];
	file.read(buf, length);
	buf[length] = '\0';
	
	return buf;
}

GLuint compileShader(GLenum type, const char *src)
{
	GLuint shader = glCreateShader(type);
	
	if (!shader)
		return 0;
	
	glShaderSource(shader, 1, &src, NULL);

	glCompileShader(shader);
	
	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

	GLint infoLen = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
	if (infoLen > 1)
	{
		char* infoLog = (char *)malloc(sizeof(char) * infoLen);

		glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
		printf("Compile info shader:\n%s\n", infoLog);

		free(infoLog);
	}

	if (!compiled) 
	{
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

bool loadShader(GLuint &program, std::string vertexFile, std::string fragmentFile)
{
	char *vSrc = loadFile(vertexFile.c_str());
	char *fSrc = loadFile(fragmentFile.c_str());
	
	if (!vSrc || !fSrc)
		return false;

	GLuint vShader = compileShader(GL_VERTEX_SHADER, vSrc);
	GLuint fShader = compileShader(GL_FRAGMENT_SHADER, fSrc);
	delete[] vSrc;
	delete[] fSrc;

	if (vShader == 0 || fShader == 0)
		return false;
		
	program = glCreateProgram();
	if (program == 0)
		return false;
	
	glAttachShader(program, vShader);
	glAttachShader(program, fShader);
	
	// link.
	glLinkProgram(program);
		
	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	
	if (!linked)
	{
		GLint infoLen = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 1)
		{
			char* infoLog = (char *)malloc(sizeof(char) * infoLen);
			
			glGetProgramInfoLog(program, infoLen, NULL, infoLog);
			printf("Error linking program:\n%s\n", infoLog);
			
			free(infoLog);
		}
		glDeleteProgram(program);
		return false;
	}
		
	// clean up.
	glDeleteShader(vShader);
	glDeleteShader(fShader);
		
	return true;
}

