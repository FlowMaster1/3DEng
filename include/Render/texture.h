#pragma once

#include <glad.h>
#include <stb_image.h>
#include <string>

class Texture{
private:
    unsigned int textureID;
    int iWidth, iHeight, iNumOfComponents;
    std::string type;

public:
    std::string path;
    Texture(const std::string &filepath,const std::string &type);
   // ~Texture();

    std::string GetType(){return type;}
    void Bind(unsigned int slot = 0);
    void Unbind();
};