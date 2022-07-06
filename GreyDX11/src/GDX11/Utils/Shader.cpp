#include "Shader.h"
#include "../Core/GDX11Assert.h"
#include <fstream>

using namespace Microsoft::WRL;

#define GDX11_SHADER_COMPILATION_EXCEPT(hr, msg) Shader::ShaderCompilationException(__LINE__, __FILE__, (hr), (msg))

namespace GDX11::Utils
{
	static std::string LoadTextFile(const std::string& filename)
	{
		std::string result;
		std::ifstream in(filename, std::ios::in | std::ios::binary);

		GDX11_CORE_ASSERT(in, "Failed to load text file {0}", filename);

		in.seekg(0, std::ios::end);
		result.resize(static_cast<uint32_t>(in.tellg()));
		in.seekg(0, std::ios::beg);
		in.read(result.data(), result.size());
		in.close();

		return result;
	}

	Shader::Shader(GDX11::GDX11Context* context, const std::string& vertexFilename, const std::string& pixelFilename)
		: m_context(context)
	{
		assert(m_context && "Context cannot be null");

		HRESULT hr;

		std::string vsSource = LoadTextFile(vertexFilename);
		std::string psSource = LoadTextFile(pixelFilename);

		ComPtr<ID3DBlob> errorBlob;
		if (FAILED(hr = D3DCompile(vsSource.data(), vsSource.size(), nullptr, nullptr, nullptr, "main", "vs_4_0", 0, 0, &m_vsByteCode, &errorBlob)))
			throw GDX11_SHADER_COMPILATION_EXCEPT(hr, static_cast<const char*>(errorBlob->GetBufferPointer()));
		if (FAILED(hr = D3DCompile(psSource.data(), psSource.size(), nullptr, nullptr, nullptr, "main", "ps_4_0", 0, 0, &m_psByteCode, &errorBlob)))
			throw GDX11_SHADER_COMPILATION_EXCEPT(hr, static_cast<const char*>(errorBlob->GetBufferPointer()));

		GDX11_CONTEXT_THROW_INFO(m_context->GetDevice()->CreateVertexShader(m_vsByteCode->GetBufferPointer(), m_vsByteCode->GetBufferSize(), nullptr, &m_vs));
		GDX11_CONTEXT_THROW_INFO(m_context->GetDevice()->CreatePixelShader(m_psByteCode->GetBufferPointer(), m_psByteCode->GetBufferSize(), nullptr, &m_ps));

		GDX11_CONTEXT_THROW_INFO(D3DReflect(m_vsByteCode->GetBufferPointer(), m_vsByteCode->GetBufferSize(), __uuidof(ID3D11ShaderReflection), &m_vsReflection));
		GDX11_CONTEXT_THROW_INFO(D3DReflect(m_psByteCode->GetBufferPointer(), m_psByteCode->GetBufferSize(), __uuidof(ID3D11ShaderReflection), &m_psReflection));

		CreateInputLayout();
	}

	Shader::Shader(GDX11::GDX11Context* context, const std::string& vertexFilename)
		: m_context(context)
	{
		assert(m_context && "Context cannot be null");

		HRESULT hr;

		std::string vsSource = LoadTextFile(vertexFilename);

		ComPtr<ID3DBlob> errorBlob;
		if (FAILED(hr = D3DCompile(vsSource.data(), vsSource.size(), nullptr, nullptr, nullptr, "main", "vs_4_0", 0, 0, &m_vsByteCode, &errorBlob)))
			throw GDX11_SHADER_COMPILATION_EXCEPT(hr, static_cast<const char*>(errorBlob->GetBufferPointer()));

		GDX11_CONTEXT_THROW_INFO(m_context->GetDevice()->CreateVertexShader(m_vsByteCode->GetBufferPointer(), m_vsByteCode->GetBufferSize(), nullptr, &m_vs));

		GDX11_CONTEXT_THROW_INFO(D3DReflect(m_vsByteCode->GetBufferPointer(), m_vsByteCode->GetBufferSize(), __uuidof(ID3D11ShaderReflection), &m_vsReflection));

		m_ps = nullptr;
		m_psByteCode = nullptr;
		m_psReflection = nullptr;

		CreateInputLayout();
	}

	std::shared_ptr<Shader> Shader::Create(GDX11::GDX11Context* context, const std::string& vertexFilename, const std::string& pixelFilename)
	{
		return std::shared_ptr<Shader>(new Shader(context, vertexFilename, pixelFilename));
	}

	std::shared_ptr<Shader> Shader::Create(GDX11::GDX11Context* context, const std::string& vertexFilename)
	{
		return std::shared_ptr<Shader>(new Shader(context, vertexFilename));
	}

	void Shader::Bind()
	{
		m_context->GetDeviceContext()->IASetInputLayout(m_vsInputLayout.Get());
		m_context->GetDeviceContext()->VSSetShader(m_vs.Get(), nullptr, 0);
		m_context->GetDeviceContext()->PSSetShader(m_ps.Get(), nullptr, 0);
	}

	uint32_t Shader::GetVSResBinding(const std::string& name)
	{
		if (m_resVSBindingCache.find(name) != m_resVSBindingCache.end())
			return m_resVSBindingCache[name];

		HRESULT hr;
		D3D11_SHADER_INPUT_BIND_DESC desc = {};
		GDX11_CONTEXT_THROW_INFO(m_vsReflection->GetResourceBindingDescByName(name.c_str(), &desc));
		uint32_t slot = desc.BindPoint;
		m_resVSBindingCache[name] = slot;
		return slot;
	}

	uint32_t Shader::GetPSResBinding(const std::string& name)
	{
		if (m_resPSBindingCache.find(name) != m_resPSBindingCache.end())
			return m_resPSBindingCache[name];

		HRESULT hr;
		D3D11_SHADER_INPUT_BIND_DESC desc = {};
		GDX11_CONTEXT_THROW_INFO(m_psReflection->GetResourceBindingDescByName(name.c_str(), &desc));
		uint32_t slot = desc.BindPoint;
		m_resPSBindingCache[name] = slot;
		return slot;
	}

	void Shader::CreateInputLayout()
	{
		HRESULT hr;

		D3D11_SHADER_DESC shaderDesc = {};
		GDX11_CONTEXT_THROW_INFO(m_vsReflection->GetDesc(&shaderDesc));

		// Read input layout description from shader info
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
		for (int i = 0; i < shaderDesc.InputParameters; i++)
		{
			D3D11_SIGNATURE_PARAMETER_DESC paramDesc = {};
			GDX11_CONTEXT_THROW_INFO(m_vsReflection->GetInputParameterDesc(i, &paramDesc));

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
			m_vsByteCode->GetBufferPointer(), m_vsByteCode->GetBufferSize(), &m_vsInputLayout));
	}




	Shader::ShaderCompilationException::ShaderCompilationException(int line, const std::string& file, HRESULT hr, const std::string& info)
		: Exception(line, file), m_hr(hr), m_info(info)
	{
	}

	const char* Shader::ShaderCompilationException::what() const
	{
		std::ostringstream oss;
		oss << GetType() << '\n'
			<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
			<< std::dec << " (" << static_cast<uint32_t>(GetErrorCode()) << ") " << '\n'
			<< "[Error Info]: " << GetErrorInfo() << '\n';
		oss << GetOriginString();

		m_whatBuffer = oss.str();
		return m_whatBuffer.c_str();
	}

}
