#ifndef SHADER_H
#define SHADER_H

#include<GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

class Shader {
private:
	string VertexSource;
	string FragSource;
public:
	Shader(const string& filepath);
	unsigned int CompileShader(unsigned int type, const string& source);
	unsigned int CreateShader();
};


#endif
