#include "DepthStencil.h"
#include "../Core/GDX11Assert.h"

namespace GDX11::Utils
{
    void DepthStencil::Clear(uint32_t clearFlags, float depth, uint8_t stencil)
    {
        m_context->GetDeviceContext()->ClearDepthStencilView(m_dsv.Get(), clearFlags, depth, stencil);
    }

    std::shared_ptr<DepthStencil> GDX11::Utils::DepthStencil::Create(GDX11Context* context, const D3D11_DEPTH_STENCIL_VIEW_DESC& dsvDesc, const std::shared_ptr<Texture2D>& tex)
    {
        return std::shared_ptr<DepthStencil>(new DepthStencil(context, dsvDesc, tex));
    }

    DepthStencil::DepthStencil(GDX11Context* context, const D3D11_DEPTH_STENCIL_VIEW_DESC& dsvDesc, const std::shared_ptr<Texture2D>& tex)
        : m_context(context), m_dsTexture(tex)
    {
        GDX11_CORE_ASSERT(m_context, "Context cannot be null");
        GDX11_CORE_ASSERT(m_dsTexture, "Texture cannot be null");

        D3D11_TEXTURE2D_DESC texDesc = {};
        m_dsTexture->GetTexture2D()->GetDesc(&texDesc);
        GDX11_CORE_ASSERT(texDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL);

        HRESULT hr;
        GDX11_CONTEXT_THROW_INFO(m_context->GetDevice()->CreateDepthStencilView(m_dsTexture->GetTexture2D(), &dsvDesc, &m_dsv));
    }
}