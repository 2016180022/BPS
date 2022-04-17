#pragma once

#include "GameTimer.h"
#include "Shader.h"

class Scene
{
public:
	Scene();
	~Scene();

	bool OnProcessingMouseMessage(HWND hwnd, UINT messageID, WPARAM wParam, LPARAM lParam);
	bool OnProcesingKeyboardMessage(HWND hwnd, UINT messageID, WPARAM wParam, LPARAM lParam);

	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pD3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();
	/*void CreateGraphicsPipelineState(ID3D12Device* pD3dDevice);*/

	void BuildObjects(ID3D12Device* pD3dDevice, ID3D12GraphicsCommandList* pD3dCommandList);
	void ReleaseObjects();

	bool ProcessInput(UCHAR* pKeyBuffer);
	void AnimateObjects(float timeElapsed);

	void PrepareRender(ID3D12GraphicsCommandList* pD3dCommandList);
	void Render(ID3D12GraphicsCommandList* pD3dCommandList);

	void ReleaseUploadBuffers();


protected:
	Shader** ppShaders = NULL;
	int shaderNum = 0;

	ID3D12RootSignature* pD3dGraphicsRootSignature = NULL;
	ID3D12PipelineState* pD3dPipelineState = NULL;
};

