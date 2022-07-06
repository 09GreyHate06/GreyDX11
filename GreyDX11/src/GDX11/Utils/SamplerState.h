#pragma once
#include "../Renderer/GDX11Context.h"
#include <wrl.h>

namespace GDX11::Utils
{
	class SamplerState
	{
	public:
		virtual ~SamplerState() = default;

		void VSBind(uint32_t slot = 0) const;
		void PSBind(uint32_t slot = 0) const;

		ID3D11SamplerState* GetSamplerState() const { return m_samplerState.Get(); }

		static std::shared_ptr<SamplerState> Create(GDX11Context* context, const D3D11_SAMPLER_DESC& samplerDesc);

	private:
		SamplerState(GDX11Context* context, const D3D11_SAMPLER_DESC& samplerDesc);

		GDX11Context* m_context;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState = nullptr;
	};
}