#pragma once
#include "RenderingResource.h"

namespace GDX11
{
	class Texture2D : public RenderingResource<ID3D11Texture2D>
	{
	public:
		virtual ~Texture2D() = default;

		void VSBind(uint32_t slot = 0) const;
		void PSBind(uint32_t slot = 0) const;

		ID3D11ShaderResourceView* GetView() const { return m_textureView.Get(); }
		virtual ID3D11Texture2D* GetNative() const override { return m_texture.Get(); }

		static std::shared_ptr<Texture2D> Create(GDX11Context* context, const void* data, const D3D11_TEXTURE2D_DESC& texDesc, const D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc);
		static std::shared_ptr<Texture2D> Create(GDX11Context* context, const void* data, const D3D11_TEXTURE2D_DESC& texDesc);
		static std::shared_ptr<Texture2D> Create(GDX11Context* context, ID3D11Texture2D* tex, const D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc);
		static std::shared_ptr<Texture2D> Create(GDX11Context* context, ID3D11Texture2D* tex, ID3D11ShaderResourceView* srv);
		static std::shared_ptr<Texture2D> Create(GDX11Context* context, ID3D11Texture2D* tex);

	private:
		Texture2D(GDX11Context* context, const void* data, const D3D11_TEXTURE2D_DESC& texDesc, const D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc);
		Texture2D(GDX11Context* context, const void* data, const D3D11_TEXTURE2D_DESC& texDesc);
		Texture2D(GDX11Context* context, ID3D11Texture2D* tex, const D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc);
		Texture2D(GDX11Context* context, ID3D11Texture2D* tex, ID3D11ShaderResourceView* srv);
		Texture2D(GDX11Context* context, ID3D11Texture2D* tex);

		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureView;
	};
}