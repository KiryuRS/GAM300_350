#pragma once
#include "PostProcessing.h"
#include "Framebuffer.h"

class PPHDR : public PostProcessing
{
public:
  PPHDR(int width, int height);
  void FillupContent(Framebuffer& framebuffer, unsigned int attachmentid, PostProcessingFlag flag, unsigned width, unsigned height) override;
  GLuint GetResult() const override;
private:
  Framebuffer mContent;
};