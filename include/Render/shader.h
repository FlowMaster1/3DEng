#pragma once

#include <glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>


std::string GetShaderFromFile(const std::string &path);

class Shader{
private:
    unsigned int ID;
    
    unsigned int CompileShader(const GLenum type, const std::string &source);

public:
    Shader(const std::string &VertexPath, const std::string &FragmentPath);
    ~Shader(){glDeleteProgram(ID);}

     const unsigned int GetId(){return ID;} 

     void setInt(const std::string name, int a);
     void setFloat(const std::string name, float a);
     void setFloat3(const std::string name, float a,float b,float c);
     void setFloat3(const std::string name, float n[3]);
     void setFloat4(const std::string name, float a,float b,float c, float d);
     void setVec3f(const std::string name, glm::vec3 vector);
     void setMatrix4f(const std::string name, glm::mat4 mtrx);

    void Activate();
    void Delete();
};
