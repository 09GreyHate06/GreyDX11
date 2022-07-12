#include "DepthStencilState.h"

namespace GDX11
{
	DepthStencilState::DepthStencilState(GDX11Context* context, const D3D11_DEPTH_STENCIL_DESC& desc)
		: RenderingResource(context)
	{
		HRESULT hr;
		GDX11_CONTEXT_THROW_INFO(m_context->GetDevice()->CreateDepthStencilState(&desc, &m_dss));
	}

	void DepthStencilState::Bind(uint32_t stencilRef)
	{
		m_context->GetDeviceContext()->OMSetDepthStencilState(m_dss.Get(), stencilRef);
	}

	std::shared_ptr<DepthStencilState> DepthStencilState::Create(GDX11Context* context, const D3D11_DEPTH_STENCIL_DESC& desc)
	{
		return std::shared_ptr<DepthStencilState>(new DepthStencilState(context,desc));
	}
}

