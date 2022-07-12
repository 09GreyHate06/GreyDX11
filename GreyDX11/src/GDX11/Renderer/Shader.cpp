#include "Shader.h"
#include "../Core/GDX11Assert.h"
#include <fstream>

using namespace Microsoft::WRL;

#define GDX11_SHADER_COMPILATION_EXCEPT(hr, msg) Shader::ShaderCompilationException(__LINE__, __FILE__, (hr), (msg))

namespace GDX11
{
	VertexShader::VertexShader(GDX11Context* context, const std::string& src)
		: Shader(context)
	{
		HRESULT hr;
		ComPtr<ID3DBlob> errorBlob;
		if (FAILED(hr = D3DCompile(src.data(), src.size(), nullptr, nullptr, nullptr, "main", "vs_4_0", 0, 0, &m_byteCode, &errorBlob)))
			throw GDX11_SHADER_COMPILATION_EXCEPT(hr, static_cast<const char*>(errorBlob->GetBufferPointer()));

		GDX11_CONTEXT_THROW_INFO(m_context->GetDevice()->CreateVertexShader(m_byteCode->GetBufferPointer(), m_byteCode->GetBufferSize(), nullptr, &m_vs));

		GDX11_CONTEXT_THROW_INFO(D3DReflect(m_byteCode->GetBufferPointer(), m_byteCode->GetBufferSize(), __uuidof(ID3D11ShaderReflection), &m_reflection));

		CreateInputLayout();
	}

	void VertexShader::CreateInputLayout()
	{
		HRESULT hr;

		D3D11_SHADER_DESC shaderDesc = {};
		GDX11_CONTEXT_THROW_INFO(m_reflection->GetDesc(&shaderDesc));

		// Read input layout description from shader info
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
		for (int i = 0; i < shaderDesc.InputParameters; i++)
		{
			D3D11_SIGNATURE_PARAMETER_DESC paramDesc = {};
			GDX11_CONTEXT_THROW_INFO(m_reflection->GetInputParameterDesc(i, &paramDesc));

			// fill out input element desc
			D3D11_INPUT_ELEMENT_DESC elementDesc = {};
			elementDesc.SemanticName = paramDesc.SemanticName;
			elementDesc.SemanticIndex = paramDesc.SemanticIndex;
			elementDesc.InputSlot = 0;
			elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			elementDesc.InstanceDataStepRate = 0;

			// determine DXGI format
			if (paramDesc.Mask == 1)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
			}
			else if (paramDesc.Mask <= 3)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else if (paramDesc.Mask <= 7)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (paramDesc.Mask <= 15)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}

			inputLayoutDesc.push_back(elementDesc);
		}

		GDX11_CONTEXT_THROW_INFO(m_context->GetDevice()->CreateInputLayout(inputLayoutDesc.data(), static_cast<uint32_t>(inputLayoutDesc.size()),
			m_byteCode->GetBufferPointer(), m_byteCode->GetBufferSize(), &m_inputLayout));
	}

	void VertexShader::Bind() const
	{
		m_context->GetDeviceContext()->IASetInputLayout(m_inputLayout.Get());
		m_context->GetDeviceContext()->VSSetShader(m_vs.Get(), nullptr, 0);
	}

	uint32_t VertexShader::GetResBinding(const std::string& name)
	{
		if (m_resBindingCache.find(name) != m_resBindingCache.end())
			return m_resBindingCache[name];

		HRESULT hr;
		D3D11_SHADER_INPUT_BIND_DESC desc = {};
		GDX11_CONTEXT_THROW_INFO(m_reflection->GetResourceBindingDescByName(name.c_str(), &desc));
		uint32_t slot = desc.BindPoint;
		m_resBindingCache[name] = slot;
		return slot;
	}

	std::shared_ptr<VertexShader> VertexShader::Create(GDX11Context* context, const std::string& src)
	{
		return std::shared_ptr<VertexShader>(new VertexShader(context, src));
	}


	PixelShader::PixelShader(GDX11Context* context, const std::string& src)
		: Shader(context)
	{
		HRESULT hr;
		ComPtr<ID3DBlob> errorBlob;
		if (FAILED(hr = D3DCompile(src.data(), src.size(), nullptr, nullptr, nullptr, "main", "ps_4_0", 0, 0, &m_byteCode, &errorBlob)))
			throw GDX11_SHADER_COMPILATION_EXCEPT(hr, static_cast<const char*>(errorBlob->GetBufferPointer()));

		GDX11_CONTEXT_THROW_INFO(m_context->GetDevice()->CreatePixelShader(m_byteCode->GetBufferPointer(), m_byteCode->GetBufferSize(), nullptr, &m_ps));
		GDX11_CONTEXT_THROW_INFO(D3DReflect(m_byteCode->GetBufferPointer(), m_byteCode->GetBufferSize(), __uuidof(ID3D11ShaderReflection), &m_reflection));
	}	
	
	PixelShader::PixelShader(GDX11Context* context)
		: Shader(context), m_ps(nullptr), m_byteCode(nullptr), m_reflection(nullptr)
	{
	}

	void PixelShader::Bind() const
	{
		m_context->GetDeviceContext()->PSSetShader(m_ps.Get(), nullptr, 0);
	}

	uint32_t PixelShader::GetResBinding(const std::string& name)
	{
		if (m_resBindingCache.find(name) != m_resBindingCache.end())
			return m_resBindingCache[name];

		HRESULT hr;
		D3D11_SHADER_INPUT_BIND_DESC desc = {};
		GDX11_CONTEXT_THROW_INFO(m_reflection->GetResourceBindingDescByName(name.c_str(), &desc));
		uint32_t slot = desc.BindPoint;
		m_resBindingCache[name] = slot;
		return slot;
	}

	std::shared_ptr<PixelShader> PixelShader::Create(GDX11Context* context, const std::string& src)
	{
		return std::shared_ptr<PixelShader>(new PixelShader(context, src));
	}

	std::shared_ptr<PixelShader> PixelShader::Create(GDX11Context* context)
	{
		return std::shared_ptr<PixelShader>(new PixelShader(context));
	}
}
