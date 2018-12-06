#pragma once

#include "Framebuffer.h"

typedef unsigned int GLuint;

enum class PostProcessingFlag
{
  ColorAttachment = 0,
  DepthAttachment
};

class PostProcessing
{
public:
  PostProcessing();

  virtual void FillupContent(Framebuffer& framebuffer, unsigned int attachmentid, PostProcessingFlag flag, unsigned width, unsigned height) = 0; 
  virtual GLuint GetResult() const = 0;

private:
};