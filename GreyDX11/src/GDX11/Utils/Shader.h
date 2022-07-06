#pragma once
#include "../Renderer/GDX11Context.h"
#include <wrl.h>
#include <d3dcompiler.h>

namespace GDX11::Utils
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		void Bind();

		uint32_t GetVSResBinding(const std::string& name);
		uint32_t GetPSResBinding(const std::string& name);



		ID3D11VertexShader* GetVertexShader() const { return m_vs.Get(); }
		ID3D11PixelShader* GetPixelShader() const { return m_ps.Get(); }
		ID3DBlob* GetVertexShaderByteCode() const { return m_vsByteCode.Get(); }
		ID3DBlob* GetPixelShaderByteCode() const { return m_psByteCode.Get(); }
		ID3D11InputLayout* GetVertexShaderInputLayout() const { return m_vsInputLayout.Get(); }
		ID3D11ShaderReflection* GetVertexShaderReflection() const { return m_vsReflection.Get(); }
		ID3D11ShaderReflection* GetPixelShaderReflection() const { return m_psReflection.Get(); }



		static std::shared_ptr<Shader> Create(GDX11::GDX11Context* context, const std::string& vertexFilename, const std::string& pixelFilename);
		static std::shared_ptr<Shader> Create(GDX11::GDX11Context* context, const std::string& vertexFilename);

	private:
		Shader(GDX11::GDX11Context* context, const std::string& vertexFilename, const std::string& pixelFilename);
		Shader(GDX11::GDX11Context* context, const std::string& vertexFilename);

		void CreateInputLayout();

		GDX11::GDX11Context* m_context;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vs = nullptr;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> m_vsByteCode = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> m_psByteCode = nullptr;

		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_vsInputLayout = nullptr;

		Microsoft::WRL::ComPtr<ID3D11ShaderReflection> m_vsReflection = nullptr;
		Microsoft::WRL::ComPtr<ID3D11ShaderReflection> m_psReflection = nullptr;

		std::unordered_map<std::string, uint32_t> m_resVSBindingCache;
		std::unordered_map<std::string, uint32_t> m_resPSBindingCache;



		// exception
	public:
		class Exception : public GDX11::GDX11Exception
		{
			using GDX11::GDX11Exception::GDX11Exception;
		};

		class ShaderCompilationException : public Exception
		{
		public:
			ShaderCompilationException(int line, const std::string& file, HRESULT hr, const std::string& info);
			virtual const char* what() const override;
			virtual const char* GetType() const override { return "Shader Compilation Exception"; }
			const std::string& GetErrorInfo() const { return m_info; }
			HRESULT GetErrorCode() const { return m_hr; }

		private:
			std::string m_info;
			HRESULT m_hr;
		};
	};
}