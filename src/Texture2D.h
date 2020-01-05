#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>

#include <GL/glew.h>
#include <SOIL2/SOIL2.h>

class Texture2D
{
public:
	GLuint Texture;

	Texture2D(const GLchar *texturePath)
	{
		glGenTextures(1, &Texture);
		glBindTexture(GL_TEXTURE_2D, Texture);

        Texture = SOIL_load_OGL_texture(texturePath, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

		if(0 == Texture) 
        {
            std::cout << "ERROR::SOIL::TEXTURE_LOADING\n" <<  SOIL_last_result() << std::endl;
        }
	}

	static void SetParameter(GLenum pName, GLint value)
	{
		glTexParameteri(GL_TEXTURE_2D, pName, value);
	}

	static void SetParameter(GLenum pName, GLfloat *value)
	{
		glTexParameterfv(GL_TEXTURE_2D, pName, value);
	}

	void Bind()
	{
		glBindTexture(GL_TEXTURE_2D, this->Texture);
	}

	void Bind(GLenum activeTexture)
	{
		glActiveTexture(activeTexture);
		glBindTexture(GL_TEXTURE_2D, this->Texture);
	}
};

#endif