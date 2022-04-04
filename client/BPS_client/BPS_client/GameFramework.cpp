#include "stdafx.h"
#include "GameFramework.h"

GameFramework::GameFramework()
{
	m_hInstance = NULL;
	m_hwnd = NULL;

	pDxgiFactory = NULL;
	pDxgiSwapChain = NULL;
	pD3dDevice = NULL;

	pD3dCommandQueue = NULL;
	pD3dCommandAllocator = NULL;
	pD3dCommandList = NULL;

	pD3dPipelineState = NULL;

	for (int i = 0; i < swapChainBuffersNum; ++i)
		d3dRenderTargetBuffers[i] = NULL;
	pD3dRtvDescriptorHeap = NULL;
	rtvDescriptorIncrementSize = 0;

	d3dDepthStencilBuffer = NULL;
	pD3dDsvDescriptorHeap = NULL;
	dsvDescriptorIncrementSize = 0;

	swapChainBufferIndex = 0;

	pD3dFence = NULL;
	fenceValue = 0;
	fenceEvent = NULL;

	clientWidth = FRAME_BUFFER_WIDTH;
	clientHeight = FRAME_BUFFER_HEIGHT;

	_tcscpy_s(frameRateStr, _T("BPS Client ("));
}

GameFramework::~GameFramework()
{

}

bool GameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hwnd = hMainWnd;

	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
	CreateRenderTargetViews();
	CreateDepthStencilView();

	BuildObjects();

	return true;
}

void GameFramework::OnDestroy()
{
	WaitForGpuComplete();

	ReleaseObjects();

	::CloseHandle(fenceEvent);

	for (int i = 0; i < swapChainBuffersNum; ++i)
		if (d3dRenderTargetBuffers[i])
			d3dRenderTargetBuffers[i]->Release();
	if (pD3dRtvDescriptorHeap)
		pD3dRtvDescriptorHeap->Release();
	if (pD3dDsvDescriptorHeap)
		pD3dDsvDescriptorHeap->Release();
	if (pD3dCommandAllocator)
		pD3dCommandAllocator->Release();
	if (pD3dCommandQueue)
		pD3dCommandQueue->Release();
	if (pD3dPipelineState)
		pD3dPipelineState->Release();
	if (pD3dCommandList)
		pD3dCommandList->Release();
	if (pD3dFence)
		pD3dFence->Release();

	pDxgiSwapChain->SetFullscreenState(FALSE, NULL);
	if (pDxgiSwapChain)
		pDxgiSwapChain->Release();
	if (pD3dDevice)
		pD3dDevice->Release();
	if (pDxgiFactory)
		pDxgiFactory->Release();

#if defined(_DEBUG)
	IDXGIDebug1* pDxgiDebug = NULL;
	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), (void**)&pDxgiDebug);
	HRESULT hResult = pDxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
	pDxgiDebug->Release();
#endif
}

void GameFramework::CreateSwapChain()
{
	RECT rcClient;
	::GetClientRect(m_hwnd, &rcClient);
	clientWidth = rcClient.right - rcClient.left;
	clientHeight = rcClient.bottom - rcClient.top;

	DXGI_SWAP_CHAIN_DESC1 dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
	dxgiSwapChainDesc.Width = clientWidth;
	dxgiSwapChainDesc.Height = clientHeight;
	dxgiSwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.SampleDesc.Count = msaa4xEnable ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = msaa4xEnable ? (msaa4xQualityLevel - 1) : 0;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.BufferCount = swapChainBuffersNum;
	dxgiSwapChainDesc.Scaling = DXGI_SCALING_NONE;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	dxgiSwapChainDesc.Flags = 0;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC dxgiSwapChainFullScreenDesc;
	::ZeroMemory(&dxgiSwapChainFullScreenDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	dxgiSwapChainFullScreenDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainFullScreenDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Windowed = TRUE;

	pDxgiFactory->CreateSwapChainForHwnd(pD3dCommandQueue, m_hwnd, &dxgiSwapChainDesc,
		&dxgiSwapChainFullScreenDesc, NULL, (IDXGISwapChain1**)&pDxgiSwapChain);

	pDxgiFactory->MakeWindowAssociation(m_hwnd, DXGI_MWA_NO_ALT_ENTER);
	swapChainBufferIndex = pDxgiSwapChain->GetCurrentBackBufferIndex();
}

void GameFramework::CreateDirect3DDevice()
{
	HRESULT hResult;

	UINT nDxgiFactoryFlags = 0;
#if defined(_DEBUG)
	ID3D12Debug* pD3dDebugController = NULL;
	hResult = D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)&pD3dDebugController);
	if (pD3dDebugController)
	{
		pD3dDebugController->EnableDebugLayer();
		pD3dDebugController->Release();
	}
	nDxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	hResult = ::CreateDXGIFactory2(nDxgiFactoryFlags, __uuidof(IDXGIFactory4),
		(void**)&pDxgiFactory);

	IDXGIAdapter1* pD3dAdapter = NULL;
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != pDxgiFactory->EnumAdapters1(i, &pD3dAdapter); ++i)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pD3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(pD3dAdapter, D3D_FEATURE_LEVEL_12_0,
			__uuidof(ID3D12Device), (void**)&pD3dDevice))) break;
	}

	if (!pD3dAdapter)
	{
		pDxgiFactory->EnumWarpAdapter(__uuidof(IDXGIAdapter1), (void**)&pD3dAdapter);
		D3D12CreateDevice(pD3dAdapter, D3D_FEATURE_LEVEL_11_0,
			__uuidof(ID3D12Device), (void**)&pD3dDevice);
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4;
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	pD3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	msaa4xQualityLevel = d3dMsaaQualityLevels.NumQualityLevels;
	msaa4xEnable = (msaa4xQualityLevel > 1) ? true : false;

	hResult = pD3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence),
		(void**)&pD3dFence);
	fenceValue = 0;

	fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	d3dViewport.TopLeftX = 0;
	d3dViewport.TopLeftY = 0;
	d3dViewport.Width = static_cast<float>(clientWidth);
	d3dViewport.Height = static_cast<float>(clientHeight);
	d3dViewport.MinDepth = 0.0f;
	d3dViewport.MaxDepth = 1.0f;

	d3dScissorRect = { 0,0,clientWidth,clientHeight };

	if (pD3dAdapter)
		pD3dAdapter->Release();
}

void GameFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	HRESULT hResult = pD3dDevice->CreateCommandQueue(&d3dCommandQueueDesc,
		__uuidof(ID3D12CommandQueue), (void**)&pD3dCommandQueue);

	hResult = pD3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		__uuidof(ID3D12CommandAllocator), (void**)&pD3dCommandAllocator);
	
	if (pD3dCommandAllocator)	// C6387 warning
		hResult = pD3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
			pD3dCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList),
			(void**)&pD3dCommandList);

	hResult = pD3dCommandList->Close();
}

void GameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = swapChainBuffersNum;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	HRESULT hResult = pD3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc,
		__uuidof(ID3D12DescriptorHeap), (void**)&pD3dRtvDescriptorHeap);
	rtvDescriptorIncrementSize =
		pD3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = pD3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc,
		__uuidof(ID3D12DescriptorHeap), (void**)&pD3dDsvDescriptorHeap);
	dsvDescriptorIncrementSize =
		pD3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void GameFramework::CreateRenderTargetViews()
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCpuDescriptorHandle =
		pD3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	for (UINT i = 0; i < swapChainBuffersNum; ++i)
	{
		pDxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource),
			(void**)&d3dRenderTargetBuffers[i]);
		pD3dDevice->CreateRenderTargetView(d3dRenderTargetBuffers[i], NULL,
			d3dRtvCpuDescriptorHandle);
		d3dRtvCpuDescriptorHandle.ptr += rtvDescriptorIncrementSize;
	}
}

void GameFramework::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = clientWidth;
	d3dResourceDesc.Height = clientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = msaa4xEnable ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = msaa4xEnable ? (msaa4xQualityLevel - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;
	pD3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE,
		&d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue,
		__uuidof(ID3D12Resource), (void**)&d3dDepthStencilBuffer);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle =
		pD3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	pD3dDevice->CreateDepthStencilView(d3dDepthStencilBuffer, NULL, d3dDsvCPUDescriptorHandle);
}

void GameFramework::BuildObjects()
{

}

void GameFramework::ReleaseObjects()
{

}

void GameFramework::OnProcessingMouseMessage(HWND hwnd, UINT nMessageID,
	WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}

void GameFramework::OnProcesingKeyboardMessage(HWND hwnd, UINT nMessageID,
	WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case VK_RETURN:
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

LRESULT CALLBACK GameFramework::OnProcesingWindowMessage(HWND hwnd, UINT nMessageID,
	WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
	{
		clientWidth = LOWORD(lParam);
		clientHeight = HIWORD(lParam);
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hwnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcesingKeyboardMessage(hwnd, nMessageID, wParam, lParam);
		break;
	}

	return 0;
}

void GameFramework::ProcessInput()
{

}

void GameFramework::AnimateObjects()
{

}

void GameFramework::WaitForGpuComplete()
{
	fenceValue++;
	const UINT64 fence = fenceValue;
	HRESULT hResult = pD3dCommandQueue->Signal(pD3dFence, fence);

	if (pD3dFence->GetCompletedValue() < fence)
	{
		hResult = pD3dFence->SetEventOnCompletion(fence, fenceEvent);
		::WaitForSingleObject(fenceEvent, INFINITE);
	}
}

void GameFramework::FrameAdvance()
{
	gameTimer.Tick(0.0f);

	ProcessInput();
	AnimateObjects();

	HRESULT hResult = pD3dCommandAllocator->Reset();
	hResult = pD3dCommandList->Reset(pD3dCommandAllocator, NULL);

	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource = d3dRenderTargetBuffers[swapChainBufferIndex];
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	pD3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	pD3dCommandList->RSSetViewports(1, &d3dViewport);
	pD3dCommandList->RSSetScissorRects(1, &d3dScissorRect);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCpuDescriptorHandle =
		pD3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCpuDescriptorHandle.ptr +=
		static_cast<SIZE_T>(swapChainBufferIndex) *
		static_cast<SIZE_T>(rtvDescriptorIncrementSize);	// C26451 warning

	float clearColor[4] = { 0.0f,0.125f,0.3f,1.0f };
	pD3dCommandList->ClearRenderTargetView(d3dRtvCpuDescriptorHandle, clearColor, 0, NULL);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCpuDescriptorHandle =
		pD3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	pD3dCommandList->ClearDepthStencilView(d3dDsvCpuDescriptorHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	pD3dCommandList->OMSetRenderTargets(1, &d3dRtvCpuDescriptorHandle, TRUE,
		&d3dDsvCpuDescriptorHandle);

	// Rendering Code

	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	pD3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	hResult = pD3dCommandList->Close();

	ID3D12CommandList* ppD3dCommandLists[] = { pD3dCommandList };
	pD3dCommandQueue->ExecuteCommandLists(1, ppD3dCommandLists);

	WaitForGpuComplete();

	DXGI_PRESENT_PARAMETERS dxgiPresentParameters;
	dxgiPresentParameters.DirtyRectsCount = 0;
	dxgiPresentParameters.pDirtyRects = NULL;
	dxgiPresentParameters.pScrollRect = NULL;
	dxgiPresentParameters.pScrollOffset = NULL;
	pDxgiSwapChain->Present1(1, 0, &dxgiPresentParameters);

	swapChainBufferIndex = pDxgiSwapChain->GetCurrentBackBufferIndex();

	gameTimer.getFrameRate(frameRateStr + 12, 37);
	::SetWindowText(m_hwnd, frameRateStr);
}