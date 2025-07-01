#include "sepch.h"
#include "StarEngine/Renderer/Framebuffer.h"

#include "StarEngine/Core/Application.h"

#include "StarEngine/Renderer/Renderer.h"

namespace StarEngine
{
	Framebuffer::Framebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)

	{
		if (spec.Width == 0)
		{
			m_Width = Application::Get().GetWindow().GetWidth();
			m_Height = Application::Get().GetWindow().GetHeight();
		}
		else
		{
			m_Width = (uint32_t)(spec.Width * m_Specification.Scale);
			m_Height = (uint32_t)(spec.Height * m_Specification.Scale);
		}

		// create all iamge object now so we can start referencing them elsewhere
		uint32_t attachmentIndex = 0;
		if (!m_Specification.ExistingFramebuffer)
		{
			for (auto& attachmentSpec : m_Specification.Attachments.Attachments)
			{
				if (m_Specification.ExistingImage)
				{
					if (Utils::IsDepthFormat(attachmentSpec.Format))
						m_DepthAttachmentImage = m_Specification.ExistingImage;
					else
						m_AttachmentImages.emplace_back(m_Specification.ExistingImage);
				}
				else if (m_Specification.ExistingImage->find(attachmentIndex) != m_Specification.ExistingImage->end())
				{
					if (Utils::IsDepthFormat(attachmentSpec.Format))
						m_DepthAttachmentImage = m_Specification.ExistingImage->at(attachmentIndex);
					else
						m_AttachmentImages.emplace_back();
				}
				else if (Utils::IsDepthFormat(attachmentSpec.Format))
				{
					ImageSpecification spec;
					spec.Format = attachmentSpec.Format;
					spec.Usage = ImageUsage::Attachement;
					spec.Transfer = m_Specification.Transfer;
					spec.Width = (uint32_t)(m_Width * m_Specification.Scale);
					spec.Height = (uint32_t)(m_Height * m_Specification.Scale);
					spec.DebugName = std::format("{0}-DebugAttachment{1}", m_Specification.DebugName, attachmentIndex);
					m_DepthAttachmentImage = Image::Create(spec);
				}
				else
				{
					ImageSpecification spec;
					spec.Format = attachmentSpec.Format;
					spec.Usage = ImageUsage::Attachement;
					spec.Transfer = m_Specification.Transfer;
					spec.Width = (uint32_t)(m_Width * m_Specification.Scale);
					spec.Height = (uint32_t)(m_Height * m_Specification.Scale);
					spec.DebugName = std::format("{0}-DebugAttachment{1}", m_Specification.DebugName, attachmentIndex);
					m_AttachmentImages.emplace_back(Image::Create(spec));
				}
				attachmentIndex++;
			}
		}

		SE_CORE_ASSERT(m_Specification.Attachments.Attachments.size());
		Resize(m_Width, m_Height, true);
	}

	Framebuffer::~Framebuffer()
	{
		Release();
	}

	void Framebuffer::Release()
	{
		if (m_Handle)
		{

		}
	}

	void Framebuffer::Resize(uint32_t width, uint32_t height, bool forceRecreate)
	{
		if (!forceRecreate && (m_Width == width && m_Height == height))
		{
			return;
		}

		Ref<Framebuffer> instance = this;
		Renderer::Submit([instance, width, height]() mutable
			{
				instance->m_Width = (uint32_t)(width * instance->m_Specification.Scale);
				instance->m_Height = (uint32_t)(height * instance->m_Specification.Scale);
				if (!instance->m_Specification.SwapChainTarget)
				{
					instance->RT_Invalidate();
				}
				else
				{
					SE_CORE_VERIFY(false);
				}
			});

		for (auto& callback : m_ResizeCallbacks)
		{
			callback(this);
		}
	}
	void Framebuffer::AddResizeCallback(const std::function<void(Ref<Framebuffer>)>& func)
	{
		m_ResizeCallbacks.push_back(func);
	}

	void Framebuffer::Invalidate()
	{
		Ref<Framebuffer> instance = this;
		Renderer::Submit([instance]() mutable
			{
				instance->RT_Invalidate();
			});
	}

	

	void Framebuffer::RT_Invalidate()
	{
		Release();

		std::vector<nvrhi::FramebufferAttachment> attachmentDescriptions;

		m_ClearValues.resize(m_Specification.Attachments.Attachments.size());

		bool createImages = m_AttachmentImages.empty();

		if (m_Specification.ExistingFramebuffer)
			m_AttachmentImages.clear();

		// TODO: what about load store ops
		nvrhi::FramebufferDesc framebufferDesc;

		uint32_t attachmentIndex = 0;
		uint32_t nvrhiAttachmentIndex = 0;
		for (const auto& attachmentSpec : m_Specification.Attachments.Attachments)
		{

		}
	}

	

}
