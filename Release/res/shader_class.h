#pragma once

#include "glad/glad.h"

#include <string>
#include <fstream>
#include <iostream>

class Shader
{
public:

	unsigned int ID;

	Shader(const char* vertexPath, const char* fragmentPath)
	{
		std::string vertexCode;
		std::string fragmentCode;
		std::string read_string;
		try
		{
			std::ifstream vfile(vertexPath);
			std::ifstream ffile(fragmentPath);
			while (getline(vfile, read_string))
			{
				vertexCode += read_string + '\n';
			}
			while (getline(ffile, read_string))
			{
				fragmentCode += read_string + '\n';
			}
			vfile.close();
			ffile.close();
		}
		catch(std::ifstream::failure &e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const char* vSource = vertexCode.c_str();
		const char* fSource = fragmentCode.c_str();

		unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
		unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
		
		glShaderSource(vertex, 1, &vSource, NULL);
		glCompileShader(vertex);
		
		glShaderSource(fragment, 1, &fSource, NULL);
		glCompileShader(fragment);
		
		int success;
		char infoLog[512];

		//Check for compile error
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}	
		
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		
		//Check for link error
		glGetProgramiv(ID, GL_LINK_STATUS, &success);

		if (!success)
		{
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	void use()
	{
		glUseProgram(ID);
	}

	void setBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}

	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

};