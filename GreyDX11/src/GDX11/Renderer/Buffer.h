#pragma once
#include "RenderingResource.h"

namespace GDX11
{
	class Buffer : public RenderingResource<ID3D11Buffer>
	{
	public:
		virtual ~Buffer() = default;

		void BindAsVB() const;
		void BindAsIB(DXGI_FORMAT format) const;
		void VSBindAsCBuf(uint32_t slot = 0) const;
		void PSBindAsCBuf(uint32_t slot = 0) const;

		void SetData(const void* data);
		D3D11_BUFFER_DESC GetDesc() const
		{
			D3D11_BUFFER_DESC desc = {};
			m_buffer->GetDesc(&desc);
			return desc;
		}

		virtual ID3D11Buffer* GetNative() const override { return m_buffer.Get(); }

		static std::shared_ptr<Buffer> Create(GDX11Context* context, const void* data, const D3D11_BUFFER_DESC& desc);

	private:
		Buffer(GDX11Context* context, const void* data, const D3D11_BUFFER_DESC& desc);
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
	};
}