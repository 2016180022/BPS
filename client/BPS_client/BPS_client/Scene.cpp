#include "Scene.h"

Scene::Scene()
{

}

Scene::~Scene()
{

}

void Scene::CreateGraphicsRootSignature(ID3D12Device* pD3dDevice)
{
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = 0;
	d3dRootSignatureDesc.pParameters = NULL;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	ID3DBlob* pD3dSignatureBlob = NULL;
	ID3DBlob* pD3dErrorBlob = NULL;

	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&pD3dSignatureBlob, &pD3dErrorBlob);
	pD3dDevice->CreateRootSignature(0, pD3dSignatureBlob->GetBufferPointer(),
		pD3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pD3dGraphicsRootSignature);

	if (pD3dSignatureBlob) pD3dSignatureBlob->Release();
	if (pD3dErrorBlob) pD3dErrorBlob->Release();
}

void Scene::CreateGraphicsPipelineState(ID3D12Device* pD3dDevice)
{
	ID3DBlob* pD3dVertexShaderBlob = NULL;
	ID3DBlob* pD3dPixelShaderBlob = NULL;

	UINT compileFlags = 0;
#if defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	D3DCompileFromFile(L"Shaders.hlsl", NULL, NULL, "VSMain", "vs_5_1", compileFlags, 0, &pD3dVertexShaderBlob, NULL);
	D3DCompileFromFile(L"Shaders.hlsl", NULL, NULL, "PSMain", "ps_5_1", compileFlags, 0, &pD3dPixelShaderBlob, NULL);

	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

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

	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = pD3dGraphicsRootSignature;
	d3dPipelineStateDesc.VS.pShaderBytecode = pD3dVertexShaderBlob->GetBufferPointer();
	d3dPipelineStateDesc.VS.BytecodeLength = pD3dVertexShaderBlob->GetBufferSize();
	d3dPipelineStateDesc.PS.pShaderBytecode = pD3dPixelShaderBlob->GetBufferPointer();
	d3dPipelineStateDesc.PS.BytecodeLength = pD3dPixelShaderBlob->GetBufferSize();
	d3dPipelineStateDesc.RasterizerState = d3dRasterizerDesc;
	d3dPipelineStateDesc.BlendState = d3dBlendDesc;
	d3dPipelineStateDesc.DepthStencilState.DepthEnable = FALSE;
	d3dPipelineStateDesc.DepthStencilState.StencilEnable = FALSE;
	d3dPipelineStateDesc.InputLayout.pInputElementDescs = NULL;
	d3dPipelineStateDesc.InputLayout.NumElements = 0;
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets = 1;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.SampleDesc.Quality = 0;
	
	pD3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc,
		__uuidof(ID3D12PipelineState), (void**)&pD3dPipelineState);

	if (pD3dVertexShaderBlob) pD3dVertexShaderBlob->Release();
	if (pD3dPixelShaderBlob) pD3dPixelShaderBlob->Release();
}

void Scene::BuildObjects(ID3D12Device* pD3dDevice)
{
	CreateGraphicsRootSignature(pD3dDevice);
	CreateGraphicsPipelineState(pD3dDevice);
}

void Scene::ReleaseObjects()
{
	if (pD3dGraphicsRootSignature)
		pD3dGraphicsRootSignature->Release();

	if (pD3dPipelineState)
		pD3dPipelineState->Release();
}

bool Scene::OnProcessingMouseMessage(HWND hwnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool Scene::OnProcesingKeyboardMessage(HWND hwnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}
bool Scene::ProcessInput()
{
	return false;
}

void Scene::AnimateObjects(float timeElapsed)
{

}

void Scene::PrepareRender(ID3D12GraphicsCommandList* pD3dCommandList)
{
	pD3dCommandList->SetGraphicsRootSignature(pD3dGraphicsRootSignature);
	pD3dCommandList->SetPipelineState(pD3dPipelineState);
	pD3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Scene::Render(ID3D12GraphicsCommandList* pD3dCommandList)
{
	PrepareRender(pD3dCommandList);
	pD3dCommandList->DrawInstanced(3, 1, 0, 0);
}