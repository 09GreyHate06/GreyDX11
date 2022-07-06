#include "SamplerState.h"
#include "../Core/GDX11Assert.h"

namespace GDX11::Utils
{
	SamplerState::SamplerState(GDX11Context* context, const D3D11_SAMPLER_DESC& samplerDesc)
		: m_context(context)
	{
		GDX11_CORE_ASSERT(m_context, "Context cannot be null");

		HRESULT hr;
		GDX11_CONTEXT_THROW_INFO(m_context->GetDevice()->CreateSamplerState(&samplerDesc, &m_samplerState));
	}

	void SamplerState::VSBind(uint32_t slot) const
	{
		m_context->GetDeviceContext()->VSSetSamplers(slot, 1, m_samplerState.GetAddressOf());
	}

	void SamplerState::PSBind(uint32_t slot) const
	{
		m_context->GetDeviceContext()->PSSetSamplers(slot, 1, m_samplerState.GetAddressOf());
	}

	std::shared_ptr<SamplerState> SamplerState::Create(GDX11Context* context, const D3D11_SAMPLER_DESC& samplerDesc)
	{
		return std::shared_ptr<SamplerState>(new SamplerState(context, samplerDesc));
	}
}

