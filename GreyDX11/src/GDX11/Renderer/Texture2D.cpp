#include "Texture2D.h"
#include "../Core/GDX11Assert.h"

namespace GDX11
{
	static constexpr uint32_t s_reqComponents = 4;

	Texture2D::Texture2D(GDX11::GDX11Context* context, const void* data, const D3D11_TEXTURE2D_DESC& texDesc, const D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc)
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

		GDX11_CONTEXT_THROW_INFO(m_context->GetDevice()->CreateShaderResourceView(m_texture.Get(), &srvDesc, &m_textureView));

		// generate the mip chain using the gpu rendering pipeline
		if (texDesc.MiscFlags & D3D11_RESOURCE_MISC_GENERATE_MIPS)
		{
			m_context->GetDeviceContext()->GenerateMips(m_textureView.Get());
		}
	}

	Texture2D::Texture2D(GDX11Context* context, const void* data, const D3D11_TEXTURE2D_DESC& texDesc)
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
		m_textureView = nullptr;
	}

	Texture2D::Texture2D(GDX11Context* context, ID3D11Texture2D* tex, const D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc)
		: RenderingResource(context), m_texture(tex)
	{
		GDX11_CORE_ASSERT(m_texture, "Texture is null");

		HRESULT hr;
		GDX11_CONTEXT_THROW_INFO(m_context->GetDevice()->CreateShaderResourceView(m_texture.Get(), &srvDesc, &m_textureView));

		D3D11_TEXTURE2D_DESC texDesc = {};

		m_texture->GetDesc(&texDesc);
		// generate the mip chain using the gpu rendering pipeline
		if (texDesc.MiscFlags & D3D11_RESOURCE_MISC_GENERATE_MIPS)
		{
			m_context->GetDeviceContext()->GenerateMips(m_textureView.Get());
		}
	}

	Texture2D::Texture2D(GDX11Context* context, ID3D11Texture2D* tex, ID3D11ShaderResourceView* srv)
		: RenderingResource(context), m_texture(tex), m_textureView(srv)
	{
		GDX11_CORE_ASSERT(m_texture, "Texture is null");
	}

	Texture2D::Texture2D(GDX11Context* context, ID3D11Texture2D* tex)
		: Texture2D(context, tex, nullptr)
	{
	}

	void Texture2D::VSBind(uint32_t slot) const
	{
		assert(m_textureView);
		m_context->GetDeviceContext()->VSSetShaderResources(slot, 1, m_textureView.GetAddressOf());
	}

	void Texture2D::PSBind(uint32_t slot) const
	{
		assert(m_textureView);
		m_context->GetDeviceContext()->PSSetShaderResources(slot, 1, m_textureView.GetAddressOf());
	}




	std::shared_ptr<Texture2D> Texture2D::Create(GDX11::GDX11Context* context, const void* data, const D3D11_TEXTURE2D_DESC& texDesc, const D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc)
	{
		return std::shared_ptr<Texture2D>(new Texture2D(context, data, texDesc, srvDesc));
	}

	std::shared_ptr<Texture2D> Texture2D::Create(GDX11Context* context, const void* data, const D3D11_TEXTURE2D_DESC& texDesc)
	{
		return std::shared_ptr<Texture2D>(new Texture2D(context, data, texDesc));
	}

	std::shared_ptr<Texture2D> Texture2D::Create(GDX11Context* context, ID3D11Texture2D* tex, const D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc)
	{
		return std::shared_ptr<Texture2D>(new Texture2D(context, tex, srvDesc));
	}

	std::shared_ptr<Texture2D> Texture2D::Create(GDX11Context* context, ID3D11Texture2D* tex, ID3D11ShaderResourceView* srv)
	{
		return std::shared_ptr<Texture2D>(new Texture2D(context, tex, srv));
	}
	std::shared_ptr<Texture2D> Texture2D::Create(GDX11Context* context, ID3D11Texture2D* tex)
	{
		return std::shared_ptr<Texture2D>(new Texture2D(context, tex));
	}
}

