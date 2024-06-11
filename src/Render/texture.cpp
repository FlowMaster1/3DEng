#include "Render/texture.h"
#include <iostream>

Texture::Texture(const std::string &filepath,const std::string &type){

    stbi_set_flip_vertically_on_load(true);
    this->type = type;
    this->path = filepath;
    
    unsigned char* img_data = stbi_load(filepath.c_str(), &iWidth, &iHeight, &iNumOfComponents,4);
    if(!img_data)
        std::cerr << "ERROR: Texture " << filepath << " don`t exist!" << std::endl;
    glGenTextures(1,&textureID);
    glBindTexture(GL_TEXTURE_2D,textureID);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,iWidth,iHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,img_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    
    glBindTexture(GL_TEXTURE_2D,0);

    if(img_data)
        stbi_image_free(img_data);
}

/*Texture::~Texture(){
    glDeleteTextures(1,&textureID); 
}*/

void Texture::Bind(unsigned int slot){
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D,textureID);
    glEnable(GL_TEXTURE_2D);
}

void Texture::Unbind(){
    glBindTexture(GL_TEXTURE_2D,0);
}