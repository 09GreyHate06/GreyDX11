#pragma once 
#include "../Renderer/GDX11Context.h"
#include "Texture2D.h"

namespace GDX11::Utils
{
	class DepthStencil
	{
	public:
		virtual ~DepthStencil() = default;

		void Clear(uint32_t clearFlags, float depth, uint8_t stencil);

		ID3D11DepthStencilView* GetDSV() const { return m_dsv.Get(); }
		std::shared_ptr<Texture2D> GetTexture() const { return m_dsTexture; }

		static std::shared_ptr<DepthStencil> Create(GDX11Context* context, const D3D11_DEPTH_STENCIL_VIEW_DESC& dsvDesc, const std::shared_ptr<Texture2D>& tex);

	private:
		DepthStencil(GDX11Context* context, const D3D11_DEPTH_STENCIL_VIEW_DESC& dsvDesc, const std::shared_ptr<Texture2D>& tex);

		GDX11Context* m_context;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv;
		std::shared_ptr<Texture2D> m_dsTexture;
	};
}