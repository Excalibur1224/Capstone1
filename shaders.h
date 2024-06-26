#ifndef SHADERS_H
#define SHADERS_H
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>

std::string get_file_contents(const char* filename);

class Shader {
	public:
		GLuint ID; //try to make private if possible
		Shader(const char* vertexFile, const char* fragmentFile);

		void Activate();
		void Delete();
};

#endif