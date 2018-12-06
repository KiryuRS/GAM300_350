#include "stdafx.h"
#include "PPHDR.h"

#include <glew.h>

PPHDR::PPHDR(int width, int height)
{
  FramebufferSetting setting;
  setting.HasColorAttachment = true;
  setting.ColorXResolution = width;
  setting.ColorYResolution = height;
  setting.HasDepthAttachment = true;
  setting.DepthFormat = GL_DEPTH_COMPONENT32;
  setting.DepthXResolution = width;
  setting.DepthYResolution = height;
  mContent.Setup(setting);
}

void PPHDR::FillupContent(Framebuffer & framebuffer, unsigned int attachmentid, PostProcessingFlag flag, unsigned width, unsigned height)
{
  glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer.GetID());
  glReadBuffer(attachmentid);
  glBindTexture(GL_TEXTURE_2D, mContent.GetTexture());
  glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width, height, 0);
  glReadBuffer(GL_COLOR_ATTACHMENT0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint PPHDR::GetResult() const
{
  return mContent.GetTexture();
}
