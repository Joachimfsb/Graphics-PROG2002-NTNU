// This is the TextureManager.h file
#ifndef TEXTUREMANAGER_H_
#define TEXTUREMANAGER_H_
#pragma once

// External libraries
#include <glad/glad.h>
#include <stb_image.h>

// STD includes
#include <string>
#include <vector>

namespace Framework {

  class TextureManager
  {
  public:

    enum TextureType {Texture2D, Texture3D, CubeMap, SkyBox};

    struct Texture
    {
      bool mipMap;
      int width, height, bpp;
      std::string name;
      std::string filePath;
      GLuint unit;
      TextureManager::TextureType type;
    };

  public:
    static TextureManager* GetInstance()
    {return TextureManager::Instance != nullptr?TextureManager::Instance: TextureManager::Instance = new TextureManager(); }

  public:
    bool LoadTexture2DRGBA(const std::string& name, const std::string& filepath, GLuint unit, bool mipMap=true);
    bool LoadCubeMapRGBA(const std::string& name, const std::string& filePath, GLuint unit, bool mipMap=true);
    GLuint GetUnitByName(const std::string& name) const;

  private:
    unsigned char* LoadTextureImage(const std::string& filepath, int& width, int& height, int& bpp, int format)const;
    void FreeTextureImage(unsigned char* data) const;

  private:
    TextureManager(){};
    ~TextureManager();
    TextureManager(const TextureManager&) = delete;
    void operator=(const TextureManager&) = delete;

  private:
    inline static TextureManager* Instance = nullptr;

  private:
    std::vector<TextureManager::Texture> Textures;
  };
};

#endif // TEXTUREMANAGER_H_
