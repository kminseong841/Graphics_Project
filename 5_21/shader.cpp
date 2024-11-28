#include "shader.h"

Shader::Shader(const string& filepath) {
	ifstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	string line;
	stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != string::npos)
		{
			if (line.find("vertex") != string::npos) //vertex shader section
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != string::npos) //fragment shader section
			{
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << '\n'; //insert code to stringstream
		}
	}
	VertexSource = ss[0].str();
	FragSource = ss[1].str();
}

unsigned int Shader::CompileShader(unsigned int type, const string& source) {
	unsigned int id = glCreateShader(type); //create shader object
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr); // ���̴��� �ҽ� �ڵ� ���
	glCompileShader(id); // id�� �ش��ϴ� ���̴� ������

	// Error Handling(������ ���̴� ���α׷����Ҷ� ���Ӵ�...)
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result); //���̴� ���α׷����κ��� ������ ���(log)�� ����
	if (result == GL_FALSE) //�����Ͽ� ������ ���
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length); //log�� ���̸� ����
		char* message = (char*)alloca(length * sizeof(char)); //stack�� �����Ҵ�
		glGetShaderInfoLog(id, length, &length, message); //���̸�ŭ log�� ����
		cout << "���̴� ������ ����! " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << endl;
		cout << message << endl;
		glDeleteShader(id); //������ ������ ��� ���̴� ����
		return 0;
	}
	return id;
}

unsigned int Shader::CreateShader() {
	unsigned int program = glCreateProgram(); //���̴� ���α׷� ��ü ����(int�� ����Ǵ� ���� id)
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, VertexSource);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, FragSource);

	//�����ϵ� ���̴� �ڵ带 program�� �߰��ϰ� ��ũ
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	//���̴� ���α׷��� ���������Ƿ� vs, fs ���� ���α׷��� ���̻� �ʿ� ����
	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}