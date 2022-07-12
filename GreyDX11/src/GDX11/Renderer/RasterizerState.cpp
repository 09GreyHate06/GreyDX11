#include "RasterizerState.h"
#include "../Core/GDX11Assert.h"

namespace GDX11
{
	RasterizerState::RasterizerState(GDX11Context* context, const D3D11_RASTERIZER_DESC& desc)
		: RenderingResource(context)
	{
		GDX11_CORE_ASSERT(m_context, "Context cannot be null");

		HRESULT hr;
		GDX11_CONTEXT_THROW_INFO(m_context->GetDevice()->CreateRasterizerState(&desc, &m_rs));
	}

	void RasterizerState::Bind() const
	{
		m_context->GetDeviceContext()->RSSetState(m_rs.Get());
	}

	std::shared_ptr<RasterizerState> RasterizerState::Create(GDX11Context* context, const D3D11_RASTERIZER_DESC& desc)
	{
		return std::shared_ptr<RasterizerState>(new RasterizerState(context, desc));
	}
}