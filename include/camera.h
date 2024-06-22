#pragma once

#include <glad.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <iostream>

#define ORTHO 0
#define PERSPECTIVE 1

class Camera{
public:
    Camera(){}
    Camera(int width, int height,glm::vec3 position,glm::vec3 direction, bool projMode = PERSPECTIVE);

    void updateCam(int shaderID);

    void changeDrawDistant(float nearPlane, float farPlane);
    void changeFOV(float FOVdeg);
    void changeProgMode(bool mode);
    void changeSize(int width,int height);  

    glm::vec3 GetPosition(){return position;}
    glm::vec3 GetDirection(){return direction;}

    int onEvent(SDL_Event *event,SDL_bool mouseMode);

private:
    int width,height;

    bool mov_forward,mov_back,mov_left,mov_right,mov_up,mov_down;

    float yaw,pitch;

    float nearPlane,farPlane;
    float FOVdeg;
    bool proj_mode;


    glm::vec3 position,up,direction;
    glm::mat4 proj,view;
};
