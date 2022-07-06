#pragma once
#include "../Renderer/GDX11Context.h"
#include <d3d11.h>
#include <wrl.h>
#include <filesystem>

namespace GDX11::Utils
{
	class Texture2D
	{
	public:
		virtual ~Texture2D() = default;

		void VSBind(uint32_t slot = 0) const;
		void PSBind(uint32_t slot = 0) const;

		ID3D11ShaderResourceView* GetView() const { return m_textureView.Get(); }
		ID3D11Texture2D* GetTexture2D() const { return m_texture.Get(); }

		static std::shared_ptr<Texture2D> Create(GDX11Context* context, const void* data, const D3D11_TEXTURE2D_DESC& texDesc, const D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc);
		static std::shared_ptr<Texture2D> Create(GDX11Context* context, const void* data, const D3D11_TEXTURE2D_DESC& texDesc);
		static std::shared_ptr<Texture2D> Create(GDX11Context* context, ID3D11Texture2D* tex, const D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc);
		static std::shared_ptr<Texture2D> Create(GDX11Context* context, ID3D11Texture2D* tex, ID3D11ShaderResourceView* srv);

	private:
		Texture2D(GDX11Context* context, const void* data, const D3D11_TEXTURE2D_DESC& texDesc, const D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc);
		Texture2D(GDX11Context* context, const void* data, const D3D11_TEXTURE2D_DESC& texDesc);
		Texture2D(GDX11Context* context, ID3D11Texture2D* tex, const D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc);
		Texture2D(GDX11Context* context, ID3D11Texture2D* tex, ID3D11ShaderResourceView* srv);

		GDX11::GDX11Context* m_context;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureView;
	};
}