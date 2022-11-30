#pragma once

#include "Core/Base.h"
#include "Core/Assert.h"
#include "Rendering/Texture.h"

constexpr uint32 MaxFramebufferSize = 8192;

class GARBAGE_API Framebuffer
{
public:

	struct TextureSpecification
	{
		TextureSpecification() = default;
		TextureSpecification(Texture::Format format, Texture::WrapMode wrapMode = Texture::WrapMode::ClampToEdge, Texture::Filtering filtering = Texture::Filtering::Nearest) 
			: Format(format), WrapMode(wrapMode), Filtering(filtering) {}

		Texture::Format Format = Texture::Format::Red;
		Texture::WrapMode WrapMode = Texture::WrapMode::ClampToEdge;
		Texture::Filtering Filtering = Texture::Filtering::Nearest;
	};

	struct Specification
	{
		Specification() = default;
		Specification(uint16 width, uint16 height, uint8 samples, const std::vector<TextureSpecification>& attachments)
			: Width(width), Height(height), Samples(samples), Attachments(attachments) {}

		uint16 Width = 0;
		uint16 Height = 0;
		uint64 Samples = 1;
		std::vector<TextureSpecification> Attachments;
	};
	
	Framebuffer(const Specification& specification);
	~Framebuffer();

	void Invalidate();

	void Bind();
	static void Unbind();

	void Resize(uint16 width, uint16 height);
	int32 ReadPixel(uint32 attachmentIndex, uint16 x, uint16 y);

	uint32 GetColorAttachmentInternalId(uint32 index) const { GARBAGE_CORE_ASSERT(index < m_colorAttachments.size()); return m_colorAttachments[index]; }
	void BindColorTexture(uint32 attachmentIndex, uint8 slot = 0) const;
	void BindDepthTexture(uint8 slot = 0) const;

	const Specification& GetSpecification() const { return m_specification; }

private:

	uint32 m_id{ 0 };
	Specification m_specification;

	std::vector<TextureSpecification> m_colorAttachmentSpecifications;
	TextureSpecification m_depthAttachmentSpecification = Texture::Format::None;

	std::vector<uint32> m_colorAttachments;
	uint32 m_depthAttachment{ 0 };

};