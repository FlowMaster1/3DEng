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
    Render();
    Render(unsigned int width,unsigned int height,std::string nameOfWindow);
    ~Render();  
    
    void RunPerFrame();
    void RunDevUI();
    void onEvent(SDL_Event *event);
    void loadScene(Scene &scene_m);
    void loadCamera(Camera &camera_m);

    
    unsigned int GetScreenWidth(){return screen_width;}
    unsigned int GetScreenHeight(){return screen_height;}
    SDL_Window *GetWindowID(){return window;}  
    bool isRuning() { return isRun; }

private:
    bool isRun;
    SDL_Window *window;
    SDL_GLContext glContext;
    ImGuiIO *io;
    Scene scene;
    Camera camera;
    SDL_bool mouseMode;

    unsigned int screen_width;
    unsigned int screen_height; 
    std::string name;
    
    void sdl_gl_init();
    void renderLightUI();

};