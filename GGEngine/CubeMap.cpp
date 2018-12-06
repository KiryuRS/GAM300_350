#include "stdafx.h"
#include "CubeMap.h"

CubeMap::CubeMap()
{
  // glGenFramebuffers(6, fbo);
  // glGenRenderbuffers(6, depthrenderbuffer);
  // glGenTextures(6, cm_render_textures);
  // 
  // for (unsigned i = 0; i < 6; ++i)
  // {
  //   glBindFramebuffer(GL_FRAMEBUFFER, fbo[i]);
  //   glBindTexture(GL_TEXTURE_2D, cm_render_textures[i]);
  //   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
  //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  // 
  //   glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, cm_render_textures[i], 0);
  //   glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer[i]);
  //   glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, 512, 512);
  //   glFramebufferRenderbuffer(GL_RENDERBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer[i]);
  // 
  //   glDrawBuffer(GL_COLOR_ATTACHMENT0);
  // 
  //   if (!(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE))
  //     throw "framebuffer incomplete";
  // 
  //   glBindRenderbuffer(GL_RENDERBUFFER, 0);
  //   glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // }
}

CubeMap::~CubeMap()
{
  // glDeleteFramebuffers(6, fbo);
  // glDeleteRenderbuffers(6, depthrenderbuffer);
  // glDeleteTextures(6, cm_render_textures);
}
