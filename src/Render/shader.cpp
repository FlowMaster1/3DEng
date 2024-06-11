#include "Render/shader.h"

std::string GetShaderFromFile(const std::string &path){
    std::ifstream shaderFile(path);
    std::stringstream result;
    if(shaderFile.is_open()){
        result << shaderFile.rdbuf();
        shaderFile.close();
        return result.str();
    }
    else{
        std::cerr << "Error wrong path to the shaders!!!" << std::endl;
        shaderFile.close();
        return "";
    }
}

Shader::Shader(const std::string &VertexPath, const std::string &FragmentPath){
    ID = glCreateProgram();
    unsigned int VertexSh = CompileShader(GL_VERTEX_SHADER, GetShaderFromFile(VertexPath));
    unsigned int FragmentSh = CompileShader(GL_FRAGMENT_SHADER, GetShaderFromFile(FragmentPath));

    glAttachShader(ID,VertexSh);
    glAttachShader(ID,FragmentSh);
    glLinkProgram(ID);

    int success;
    glGetShaderiv(ID, GL_LINK_STATUS, &success);

    if(!success)
    {
        int length;
        glGetShaderiv(ID, GL_INFO_LOG_LENGTH, &length);
        char infoLog[length];
        glGetShaderInfoLog(ID, length, NULL, infoLog);
        std::cerr << "ERROR::LINKING PROGRAM FAILED!\n" << infoLog << std::endl;
    }
    

    glDeleteShader(VertexSh);
    glDeleteShader(FragmentSh);
}

unsigned int Shader::CompileShader(const GLenum type, const std::string &source){
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src,NULL);
    glCompileShader(id);

    int  success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char infoLog[length];
        glGetShaderInfoLog(id, length, NULL, infoLog);
        std::cerr << "ERROR::SHADER::"<< type << "::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return id;
}

void Shader::setInt(const std::string name, int a){
    glUniform1i(glGetUniformLocation(ID,name.c_str()),a);
 }

void Shader::setFloat(const std::string name, float a){
    glUniform1f(glGetUniformLocation(ID,name.c_str()),a);
}

void Shader::setFloat3(const std::string name, float a,float b,float c){
    glUniform3f(glGetUniformLocation(ID,name.c_str()),a,b,c);
}

void Shader::setFloat3(const std::string name, float n[3]){
   glUniform3f(glGetUniformLocation(ID,name.c_str()),n[0],n[1],n[2]);
}

void Shader::setFloat4(const std::string name, float a,float b,float c, float d){
    glUniform4f(glGetUniformLocation(ID,name.c_str()),a,b,c,d);
}

void Shader::setVec3f(const std::string name, glm::vec3 vector){
    glUniform3fv(glGetUniformLocation(ID,name.c_str()),1, glm::value_ptr(vector));
}

void Shader::setMatrix4f(const std::string name, glm::mat4 mtrx){
    glUniformMatrix4fv(glGetUniformLocation(ID,name.c_str()),1,GL_FALSE,glm::value_ptr(mtrx));
}

void Shader::Activate(){
    glUseProgram(ID);
}

void Shader::Delete(){
    glDeleteProgram(ID);
}