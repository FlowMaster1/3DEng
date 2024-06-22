# pragma once

#include <glad.h>
#include <SDL2/SDL.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_opengl3.h>
#include <iostream>

#include "scene.h"
#include "model.h"
#include "camera.h"


class Render{
public:
    SDL_bool mouseMode;

    Render();
    Render(int width,int height,std::string nameOfWindow);
    ~Render();  
    
    void RunPerFrame();
    void RunDevUI();
    void onEvent(SDL_Event *event);
    void loadScene(Scene *scene_m);
    void loadCamera(Camera *camera_m);

    
    int GetScreenWidth(){return screen_width;}
    int GetScreenHeight(){return screen_height;}
    SDL_Window *GetWindowID(){return window;}  
    bool isRuning() { return isRun; }

private:
    bool isRun;
    SDL_Window *window;
    SDL_GLContext glContext;
    ImGuiIO *io;
    Camera *camera;
    Scene *scene;

    int screen_width;
    int screen_height; 
    std::string name;
    
    void sdl_gl_init();
    void renderLightUI();

};
