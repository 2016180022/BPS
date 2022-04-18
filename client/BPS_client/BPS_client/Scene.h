#pragma once

#include "GameTimer.h"

class Scene
{
public:
	Scene();
	~Scene();

	bool OnProcessingMouseMessage(HWND hwnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcesingKeyboardMessage(HWND hwnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void CreateGraphicsRootSignature(ID3D12Device* pD3dDevice);
	void CreateGraphicsPipelineState(ID3D12Device* pD3dDevice);

	void BuildObjects(ID3D12Device* pD3dDevice);
	void ReleaseObjects();

	bool ProcessInput();
	void AnimateObjects(float timeElapsed);

	void PrepareRender(ID3D12GraphicsCommandList* pD3dCommandList);
	void Render(ID3D12GraphicsCommandList* pD3dCommandList);

	ID3D12RootSignature* pD3dGraphicsRootSignature = NULL;
	ID3D12PipelineState* pD3dPipelineState = NULL;
};

