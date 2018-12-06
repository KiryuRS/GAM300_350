#include "stdafx.h"
#include "Framebuffer.h"


Framebuffer::Framebuffer()
  :mID{ static_cast<unsigned>(-1) }, mColorAttachment{}, mDepthAttachment{}
{
}

Framebuffer::~Framebuffer()
{
  glDeleteTextures(mColorAttachment.size(), &mColorAttachment[0]);
  glDeleteBuffers(mDepthAttachment.size(), &mDepthAttachment[0]);
}

GLuint Framebuffer::GetID() const
{
  return mID;
}

GLuint Framebuffer::GetTexture(int id) const
{
  return mColorAttachment[id];
}

GLuint Framebuffer::GetDepth(int id) const
{
  return mDepthAttachment[id];
}

void Framebuffer::Use() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, mID);
}

void Framebuffer::Unuse() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Setup(FramebufferSetting s)
{
  mSetting = s;
  glGenFramebuffers(1, &mID);
  glBindFramebuffer(GL_FRAMEBUFFER, mID);

  if (mSetting.IsMultisample)
  {
    if (mSetting.HasColorAttachment)
    {
      mColorAttachment.resize(mSetting.NumColorTexture);
      glGenTextures(mSetting.NumColorTexture, &mColorAttachment[0]);
      glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mColorAttachment[0]);
      glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,
        mSetting.MultiSampleAmount,
        mSetting.ColorFormat,
        mSetting.ColorXResolution,
        mSetting.ColorYResolution,
        GL_TRUE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

      if (mSetting.WhiteBorder)
      {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
      }

      glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, mColorAttachment[0], 0);
    }

    if (mSetting.HasDepthAttachment)
    {
      mDepthAttachment.resize(mSetting.NumDepthTexture);
      glGenRenderbuffers(mSetting.NumDepthTexture, &mDepthAttachment[0]);

      glBindRenderbuffer(GL_RENDERBUFFER, mDepthAttachment[0]);
      glRenderbufferStorageMultisample(GL_RENDERBUFFER,
        mSetting.MultiSampleAmount,
        mSetting.DepthFormat,
        mSetting.DepthXResolution,
        mSetting.DepthYResolution);

      glBindRenderbuffer(GL_RENDERBUFFER, 0);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthAttachment[0]);

    }
  }
  else
  {
    if (mSetting.HasColorAttachment)
    {
      mColorAttachment.resize(mSetting.NumColorTexture);
      glGenTextures(mSetting.NumColorTexture, &mColorAttachment[0]);
      glBindTexture(GL_TEXTURE_2D, mColorAttachment[0]);
      glTexImage2D(GL_TEXTURE_2D, 0, 
        mSetting.ColorFormat,
        mSetting.ColorXResolution,
        mSetting.ColorYResolution,
        0, 
        mSetting.ColorFormat,
        mSetting.DataType, NULL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

      if (mSetting.WhiteBorder)
      {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
      }

      glBindTexture(GL_TEXTURE_2D, 0);

      glFramebufferTexture2D(GL_FRAMEBUFFER, mSetting.ColorTarget, GL_TEXTURE_2D, mColorAttachment[0], 0);
    }

    if (mSetting.HasDepthAttachment)
    {
      mDepthAttachment.resize(mSetting.NumDepthTexture);
      glGenRenderbuffers(mSetting.NumDepthTexture, &mDepthAttachment[0]);


      glBindRenderbuffer(GL_RENDERBUFFER, mDepthAttachment[0]);
      glRenderbufferStorage(GL_RENDERBUFFER,
        mSetting.DepthFormat,
        mSetting.DepthXResolution,
        mSetting.DepthYResolution);

      glBindRenderbuffer(GL_RENDERBUFFER, 0);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthAttachment[0]);

    }
  }

  if (mSetting.NoColorAttachment)
  {
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
  }

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    CONSOLE_LN("Framebuffer not complete!");
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::ChangeAttachmentResolution(int x, int y)
{
  glBindFramebuffer(GL_FRAMEBUFFER, mID);

  if (mSetting.IsMultisample)
  {
    if (!mColorAttachment.empty())
    {
      mSetting.ColorXResolution = x;
      mSetting.ColorYResolution = y;

      for (unsigned i = 0; i < mColorAttachment.size(); ++i)
      {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mColorAttachment[i]);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,
          mSetting.MultiSampleAmount,
          mSetting.ColorFormat,
          mSetting.ColorXResolution,
          mSetting.ColorYResolution,
          GL_TRUE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        if (mSetting.WhiteBorder)
        {
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

          float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
          glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
        }

        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, mColorAttachment[i], 0);
      }
    }

    if (!mDepthAttachment.empty())
    {
      mSetting.DepthXResolution = x;
      mSetting.DepthYResolution = y;

      for (unsigned i = 0; i < mDepthAttachment.size(); ++i)
      {
        glBindRenderbuffer(GL_RENDERBUFFER, mDepthAttachment[i]);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER,
          mSetting.MultiSampleAmount,
          mSetting.DepthFormat,
          mSetting.DepthXResolution,
          mSetting.DepthYResolution);

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthAttachment[i]);
      }

      

    }
  }
  else
  {
    if (!mColorAttachment.empty())
    {
      mSetting.ColorXResolution = x;
      mSetting.ColorYResolution = y;

      for (unsigned i = 0; i < mColorAttachment.size(); ++i)
      {
        glBindTexture(GL_TEXTURE_2D, mColorAttachment[i]);
        glTexImage2D(GL_TEXTURE_2D, 0,
          mSetting.ColorFormat,
          mSetting.ColorXResolution,
          mSetting.ColorYResolution,
          0,
          mSetting.ColorFormat,
          mSetting.DataType, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        if (mSetting.WhiteBorder)
        {
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

          float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
          glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
        }
        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, mColorAttachment[i], 0);
      }
    }

    if (!mDepthAttachment.empty())
    {
      mSetting.DepthXResolution = x;
      mSetting.DepthYResolution = y;

      for (unsigned i = 0; i < mDepthAttachment.size(); ++i)
      {
        glBindRenderbuffer(GL_RENDERBUFFER, mDepthAttachment[i]);
        glRenderbufferStorage(GL_RENDERBUFFER,
          mSetting.DepthFormat,
          mSetting.DepthXResolution,
          mSetting.DepthYResolution);

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthAttachment[i]);
      }
    }
  }

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    CONSOLE_LN("Change Framebuffer Resolution not complete!");
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::AddColorAttachment(int x, int y, int colorTarget)
{
  glBindFramebuffer(GL_FRAMEBUFFER, mID);

  mSetting.ColorXResolution = x;
  mSetting.ColorYResolution = y;
  mSetting.ColorTarget = colorTarget;

  GLuint newtexture;
  glGenTextures(1, &newtexture);
  mColorAttachment.push_back(newtexture);

  if (mSetting.IsMultisample)
  {
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mColorAttachment[mColorAttachment.size() - 1]);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,
      mSetting.MultiSampleAmount,
      mSetting.ColorFormat,
      mSetting.ColorXResolution,
      mSetting.ColorYResolution,
      GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP);

    if (mSetting.WhiteBorder)
    {
      glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
      glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

      float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
      glTexParameterfv(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_BORDER_COLOR, color);
    }

    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, mSetting.ColorTarget, GL_TEXTURE_2D_MULTISAMPLE, mColorAttachment[mColorAttachment.size() - 1], 0);
  }
  else
  {
    glBindTexture(GL_TEXTURE_2D, mColorAttachment[mColorAttachment.size() - 1]);
    glTexImage2D(GL_TEXTURE_2D, 0,
      mSetting.ColorFormat,
      mSetting.ColorXResolution,
      mSetting.ColorYResolution,
      0,
      mSetting.ColorFormat,
      mSetting.DataType, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    if (mSetting.WhiteBorder)
    {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

      float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
      glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, mSetting.ColorTarget, GL_TEXTURE_2D, mColorAttachment[mColorAttachment.size() - 1], 0);
  }

  

  

  glDrawBuffers(mColorAttachment.size(), attachments);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    CONSOLE_LN("Add Framebuffer not complete!");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

