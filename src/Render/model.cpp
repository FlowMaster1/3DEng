#include "Render/model.h"

Model::Model(std::string const &path)
{
    this->model_mat = glm::mat4(1.0f);
    std::clog << "Loading model:" << path << "\n";
    UseMaterial = false;
    loadModel(path);
}


void Model::Draw(Shader &shader)
{
    
    shader.setInt("UseMaterial",UseMaterial);
    if(UseMaterial){
        if(material.Specular == glm::vec3(0.0,0.0,0.0)){
            material.Specular = material.Diffuse * glm::vec3(1.0f);
            material.Ambient = material.Diffuse * glm::vec3(0.5f);
            Shininess = 12000;
        }
        shader.setVec3f("material.Ambient",material.Ambient);
        shader.setVec3f("material.Diffuse",material.Diffuse);
        shader.setVec3f("material.Specular",material.Specular);
        shader.setFloat("Shininess",Shininess);
    }

    shader.setMatrix4f("u_MM",model_mat);
    for(unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
    model_mat = glm::mat4(1.0);
}  

void Model::Rotate(float angle, glm::vec3 coord){
    model_mat = glm::rotate(model_mat,glm::radians(angle),coord);
}

void Model::Scale(glm::vec3 size){
    model_mat = glm::scale(model_mat,size);
}

void Model::Move(glm::vec3 coord){
    model_mat = glm::translate(model_mat,coord);
}

void Model::loadModel(std::string const &path)
{
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_PreTransformVertices );	
	
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        std::cerr << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}  

void Model::processNode(aiNode *node, const aiScene *scene)
{
    // process all the node's meshes (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        Mesh m = processMesh(mesh, scene);
        meshes.push_back(m);			
    }
    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}  


Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // process vertex positions, normals and texture coordinates
        glm::vec3 vector; 
        vector.x = (float)mesh->mVertices[i].x;
        vector.y = -(float)mesh->mVertices[i].y;
        vector.z = (float)mesh->mVertices[i].z; 
        vertex.position = vector;  
        if (mesh->HasNormals()){   
            vector.x = (float)mesh->mNormals[i].x;
            vector.y = -(float)mesh->mNormals[i].y;
            vector.z = (float)mesh->mNormals[i].z;
            vertex.Normal = vector;    
        }
        if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x; 
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoord = vec;
        }
        else
            vertex.TexCoord = glm::vec2(0.0f, 0.0f);    
        
        vertices.push_back(vertex);
    }
    // process indices
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back((unsigned int)face.mIndices[j]);
    }  
    // process material
    if(mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, 
                                            aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = loadMaterialTextures(material, 
                                            aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());


    }  
    
    return Mesh(vertices, indices, textures);
}  


std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
{
    mat->Get(AI_MATKEY_SHININESS, Shininess);

    std::vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for(unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; 
                break;
            }
        }
        if(!skip)
        {   // if texture hasn't been loaded already, load it
            std::clog << "  Loading texture:" << directory + '/' + str.C_Str() + "\n";
            Texture texture(directory + '/' + str.C_Str(), typeName);
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture); // add to loaded textures
        }
    }
    if(textures_loaded.empty()){
       textures = lookForTexture(directory,mat);
    }

    return textures;
} 

std::vector<Texture> Model::lookForTexture(std::string path,aiMaterial *mat){
    std::clog << "  Can`t find textures:" << "\n";
    std::vector<Texture> tex;
    std::string dir;
    if(path.find("source") != std::string::npos){
        dir = path.substr(0, path.find_last_of('/'));
    }
    dir += "/textures";
    std::clog << "      Trying to find textures in:" << dir << "\n";
    if(std::filesystem::exists(dir)){
        for (const auto & entry : std::filesystem::directory_iterator(dir)){
            Texture t(entry.path().string(), "texture_diffuse");
            tex.push_back(t);
            std::cout << entry.path() << std::endl;
        }
    }
    else{
        std::clog << "      Can`t find textures in:" << dir << "\n          Looking for materials...\n";
        material = loadMaterial(mat);
        UseMaterial = true;
        std::clog << "      Done\n";
    }
    return tex;
}

Material Model::loadMaterial(aiMaterial* mat) {
    Material material;
    aiColor3D color(0.f, 0.f, 0.f);

    mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
    material.Diffuse = glm::vec3(color.r, color.b, color.g);

    mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
    material.Ambient = glm::vec3(color.r, color.b, color.g);

    mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
    material.Specular = glm::vec3(color.r, color.b, color.g);

    return material;
}