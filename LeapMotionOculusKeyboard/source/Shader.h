#ifndef _SHADER_H_
#define _SHADER_H_

#ifdef __APPLE__
#include <SFML/OpenGL.hpp>
#elif WIN32
#include <GL/glew.h>
#include <GL/gl.h>
#endif

#include <string>

bool loadShader(GLuint &program, std::string vertexFile, std::string fragmentFile);
#endif /* _SHADER_H_ */