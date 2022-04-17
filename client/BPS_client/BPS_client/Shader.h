#pragma once

#include "GameObject.h"

class GameObject;

class Shader
{
private:
	int references = 0;

protected:
	GameObject** ppObjects = NULL;
	int objectNum = 0;

	ID3D12PipelineState** ppD3dPipelineStates = NULL;
	int pipelineStateNum = 0;

public:
	Shader();
	virtual ~Shader();

	void AddRef()
	{
		references++;
	}

	void Release()
	{
		if (--references <= 0)
			delete this;
	}

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppD3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppD3dShaderBlob);

	D3D12_SHADER_BYTECODE CompileShaderFromFile(const WCHAR* pFimeName, LPCSTR pShaderName, LPCSTR pShaderProfile,
		ID3DBlob** ppD3dShaderBlob);

	virtual void CreateShader(ID3D12Device* pD3dDevice, ID3D12RootSignature* pD3dRootSignature);
	
	virtual void CreateShaderVariables(ID3D12Device* pD3dDevice, ID3D12GraphicsCommandList* pD3dCommandList) {  }
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pD3dCommandList) {  }
	virtual void ReleaseShaderVariables() {  }

	virtual void ReleaseUploadBuffers();

	virtual void BuildObjects(ID3D12Device* pD3dDevice, ID3D12GraphicsCommandList* pD3dCommandList,
		void* context = NULL);
	virtual void AnimateObjects(float timeElpased);
	virtual void ReleaseObjects();

	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pD3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pD3dCommandList);
};

