#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Shader
{
public:
	GLuint Program;

	Shader(const GLchar *vertexPath, const GLchar *fragmentPath)
	{
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;

		vShaderFile.exceptions(std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::badbit);

		try
		{
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;

			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();

			vShaderFile.close();
			fShaderFile.close();

			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR:SHADER::FILE_NOT_SUCCESFULLY_READ\n" << std::endl;
		}

		const GLchar* vShaderCode = vertexCode.c_str();
		const GLchar* fShaderCode = fragmentCode.c_str();

		GLuint vertex, fragment;
		GLint success;
		GLchar infoLog[512];

		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);

		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success) 
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);

		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success) 
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		this->Program = glCreateProgram();
		glAttachShader(this->Program, vertex);
		glAttachShader(this->Program, fragment);
		glLinkProgram(this->Program);

		glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}

		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	void Use()
	{
		glUseProgram(this->Program);
	}

	void UseUniform()
	{
		GLfloat valueTime = glfwGetTime();
		GLfloat greenValue = (sin(valueTime) / 2.0f) + 0.5f;
		GLint uniformLocation = glGetUniformLocation(this->Program, "uniformColor");
		glUseProgram(this->Program);
		glUniform4f(uniformLocation, 0.0f, greenValue, 0.0f, 1.0f);
	}

	void setFloat(const std::string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(this->Program, name.c_str()), value);
	}

	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(this->Program, name.c_str()), value);
	}

	void setMatrix(const std::string& name, const GLfloat *matrix)
	{
		glUniformMatrix4fv(glGetUniformLocation(this->Program, name.c_str()), 1, GL_FALSE, matrix);
	}

	void setVec3(const std::string& name, float x, float y, float z)
	{
		glUniform3f(glGetUniformLocation(this->Program, name.c_str()), x, y, z);
	}

	void setVec3(const std::string& name, glm::vec3 vector)
	{
		glUniform3f(glGetUniformLocation(this->Program, name.c_str()), vector.x, vector.y, vector.z);
	}
};

#endif