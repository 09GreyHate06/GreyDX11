#include "RenderTarget.h"
#include "../Core/GDX11Assert.h"

namespace GDX11::Utils
{
	RenderTarget::RenderTarget(GDX11Context* context, const D3D11_RENDER_TARGET_VIEW_DESC& rtvDesc, const std::shared_ptr<Texture2D>& tex)
		: m_context(context), m_texture(tex)
	{
		GDX11_CORE_ASSERT(m_context, "Context cannot be null");
		GDX11_CORE_ASSERT(m_texture, "Texture cannot be null");

		D3D11_TEXTURE2D_DESC texDesc = {};
		m_texture->GetTexture2D()->GetDesc(&texDesc);
		assert(texDesc.BindFlags & D3D11_BIND_RENDER_TARGET);

		HRESULT hr;
		GDX11_CONTEXT_THROW_INFO(m_context->GetDevice()->CreateRenderTargetView(m_texture->GetTexture2D(), &rtvDesc, &m_rtv));
	}

	void RenderTarget::Clear(float r, float g, float b, float a)
	{
		float color[] = { r, g, b, a };
		m_context->GetDeviceContext()->ClearRenderTargetView(m_rtv.Get(), color);
	}

	void RenderTarget::Bind(const DepthStencil* ds) const
	{
		ID3D11DepthStencilView* dsv = ds ? ds->GetDSV() : nullptr;
		m_context->GetDeviceContext()->OMSetRenderTargets(1, m_rtv.GetAddressOf(), dsv);
	}

	std::shared_ptr<RenderTarget> RenderTarget::Create(GDX11Context* context, const D3D11_RENDER_TARGET_VIEW_DESC& rtvDesc, const std::shared_ptr<Texture2D>& tex)
	{
		return std::shared_ptr<RenderTarget>(new RenderTarget(context, rtvDesc, tex));
	}
}
