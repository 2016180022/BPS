#pragma once

#include "GameTimer.h"
#include "Scene.h"

class GameFramework
{
private:
	Scene* pScene;

	HINSTANCE m_hInstance;
	HWND m_hwnd;

	int clientWidth;
	int clientHeight;

	IDXGIFactory4* pDxgiFactory;
	IDXGISwapChain3* pDxgiSwapChain;
	ID3D12Device* pD3dDevice;

	bool msaa4xEnable = false;
	UINT msaa4xQualityLevel = 0;

	static const UINT swapChainBuffersNum = 2;
	UINT swapChainBufferIndex;
	ID3D12Resource* d3dSwapChainBackBuffers[swapChainBuffersNum];

	ID3D12Resource* d3dRenderTargetBuffers[swapChainBuffersNum];
	ID3D12DescriptorHeap* pD3dRtvDescriptorHeap;
	UINT rtvDescriptorIncrementSize;

	ID3D12Resource* d3dDepthStencilBuffer;
	ID3D12DescriptorHeap* pD3dDsvDescriptorHeap;
	UINT dsvDescriptorIncrementSize;

	ID3D12CommandQueue* pD3dCommandQueue;
	ID3D12CommandAllocator* pD3dCommandAllocator;
	ID3D12GraphicsCommandList* pD3dCommandList;

	ID3D12PipelineState* pD3dPipelineState;

	ID3D12Fence* pD3dFence;
	UINT64 fenceValue;
	HANDLE fenceEvent;
	UINT64 fenceValues[swapChainBuffersNum];

	D3D12_VIEWPORT d3dViewport;
	D3D12_RECT d3dScissorRect;

	GameTimer gameTimer;
	_TCHAR frameRateStr[50];

	UINT64 fenceValues[swapChainBuffersNum];
	Scene* pScene;

public:
	GameFramework();
	~GameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void ChangeSwapChainState();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();
	
	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	void BuildObjects();
	void ReleaseObjects();

	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();
	void MoveToNextFrame();

	void WaitForGpuComplete();

	void OnProcessingMouseMessage(HWND hwnd, UINT nMessageID, WPARAM wParam,
		LPARAM lParam);
	void OnProcesingKeyboardMessage(HWND hwnd, UINT nMessageID, WPARAM wParam,
		LPARAM lParam);
	LRESULT CALLBACK OnProcesingWindowMessage(HWND hwnd, UINT nMessageID, WPARAM wParam,
		LPARAM lParam);
};