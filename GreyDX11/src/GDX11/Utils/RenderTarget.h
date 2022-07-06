#pragma once
#include "../Renderer/GDX11Context.h"
#include "Texture2D.h"
#include "DepthStencil.h"

namespace GDX11::Utils
{
	class RenderTarget
	{
	public:
		virtual ~RenderTarget() = default;
		
		void Clear(float r, float g, float b, float a);
		void Bind(const DepthStencil* ds) const;


		ID3D11RenderTargetView* GetRTV() const { return m_rtv.Get(); }
		std::shared_ptr<Texture2D> GetTexture() { return m_texture; }
		const std::shared_ptr<Texture2D>& GetTexture() const { return m_texture; }

		static std::shared_ptr<RenderTarget> Create(GDX11Context* context, const D3D11_RENDER_TARGET_VIEW_DESC& rtvDesc, const std::shared_ptr<Texture2D>& tex);

	private:
		RenderTarget(GDX11Context* context, const D3D11_RENDER_TARGET_VIEW_DESC& rtvDesc, const std::shared_ptr<Texture2D>& tex);
		
		GDX11Context* m_context;
		std::shared_ptr<Texture2D> m_texture = nullptr;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv = nullptr;
	};
}