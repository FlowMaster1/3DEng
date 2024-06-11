#include "Render/render.h"

void Render::sdl_gl_init(){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr <<"Error initializing SDL:\n"<< SDL_GetError() << std::endl;
        isRun = false;
    }
    window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          screen_width, screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

        // Создание окна
    if (window == nullptr) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        isRun = false;
    }

        // Создание контекста OpenGL
    glContext = SDL_GL_CreateContext(window);
    if (glContext == nullptr) {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        isRun = false;
    }

        // Инициализация GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        isRun = false;
    }

    // Устанавливаем viewport OpenGL
    glViewport(0, 0, screen_width, screen_height);
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO(); (void)io;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init("#version 420");


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    
}

Render::Render():
screen_width{640},screen_height{480},name{"3DENG"},isRun{true}
{
    sdl_gl_init();
}

Render::Render(unsigned int width,unsigned int height,std::string nameOfWindow):
screen_width{width},screen_height{height},name{nameOfWindow},isRun{true}
{
    sdl_gl_init();
}

 Render::~Render(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
 }

 void Render::RunPerFrame(){
    SDL_Event e;
    while(isRuning()){
        while(SDL_PollEvent(&e) ) {
            onEvent(&e);
            camera.onEvent(&e,mouseMode);
        }
        glClearColor(scene.bgColor[0],scene.bgColor[1],scene.bgColor[2],1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //TODO: Check if it needs activate
        scene.shaders["default"]->Activate();
        scene.Draw();

        scene.shaders["default"]->setVec3f("viewPos",camera.GetPosition());
        camera.updateCam(scene.shaders["default"]->GetId());
        RunDevUI();
        SDL_GL_SwapWindow(window);
    }
 }

void Render::onEvent(SDL_Event *event){
    ImGui_ImplSDL2_ProcessEvent(event);
    switch (event->type) {
        case SDL_QUIT:
            isRun = false;
            break;
        case SDL_KEYDOWN:
            switch (event->key.keysym.sym)
        {
            case SDLK_i:
                if(mouseMode)
                    mouseMode = SDL_FALSE;                    
                else
                    mouseMode = SDL_TRUE;
                break;
        }
        break;        
        default:
        break;
    }
    SDL_SetRelativeMouseMode(mouseMode);
}

 void Render::RunDevUI(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    renderLightUI();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
 }

void Render::loadScene(Scene &scene_m){
    this->scene = scene_m;
}

void Render::loadCamera(Camera &camera_m){
    this->camera = camera_m;
}

void Render::renderLightUI(){
    
    ImGui::Begin("Lights");
    if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None)){
        if(ImGui::BeginTabItem("Direct light")){
            
            ImGui::Checkbox("Enable directional light",&scene.dir_light.enable);
            static bool autocorrect = true;
            ImGui::Checkbox("AutoCorect Ambient/Specular",&autocorrect);
            if(autocorrect){
                ImGui::ColorEdit3("Light Color", scene.dir_light.diffuse);
                for(int i = 0;i < 3;i++){
                    scene.dir_light.ambient[i] = scene.dir_light.diffuse[i] - 0.5;
                    scene.dir_light.specular[i] = scene.dir_light.diffuse[i] + 0.5;
                }
            }
            else{
                ImGui::ColorEdit3("Ambient color", scene.dir_light.ambient);
                ImGui::ColorEdit3("Diffuse color", scene.dir_light.diffuse);
                ImGui::ColorEdit3("Specular color", scene.dir_light.specular);
            }
            ImGui::ColorEdit3("BackGround color", scene.bgColor);

            ImGui::SliderFloat3("Direction:", scene.dir_light.direction, -1.0, 1.0);
            ImGui::EndTabItem();
        }
    
    if (ImGui::BeginTabItem("Point lights")){
        
        static int item_current_idx = 0;

        if(ImGui::Button("Add Point Light")){
            PointLight l;
            scene.point_lights.push_back(l);
        }

        
        if(!scene.point_lights.empty()){
          
            if(ImGui::Button("Remove Point Light")){
                scene.point_lights.erase(scene.point_lights.begin() + item_current_idx);
            }
                
            ImGui::Checkbox("Enable point light",&scene.point_lights[item_current_idx].enable);           

            static bool autocorrect = true;
            ImGui::Checkbox("AutoCorect Ambient/Specular",&autocorrect);
            if(autocorrect){
                ImGui::ColorEdit3("Light Color", scene.point_lights[item_current_idx].diffuse);
                for(int i = 0;i < 3;i++){
                    scene.point_lights[item_current_idx].ambient[i] = scene.point_lights[item_current_idx].diffuse[i] - 0.5;
                    scene.point_lights[item_current_idx].specular[i]= scene.point_lights[item_current_idx].diffuse[i] + 0.5;
                }
            }
            else{
                ImGui::ColorEdit3("Ambient color", scene.point_lights[item_current_idx].ambient);
                ImGui::ColorEdit3("Diffuse color", scene.point_lights[item_current_idx].diffuse);
                ImGui::ColorEdit3("Specular color",scene.point_lights[item_current_idx].specular);
            }

            if (ImGui::BeginTable("Position", 3)){
                ImGui::TableNextColumn(); ImGui::DragFloat("X", &scene.point_lights[item_current_idx].position[0],0.5f);
                ImGui::TableNextColumn(); ImGui::DragFloat("Y", &scene.point_lights[item_current_idx].position[1],0.5f);  
                ImGui::TableNextColumn(); ImGui::DragFloat("Z", &scene.point_lights[item_current_idx].position[2],0.5f);  
                ImGui::EndTable();
            }
            ImGui::InputFloat("Constant",&scene.point_lights[item_current_idx].constant);
            ImGui::InputFloat("Linear",&scene.point_lights[item_current_idx].linear);
            ImGui::InputFloat("Quadratic",&scene.point_lights[item_current_idx].quadratic);

            if (ImGui::BeginListBox("Point light"))
            {   
                for (int n = 0; n < scene.point_lights.size(); n++)
                {
                    const bool is_selected = (item_current_idx == n);
                    std::string name = "PointLigth# " + std::to_string(n);
                    const char *label = name.c_str();
                    if (ImGui::Selectable(label, is_selected))
                        item_current_idx = n;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndListBox();
            }
        }
       
        ImGui::EndTabItem();
    }
        
    
    if (ImGui::BeginTabItem("Spot lights")){
        
        static int item_current_idx = 0;

        if(ImGui::Button("Add Spot Light")){
            SpotLight l;
            scene.spot_lights.push_back(l);
        }

        
        if(!scene.spot_lights.empty()){
          
            if(ImGui::Button("Remove Spot Light")){
                scene.spot_lights.erase(scene.spot_lights.begin() + item_current_idx);
            }
                
            ImGui::Checkbox("Enable Spot light",&scene.spot_lights[item_current_idx].enable);
            static bool bindToCamera;
            ImGui::Checkbox("Bind Spot Light to camera",&bindToCamera);
            
            static bool autocorrect = true;
            ImGui::Checkbox("AutoCorect Ambient/Specular",&autocorrect);
            if(autocorrect){
                ImGui::ColorEdit3("Light Color", scene.spot_lights[item_current_idx].diffuse);
                for(int i = 0;i < 3;i++){
                    scene.spot_lights[item_current_idx].ambient[i] = scene.spot_lights[item_current_idx].diffuse[i] - 0.5;
                    scene.spot_lights[item_current_idx].specular[i]= scene.spot_lights[item_current_idx].diffuse[i] + 0.5;
                }
            }
            else{
                ImGui::ColorEdit3("Ambient color", scene.spot_lights[item_current_idx].ambient);
                ImGui::ColorEdit3("Diffuse color", scene.spot_lights[item_current_idx].diffuse);
                ImGui::ColorEdit3("Specular color",scene.spot_lights[item_current_idx].specular);
            }

            if(bindToCamera){
                float pos[3];
                pos[0] = camera.GetPosition().x;
                pos[1] = camera.GetPosition().y;
                pos[2] = camera.GetPosition().z;
                float dir[3];
                dir[0] = camera.GetDirection().x;
                dir[1] = camera.GetDirection().y;
                dir[2] = camera.GetDirection().z;
                for(int i = 0;i < 3;i++){
                    scene.spot_lights[item_current_idx].position[i] = pos[i];
                    scene.spot_lights[item_current_idx].direction[i] = dir[i];
                }
            }
            else{
                if (ImGui::BeginTable("Position", 3)){
                    ImGui::TableNextColumn(); ImGui::DragFloat("X", &scene.spot_lights[item_current_idx].position[0],0.5f);
                    ImGui::TableNextColumn(); ImGui::DragFloat("Y", &scene.spot_lights[item_current_idx].position[1],0.5f);  
                    ImGui::TableNextColumn(); ImGui::DragFloat("Z", &scene.spot_lights[item_current_idx].position[2],0.5f);  
                    ImGui::EndTable();
                }

                ImGui::SliderFloat3("Direction:", scene.spot_lights[item_current_idx].direction, -1.0, 1.0);
            }
            ImGui::InputFloat("Constant",&scene.spot_lights[item_current_idx].constant);
            ImGui::InputFloat("Linear",&scene.spot_lights[item_current_idx].linear);
            ImGui::InputFloat("Quadratic",&scene.spot_lights[item_current_idx].quadratic);
            ImGui::InputFloat("CutOff",&scene.spot_lights[item_current_idx].cutOff);
            ImGui::InputFloat("OuterCutOff",&scene.spot_lights[item_current_idx].outerCutOff);

            if (ImGui::BeginListBox("Spot lights"))
            {   
                for (int n = 0; n < scene.spot_lights.size(); n++)
                {
                    const bool is_selected = (item_current_idx == n);
                    std::string name = "SpotLight# " + std::to_string(n);
                    const char *label = name.c_str();
                    if (ImGui::Selectable(label, is_selected))
                        item_current_idx = n;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndListBox();
            }
        }
       
        ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
    }
 
    ImGui::Text("Application average FPS:%.1f",io->Framerate);

    ImGui::End();
}