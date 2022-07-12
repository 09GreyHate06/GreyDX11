#pragma once
#include "RenderingResource.h"
#include <wrl.h>
#include <d3dcompiler.h>

namespace GDX11
{
	template<class T>
	class Shader : public RenderingResource<T>
	{
	public:
		using RenderingResource<T>::RenderingResource;

		virtual ~Shader() = default;

		virtual void Bind() const = 0;

		virtual uint32_t GetResBinding(const std::string& name) = 0;
		virtual ID3DBlob* GetByteCode() const = 0;
		virtual ID3D11ShaderReflection* GetReflection() const = 0;

		// exception
	public:
		class Exception : public GDX11::GDX11Exception
		{
			using GDX11::GDX11Exception::GDX11Exception;
		};

		class ShaderCompilationException : public Exception
		{
		public:
			ShaderCompilationException(int line, const std::string& file, HRESULT hr, const std::string& info)
				: Exception(line, file), m_hr(hr), m_info(info)
			{
			}

			virtual const char* what() const override
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

			virtual const char* GetType() const override { return "Shader Compilation Exception"; }
			const std::string& GetErrorInfo() const { return m_info; }
			HRESULT GetErrorCode() const { return m_hr; }

		private:
			std::string m_info;
			HRESULT m_hr;
		};
	};

	class VertexShader : Shader<ID3D11VertexShader>
	{
	public:
		virtual ~VertexShader() = default;

		virtual void Bind() const;
		virtual uint32_t GetResBinding(const std::string& name);

		virtual ID3D11VertexShader* GetNative() const override { return m_vs.Get(); }
		virtual ID3DBlob* GetByteCode() const override { return m_byteCode.Get(); }
		virtual ID3D11InputLayout* GetInputLayout() const { return m_inputLayout.Get(); }
		virtual ID3D11ShaderReflection* GetReflection() const override { return m_reflection.Get(); }

		static std::shared_ptr<VertexShader> Create(GDX11Context* context, const std::string& src);

	private:
		VertexShader(GDX11Context* context, const std::string& src);

		void CreateInputLayout();

		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vs;
		Microsoft::WRL::ComPtr<ID3DBlob> m_byteCode;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11ShaderReflection> m_reflection;

		std::unordered_map<std::string, uint32_t> m_resBindingCache;
	};

	class PixelShader : public Shader<ID3D11PixelShader>
	{
	public:
		virtual ~PixelShader() = default;

		virtual void Bind() const;
		virtual uint32_t GetResBinding(const std::string& name);

		virtual ID3D11PixelShader* GetNative() const override { return m_ps.Get(); }
		virtual ID3DBlob* GetByteCode() const override { return m_byteCode.Get(); }
		virtual ID3D11ShaderReflection* GetReflection() const override { return m_reflection.Get(); }

		static std::shared_ptr<PixelShader> Create(GDX11Context* context, const std::string& src);
		static std::shared_ptr<PixelShader> Create(GDX11Context* context);

	private:
		PixelShader(GDX11Context* context, const std::string& src);
		PixelShader(GDX11Context* context);

		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps;
		Microsoft::WRL::ComPtr<ID3DBlob> m_byteCode;
		Microsoft::WRL::ComPtr<ID3D11ShaderReflection> m_reflection;

		std::unordered_map<std::string, uint32_t> m_resBindingCache;
	};

	//class Shader
	//{
	//public:
	//	virtual ~Shader() = default;

	//	void Bind();

	//	uint32_t GetVSResBinding(const std::string& name);
	//	uint32_t GetPSResBinding(const std::string& name);



	//	ID3D11VertexShader* GetVertexShader() const { return m_vs.Get(); }
	//	ID3D11PixelShader* GetPixelShader() const { return m_ps.Get(); }
	//	ID3DBlob* GetVertexShaderByteCode() const { return m_vsByteCode.Get(); }
	//	ID3DBlob* GetPixelShaderByteCode() const { return m_psByteCode.Get(); }
	//	ID3D11InputLayout* GetVertexShaderInputLayout() const { return m_vsInputLayout.Get(); }
	//	ID3D11ShaderReflection* GetVertexShaderReflection() const { return m_vsReflection.Get(); }
	//	ID3D11ShaderReflection* GetPixelShaderReflection() const { return m_psReflection.Get(); }



	//	static std::shared_ptr<Shader> Create(GDX11::GDX11Context* context, const std::string& vertexFilename, const std::string& pixelFilename);
	//	static std::shared_ptr<Shader> Create(GDX11::GDX11Context* context, const std::string& vertexFilename);

	//private:
	//	Shader(GDX11::GDX11Context* context, const std::string& vertexFilename, const std::string& pixelFilename);
	//	Shader(GDX11::GDX11Context* context, const std::string& vertexFilename);

	//	void CreateInputLayout();

	//	GDX11::GDX11Context* m_context;

	//	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vs = nullptr;
	//	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps = nullptr;
	//	Microsoft::WRL::ComPtr<ID3DBlob> m_vsByteCode = nullptr;
	//	Microsoft::WRL::ComPtr<ID3DBlob> m_psByteCode = nullptr;

	//	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_vsInputLayout = nullptr;

	//	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> m_vsReflection = nullptr;
	//	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> m_psReflection = nullptr;

	//	std::unordered_map<std::string, uint32_t> m_resVSBindingCache;
	//	std::unordered_map<std::string, uint32_t> m_resPSBindingCache;




	//};
}