#ifndef VAO_H
#define VAO_H

#include<glad/glad.h>
#include"VBO.h"

class VAO {
public:
	GLuint ID;
	VAO();

	void Link_atrib(VBO& VBO, GLuint layout, GLuint components, GLenum type, GLsizeiptr dtride, void* offset);
	void Bind();
	void Unbind();
	void Delete();
};

#endif