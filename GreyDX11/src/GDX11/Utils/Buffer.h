#pragma once
#include "../Renderer/GDX11Context.h"
#include <wrl.h>

namespace GDX11::Utils
{
	class Buffer
	{
	public:
		virtual ~Buffer() = default;

		void BindAsVB() const;
		void BindAsIB(DXGI_FORMAT format) const;
		void VSBindAsCBuf(uint32_t slot = 0) const;
		void PSBindAsCBuf(uint32_t slot = 0) const;

		void SetData(const void* data);

		ID3D11Buffer* GetBuffer() const { return m_buffer.Get(); }
		D3D11_BUFFER_DESC GetDesc() const 
		{ 
			D3D11_BUFFER_DESC desc = {};
			m_buffer->GetDesc(&desc);
			return desc;
		}

		static std::shared_ptr<Buffer> Create(GDX11Context* context, const void* data, const D3D11_BUFFER_DESC& desc);

	private:
		Buffer(GDX11Context* context, const void* data, const D3D11_BUFFER_DESC& desc);

		GDX11Context* m_context;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
	};
}