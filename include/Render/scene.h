#pragma once

#include <map>
#include <vector>
#include <iostream>
#include <memory>

#include "shader.h"
#include "model.h"

struct DirLight{
    bool enable = true;
    float direction[3] = {0.0,1.0,-1.0};
    float ambient[3] = {0.3,0.3,0.3};
    float diffuse[3] = {0.6,0.6,0.6};
    float specular[3] = {1.0,1.0,1.0};
};

struct PointLight{
    bool enable = true;
    float position[3] = {0.0,0.0,0.0};
    float ambient[3] =  {0.3,0.3,0.3};
    float diffuse[3] =  {0.6,0.6,0.6};
    float specular[3] = {1.0,1.0,1.0};
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};

struct SpotLight{
    bool enable = true;
    float position[3] = {1.0,0.0,0.0};
    float direction[3] = {1.0,0.0,0.0};
    float ambient[3] = {0.3,0.3,0.3};
    float diffuse[3] = {0.6,0.6,0.6};
    float specular[3] = {1.0,1.0,1.0};
    float constant = 1.f;
    float linear = 0.09f;
    float quadratic = 0.032f;
    float cutOff = 12.5f;
    float outerCutOff = 17.5f;
};

class Scene{
public:
    std::map<std::string,std::shared_ptr<Shader>> shaders;
    std::map<std::string,std::shared_ptr<Model>> models;
    DirLight dir_light;
    std::vector<PointLight> point_lights;
    std::vector<SpotLight> spot_lights;
    float bgColor[3];

    Scene(){}

    void loadModel(const std::string name,const std::string path){
        models.insert(std::pair<std::string,std::shared_ptr<Model>>(name,new Model(path)));
    }

    void Draw(){

        DirLightToShader();
        PointLightToShader();
        SpotLightToShader();
        std::map<std::string,std::shared_ptr<Model>>::iterator it;
        for (it = models.begin(); it != models.end(); it++) {
            it->second->Draw(*shaders["default"]);
        }
    }

private:
    void DirLightToShader(){
        shaders["default"]->setInt("EnableDirL",dir_light.enable);
        if(!dir_light.enable){
        return;
        }
    
        shaders["default"]->setFloat3("directionL.direction",dir_light.direction);
        shaders["default"]->setFloat3("directionL.Ambient",dir_light.ambient);
        shaders["default"]->setFloat3("directionL.Diffuse",dir_light.diffuse);
        shaders["default"]->setFloat3("directionL.Specular",dir_light.specular);
    }
    
    void PointLightToShader(){

        shaders["default"]->setInt("NumOfPoints",point_lights.size());
        
        if(point_lights.empty())
            return;
        
        int i = 0;

        for(auto light: point_lights){
            std::string pointL = "pointL[" + std::to_string(i) + ']';
            shaders["default"]->setInt(pointL + ".enable",light.enable);
            i++;
            
            if(!light.enable)
                continue;
            
            shaders["default"]->setFloat3(pointL + ".position",light.position);
            shaders["default"]->setFloat3(pointL + ".Ambient", light.ambient);
            shaders["default"]->setFloat3(pointL + ".Diffuse", light.diffuse);
            shaders["default"]->setFloat3(pointL + ".Specular",light.specular);
            shaders["default"]->setFloat(pointL + ".constant", light.constant);
            shaders["default"]->setFloat(pointL + ".linear", light.linear);
            shaders["default"]->setFloat(pointL + ".quadratic",light.quadratic);

        }
    }

    void SpotLightToShader(){
        shaders["default"]->setInt("NumOfSpot",spot_lights.size());
        
        if(spot_lights.empty())
            return;
        
        int i = 0;

        for(auto light: spot_lights){
            std::string spotL = "spotL[" + std::to_string(i) + ']';
            shaders["default"]->setInt(spotL + ".enable",light.enable);
            i++;
            
            if(!light.enable)
                continue;
            
            shaders["default"]->setFloat3(spotL + ".position", light.position);
            shaders["default"]->setFloat3(spotL + ".direction",light.direction);
            shaders["default"]->setFloat(spotL + ".cutoff", glm::cos(glm::radians(light.cutOff)));
            shaders["default"]->setFloat(spotL + ".outerCutoff", glm::cos(glm::radians(light.outerCutOff)));
            shaders["default"]->setFloat3(spotL + ".Ambient", light.ambient);
            shaders["default"]->setFloat3(spotL + ".Diffuse",light.diffuse);
            shaders["default"]->setFloat3(spotL + ".Specular",light.specular);
            shaders["default"]->setFloat(spotL + ".constant",  light.constant);
            shaders["default"]->setFloat(spotL + ".linear",    light.linear);
            shaders["default"]->setFloat(spotL + ".quadratic", light.quadratic);
        }
    }

};


