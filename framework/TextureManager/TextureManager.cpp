// This is the TextureManager.cpp
#include "TextureManager.h"

#include <iostream>

namespace Framework {
  bool TextureManager::LoadTexture2DRGBA(const std::string& name, const std::string& filePath, GLuint unit, bool mipMap)
  {
    int width, height, bpp;
    auto data = this->LoadTextureImage(filePath, width, height, bpp, STBI_rgb_alpha);

    if (!data)
      {
      return false;
      }

    GLuint tex;
    glGenTextures(1, &tex);
    glActiveTexture(GL_TEXTURE0 + unit); // Texture Unit
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    if (mipMap)
      {
      glGenerateMipmap(GL_TEXTURE_2D);
      }

    // Wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Texture texture;
    texture.mipMap = mipMap;
    texture.width = width;
    texture.height = height;
    texture.name = name;
    texture.filePath = filePath;
    texture.unit = unit;
    texture.type = Texture2D;

    this->Textures.push_back(texture);

    this->FreeTextureImage(data);

    return true;
  }

  bool TextureManager::LoadCubeMapRGBA(const std::string& name, const std::string& filePath, GLuint unit, bool mipMap)
  {
    int width, height, bpp;
    auto data = this->LoadTextureImage(filePath, width, height, bpp, STBI_rgb_alpha);

    if (!data)
      {
      return false;
      }

    /*Generate a texture object and upload the loaded image to it.*/
    GLuint tex;
    glGenTextures(1, &tex);
    glActiveTexture(GL_TEXTURE0 + unit); // Texture Unit
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

    for (unsigned int i = 0; i < 6; i++) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }

    if (mipMap)
      {
      glGenerateMipmap(GL_TEXTURE_2D);
      }

    // Wrapping
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
    // Filtering
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Texture texture;
    texture.mipMap = mipMap;
    texture.width = width;
    texture.height = height;
    texture.name = name;
    texture.filePath = filePath;
    texture.unit = unit;
    texture.type = CubeMap;

    this->Textures.push_back(texture);
    this->FreeTextureImage(data);

    return true;
  }


  GLuint TextureManager::GetUnitByName(const std::string& name) const
  {
  for(const auto& texture: this->Textures)
    {
    if (!texture.name.compare(name))
      {
      return texture.unit;
      }
    }
  return -1;
  }

  unsigned char* TextureManager::LoadTextureImage(const std::string& filepath, int& width, int& height, int& bpp, int format) const
  {
    stbi_set_flip_vertically_on_load(1); //flipper texture. fjern om den ødelegger noe. må brukes for orthographic camera
    return stbi_load(filepath.c_str(), &width, &height, &bpp, format);
  }


  void TextureManager::FreeTextureImage(unsigned char* data) const
  {
    if (data)
      {
      stbi_image_free(data);
      }
  }
};