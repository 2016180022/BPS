#include "Shader.h"

Shader::Shader()
{

}

Shader::~Shader()
{
	if (ppD3dPipelineStates)
	{
		for (int i = 0; i < pipelineStateNum; ++i)
			if (ppD3dPipelineStates[i])
				ppD3dPipelineStates[i]->Release();
		delete[] ppD3dPipelineStates;
	}
}

D3D12_RASTERIZER_DESC Shader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizeDesc;
	::ZeroMemory(&d3dRasterizeDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizeDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizeDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizeDesc.FrontCounterClockwise = FALSE;
	d3dRasterizeDesc.DepthBias = 0;
	d3dRasterizeDesc.DepthBiasClamp = 0.0f;
	d3dRasterizeDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizeDesc.DepthClipEnable = TRUE;
	d3dRasterizeDesc.MultisampleEnable = FALSE;
	d3dRasterizeDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizeDesc.ForcedSampleCount = 0;
	d3dRasterizeDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return d3dRasterizeDesc;
}

D3D12_DEPTH_STENCIL_DESC Shader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return d3dDepthStencilDesc;
}

D3D12_BLEND_DESC Shader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return d3dBlendDesc;
}

D3D12_INPUT_LAYOUT_DESC Shader::CreateInputLayout()
{
	UINT inputElementDescNum = 2;
	D3D12_INPUT_ELEMENT_DESC* pD3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[inputElementDescNum];

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pD3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = inputElementDescNum;

	return d3dInputLayoutDesc;
}

D3D12_SHADER_BYTECODE Shader::CreateVertexShader(ID3DBlob** ppD3dShaderBlob)
{
	return CompileShaderFromFile(L"Shaders.hlsl", "VSMain", "vs_5_1", ppD3dShaderBlob);
}

D3D12_SHADER_BYTECODE Shader::CreatePixelShader(ID3DBlob** ppD3dShaderBlob)
{
	return CompileShaderFromFile(L"Shaders.hlsl", "PSMain", "ps_5_1", ppD3dShaderBlob);
}

D3D12_SHADER_BYTECODE Shader::CompileShaderFromFile(const WCHAR* pFileName, LPCSTR pShaderName, LPCSTR pShaderProfile,
	ID3DBlob** ppD3dShaderBlob)
{
	UINT compileFlagNum = 0;
#if defined(_DEBUG)
	compileFlagNum = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	::D3DCompileFromFile(pFileName, NULL, NULL, pShaderName, pShaderProfile, compileFlagNum, 0, ppD3dShaderBlob, NULL);

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppD3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppD3dShaderBlob)->GetBufferPointer();

	return d3dShaderByteCode;
}

void Shader::CreateShader(ID3D12Device* pD3dDevice, ID3D12RootSignature* pD3dRootSignatue)
{
	pipelineStateNum = 1;
	ppD3dPipelineStates = new ID3D12PipelineState * [pipelineStateNum];

	ID3DBlob* pD3dVertexShaderBlob = NULL;
	ID3DBlob* pD3dPixelShaderBlob = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = pD3dRootSignatue;
	d3dPipelineStateDesc.VS = CreateVertexShader(&pD3dVertexShaderBlob);
	d3dPipelineStateDesc.PS = CreatePixelShader(&pD3dPixelShaderBlob);
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	d3dPipelineStateDesc.BlendState = CreateBlendState();
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets = 1;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;

	pD3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState),
		(void**)&ppD3dPipelineStates[0]);

	if (pD3dVertexShaderBlob)
		pD3dVertexShaderBlob->Release();
	if (pD3dPixelShaderBlob)
		pD3dPixelShaderBlob->Release();

	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs)
		delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void Shader::BuildObjects(ID3D12Device* pD3dDevice, ID3D12GraphicsCommandList* pD3dCommandList, void* context)
{
	TriangleMesh* pTriangleMesh = new TriangleMesh(pD3dDevice, pD3dCommandList);

	objectNum = 1;
	ppObjects = new GameObject * [objectNum];

	ppObjects[0] = new GameObject();
	ppObjects[0]->SetMesh(pTriangleMesh);
}

void Shader::ReleaseObjects()
{
	if (ppObjects)
	{
		for (int i = 0; i < objectNum; ++i)
			if (ppObjects[i])
				delete ppObjects[i];
		delete[] ppObjects;
	}
}

void Shader::AnimateObjects(float timeElpased)
{
	for (int i = 0; i < objectNum; ++i)
		ppObjects[i]->Animate(timeElpased);
}

void Shader::ReleaseUploadBuffers()
{
	if (ppObjects)
	{
		for (int i = 0; i < objectNum; ++i)
			if (ppObjects)
				ppObjects[i]->ReleaseUploadBuffers();
	}
}

void Shader::OnPrepareRender(ID3D12GraphicsCommandList* pD3dCommandList)
{
	pD3dCommandList->SetPipelineState(ppD3dPipelineStates[0]);
}

void Shader::Render(ID3D12GraphicsCommandList* pD3dCommandList)
{
	OnPrepareRender(pD3dCommandList);

	for (int i = 0; i < objectNum; ++i)
		if (ppObjects[i])
			ppObjects[i]->Render(pD3dCommandList);
}