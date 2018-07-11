//
// Created by jammer on 16/05/17.
//

#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <stb_image.h>

WTexture::WTexture(const std::string& filename)
{
  int n;
  auto data = stbi_load(filename.c_str(), &m_x, &m_y, &n, 0);
  if (data == nullptr) {
    std::cout << "could not open file: " << filename << std::endl;
    return;
  }

  glGenTextures(1, &m_tex);
  glBindTexture(GL_TEXTURE_2D, m_tex);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(
    GL_TEXTURE_2D, 0, GL_RGBA, m_x, m_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, 0);
  stbi_image_free(data);
}

WTexture::~WTexture()
{
  // glDeleteTextures(1, &m_tex);
}
