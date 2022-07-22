#include "Texture2D.h"
#include "../Core/GDX11Assert.h"

namespace GDX11
{
	static constexpr uint32_t s_reqComponents = 4;

	Texture2D::Texture2D(GDX11Context* context, const D3D11_TEXTURE2D_DESC& texDesc, const void* data)
		: RenderingResource(context)
	{
		HRESULT hr;
		if (data)
		{
			if (texDesc.MiscFlags & D3D11_RESOURCE_MISC_GENERATE_MIPS)
			{
				GDX11_CONTEXT_THROW_INFO(m_context->GetDevice()->CreateTexture2D(&texDesc, nullptr, &m_texture));
				m_context->GetDeviceContext()->UpdateSubresource(m_texture.Get(), 0, nullptr, data, s_reqComponents * texDesc.Width, 0);
			}
			else
			{
				D3D11_SUBRESOURCE_DATA srd = {};
				srd.pSysMem = data;
				srd.SysMemPitch = s_reqComponents * texDesc.Width;
				srd.SysMemSlicePitch = 0;
				GDX11_CONTEXT_THROW_INFO(m_context->GetDevice()->CreateTexture2D(&texDesc, &srd, &m_texture));
			}
		}

		GDX11_CONTEXT_THROW_INFO(m_context->GetDevice()->CreateTexture2D(&texDesc, nullptr, &m_texture));
	}

	Texture2D::Texture2D(GDX11Context* context, ID3D11Texture2D* tex)
		: RenderingResource(context), m_texture(tex)
	{
		GDX11_CORE_ASSERT(m_texture, "Texture2D is null");
	}

	std::shared_ptr<Texture2D> Texture2D::Create(GDX11Context* context, const D3D11_TEXTURE2D_DESC& texDesc, const void* data)
	{
		return std::shared_ptr<Texture2D>(new Texture2D(context, texDesc, data));
	}

	std::shared_ptr<Texture2D> Texture2D::Create(GDX11Context* context, ID3D11Texture2D* tex)
	{
		return std::shared_ptr<Texture2D>(new Texture2D(context, tex));
	}
}

