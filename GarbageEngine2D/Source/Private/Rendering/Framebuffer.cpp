#include "Rendering/Framebuffer.h"
#include "Core/Assert.h"
#include "OpenGL.h"

FORCEINLINE static uint32 GetTextureTarget(bool multisampled)
{
	return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
}

FORCEINLINE static void CreateTextures(uint32* outIds, uint32 count)
{
	GLCall(glGenTextures(count, outIds));
}

FORCEINLINE static void BindTexture(uint32 id, bool multisampled)
{
	glBindTexture(GetTextureTarget(multisampled), id);
}

static void AttachColorTexture(uint32 id, uint64 samples, uint32 internalFormat, uint32 format, uint32 wrapMode, uint32 filtering, uint16 width, uint16 height, uint8 index)
{
	glBindTexture(GL_TEXTURE_2D, id);

	bool multisampled = samples > 1;
	if (multisampled)
	{
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, (GLsizei)samples, internalFormat, width, height, GL_FALSE);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	}

	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GetTextureTarget(multisampled), id, 0));
}

static void AttachDepthTexture(uint32 id, uint64 samples, uint32 format, uint32 wrapMode, uint32 filtering, uint64 attachmentType, uint16 width, uint16 height)
{
	glBindTexture(GL_TEXTURE_2D, id);

	bool multisampled = samples > 1;
	if (multisampled)
	{
		GLCall(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, (GLsizei)samples, format, width, height, GL_FALSE));
	}
	else
	{
		//GLCall(glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height));
		glTexImage2D(GL_TEXTURE_2D, 1, format, width, height, 0, GL_RGB, GL_UNSIGNED_INT, nullptr);

		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrapMode));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode));
	}

	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, (GLenum)attachmentType, GetTextureTarget(multisampled), id, 0));
}

FORCEINLINE static bool IsDepthFormat(Texture::Format format)
{
	switch (format)
	{
		case Texture::Format::Depth16:
		case Texture::Format::Depth24Stencil8:
		case Texture::Format::Depth32: return true;
	}

	return false;
}

static uint32 GarbageEngineFramebufferTextureFormatToOpenGL(Texture::Format format)
{
	switch (format)
	{
		case Texture::Format::Red: return   GL_R8;
		case Texture::Format::RGB8: return  GL_RGB8;
		case Texture::Format::RGBA8: return GL_RGBA8;
		case Texture::Format::Depth16: return GL_R16;
		case Texture::Format::Depth24Stencil8: return GL_DEPTH24_STENCIL8;
		case Texture::Format::Depth32: return GL_R32F;
	}

	GARBAGE_DEBUGBREAK();

	return 0;
}

static uint32 GarbageEngineWrapModeToOpenGLWrapMode(Texture::WrapMode wrapMode)
{
	switch (wrapMode)
	{
		case Texture::WrapMode::Repeat: return         GL_REPEAT;
		case Texture::WrapMode::MirroredRepeat: return GL_MIRRORED_REPEAT;
		case Texture::WrapMode::ClampToEdge: return    GL_CLAMP_TO_EDGE;
		case Texture::WrapMode::ClampToBorder: return  GL_CLAMP_TO_BORDER;
		}

	GARBAGE_CORE_ASSERT(false, "Texture: Unknown wrap mode: {}", (uint32)wrapMode);

	return 0;
}

static uint32 GarbageEngineFilteringToOpenGLFiltering(Texture::Filtering filtering)
{
	switch (filtering)
	{
		case Texture::Filtering::Linear:  return GL_LINEAR;
		case Texture::Filtering::Nearest: return GL_NEAREST;
	}

	GARBAGE_CORE_ASSERT(false, "Texture: Unknown filtering type: {}", (uint32)filtering);

	return 0;
}

Framebuffer::Framebuffer(const Framebuffer::Specification& specification) : m_specification(specification)
{
	for (auto& attachmentSpecification : specification.Attachments)
	{
		if (!IsDepthFormat(attachmentSpecification.Format)) m_colorAttachmentSpecifications.emplace_back(attachmentSpecification);
		else m_depthAttachmentSpecification = attachmentSpecification;
	}

	Invalidate();
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &m_id);
	glDeleteTextures((GLsizei)m_colorAttachments.size(), m_colorAttachments.data());
	glDeleteTextures(1, &m_depthAttachment);
}

void Framebuffer::Invalidate()
{
	if (m_id > 0)
	{
		glDeleteFramebuffers(1, &m_id);
		glDeleteTextures((GLsizei)m_colorAttachments.size(), m_colorAttachments.data());
		glDeleteTextures(1, &m_depthAttachment);

		m_colorAttachments.clear();
		m_depthAttachment = 0;
	}

	glGenFramebuffers(1, &m_id);
	glBindFramebuffer(GL_FRAMEBUFFER, m_id);

	bool multisample = m_specification.Samples > 1;

	// Attachments
	if (m_colorAttachmentSpecifications.size())
	{
		m_colorAttachments.resize(m_colorAttachmentSpecifications.size());
		CreateTextures(m_colorAttachments.data(), (uint32)m_colorAttachments.size());

		for (size_t i = 0; i < m_colorAttachments.size(); i++)
		{
			BindTexture(multisample, m_colorAttachments[i]);

			switch (m_colorAttachmentSpecifications[i].Format)
			{
				case Texture::Format::RGB8:
					AttachColorTexture(m_colorAttachments[i], m_specification.Samples, GL_RGB8, GL_RGB,
						GarbageEngineWrapModeToOpenGLWrapMode(m_colorAttachmentSpecifications[i].WrapMode), GarbageEngineFilteringToOpenGLFiltering(m_colorAttachmentSpecifications[i].Filtering),
						m_specification.Width, m_specification.Height, (uint8)i);
					break;

				case Texture::Format::RGBA8:
					AttachColorTexture(m_colorAttachments[i], m_specification.Samples, GL_RGBA8, GL_RGBA,
						GarbageEngineWrapModeToOpenGLWrapMode(m_colorAttachmentSpecifications[i].WrapMode), GarbageEngineFilteringToOpenGLFiltering(m_colorAttachmentSpecifications[i].Filtering),
						m_specification.Width, m_specification.Height, (uint8)i);
					break;

				case Texture::Format::RedInteger:
					AttachColorTexture(m_colorAttachments[i], m_specification.Samples, GL_R32I, GL_RED_INTEGER,
						GarbageEngineWrapModeToOpenGLWrapMode(m_colorAttachmentSpecifications[i].WrapMode), GarbageEngineFilteringToOpenGLFiltering(m_colorAttachmentSpecifications[i].Filtering), 
						m_specification.Width, m_specification.Height, (uint8)i);
					break;
				}
		}
	}

	if (m_depthAttachmentSpecification.Format != Texture::Format::None)
	{
		CreateTextures(&m_depthAttachment, 1);
		BindTexture(multisample, m_depthAttachment);
		switch (m_depthAttachmentSpecification.Format)
		{
		case Texture::Format::Depth16:
			AttachDepthTexture(m_depthAttachment, m_specification.Samples, GL_DEPTH_COMPONENT16,
				GarbageEngineWrapModeToOpenGLWrapMode(m_depthAttachmentSpecification.WrapMode), GarbageEngineFilteringToOpenGLFiltering(m_depthAttachmentSpecification.Filtering),
				GL_DEPTH_ATTACHMENT, m_specification.Width, m_specification.Height);
			break;

			case Texture::Format::Depth24Stencil8:
				AttachDepthTexture(m_depthAttachment, m_specification.Samples, GL_DEPTH24_STENCIL8, 
					GarbageEngineWrapModeToOpenGLWrapMode(m_depthAttachmentSpecification.WrapMode), GarbageEngineFilteringToOpenGLFiltering(m_depthAttachmentSpecification.Filtering),
					GL_DEPTH_STENCIL_ATTACHMENT, m_specification.Width, m_specification.Height);
				break;

			case Texture::Format::Depth32:
				AttachDepthTexture(m_depthAttachment, m_specification.Samples, GL_DEPTH_COMPONENT32F,
					GarbageEngineWrapModeToOpenGLWrapMode(m_depthAttachmentSpecification.WrapMode), GarbageEngineFilteringToOpenGLFiltering(m_depthAttachmentSpecification.Filtering),
					GL_DEPTH_ATTACHMENT, m_specification.Width, m_specification.Height);
				break;
		}
	}

	if (m_colorAttachments.size() > 1)
	{
		GARBAGE_CORE_ASSERT(m_colorAttachments.size() <= 4);
		GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		GLCall(glDrawBuffers((GLsizei)m_colorAttachments.size(), buffers));
	}
	else if (m_colorAttachments.empty())
	{
		// Only depth-pass
		GLCall(glDrawBuffer(GL_NONE));
	}

	GARBAGE_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_id);
	glViewport(0, 0, m_specification.Width, m_specification.Height);
}

void Framebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Resize(uint16 width, uint16 height)
{
	if (width == 0 || height == 0 || width > MaxFramebufferSize || height > MaxFramebufferSize)
	{
		GARBAGE_CORE_WARN("Framebuffer: Attempted to rezize to {0}, {1}", width, height);
		return;
	}

	m_specification.Width = width;
	m_specification.Height = height;

	Invalidate();
}

int32 Framebuffer::ReadPixel(uint32 attachmentIndex, uint16 x, uint16 y)
{
	GARBAGE_CORE_ASSERT(attachmentIndex < m_colorAttachments.size());

	GLCall(glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex));
	int32 pixelData;
	GLCall(glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData));

	return pixelData;
}

void Framebuffer::BindColorTexture(uint32 attachmentIndex, uint8 slot) const
{
	GARBAGE_CORE_ASSERT(attachmentIndex < m_colorAttachments.size());
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_colorAttachments[attachmentIndex]);
}

void Framebuffer::BindDepthTexture(uint8 slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_depthAttachment);
}
