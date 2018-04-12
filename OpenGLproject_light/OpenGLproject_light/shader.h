#pragma once
#include<iostream>
#include<string>
#include<fstream>
#include<sstream>
#include<glad\glad.h>
#include "Init.h"

class Shader {
public:	
	unsigned int ID;//program ID

	Shader(const char* vertexPath, const char* fragmentPath);//read and build shader
	void use();
	void setBool(const std::string name, bool value) const;
	void setInt(const std::string name, int value) const;
	void setFloat(const std::string name, float value) const;
	void setMat4(const std::string name, glm::mat4 value) const;	
	void setVec3(const std::string name, glm::vec3 value) const;
};

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
	std::string vShaderCode;
	std::string fShaderCode;
	std::ifstream vertexShaderFile;
	std::ifstream fragmentShaderFile;
	vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		vertexShaderFile.open(vertexPath);
		fragmentShaderFile.open(fragmentPath);
		std::stringstream vertexShaderStream, fragmentShaderStream;
		vertexShaderStream << vertexShaderFile.rdbuf();
		fragmentShaderStream << fragmentShaderFile.rdbuf();
		vertexShaderFile.close();
		fragmentShaderFile.close();

		vShaderCode = vertexShaderStream.str();
		fShaderCode = fragmentShaderStream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FAIL_TO_READ_FILE\n";
	}
	//the source code of vertex and fragment shaders
	const char* vertexShaderCode = vShaderCode.c_str();
	const char* fragmentShaderCode = fShaderCode.c_str();
	//generate and link the shaders
	unsigned int vertexShader, fragmentShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
	glCompileShader(fragmentShader);
	int success_vs, success_fs;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success_vs);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success_fs);
	if (!success_fs)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	if (!success_vs)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	int success_sp;
	glGetProgramiv(ID, GL_LINK_STATUS, &success_sp);
	if (!success_sp) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
}

void Shader::setBool(std::string name, bool value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(std::string name, int value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(std::string name, float value)const {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setMat4(std::string name, glm::mat4 value)const {
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::setVec3(std::string name, glm::vec3 value)const {
	glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
}

void Shader::use() {
	glUseProgram(ID);
}