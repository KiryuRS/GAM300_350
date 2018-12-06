#pragma once

enum class FBOType
{
  Standard = 0,
  Multisample,
};

struct FramebufferSetting
{
  bool      HasColorAttachment = { false };
  bool      HasDepthAttachment = { false };
  bool      IsMultisample = { false };
  bool      NoColorAttachment = { false };
  bool      WhiteBorder = { false };

  // color attachment
  int       NumColorTexture = { 1 };
  int       ColorFormat = GL_RGBA;
  int       DataType = GL_UNSIGNED_BYTE;
  int       ColorTarget = GL_COLOR_ATTACHMENT0;
  int       ColorXResolution = { 1024 };
  int       ColorYResolution = { 1024 };

  // depth attachment
  int       NumDepthTexture = { 1 };
  int       DepthFormat = GL_DEPTH_COMPONENT;
  int       DepthXResolution = { 1024 };
  int       DepthYResolution = { 1024 };

  // multisample setting
  int       MultiSampleAmount = { 2 };
};

class Framebuffer
{
public:
  Framebuffer();
  ~Framebuffer();
  void      Setup(FramebufferSetting setting);
  void      ChangeAttachmentResolution(int x, int y);

  void      AddColorAttachment(int x, int y, int colorTarget);

  GLuint    GetID() const;
  GLuint    GetTexture(int id = 0) const;
  GLuint    GetDepth(int id = 0) const;
  void      Use() const;
  void      Unuse() const;

private:
  GLuint    mID;
  std::vector<GLuint>    mColorAttachment;
  std::vector<GLuint>    mDepthAttachment;

  FramebufferSetting     mSetting;
};