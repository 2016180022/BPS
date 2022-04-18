#pragma once

#include "stdafx.h"

class Mesh
{
private:
	int references = 0;

protected:
	ID3D12Resource* pD3dVertexBuffer = NULL;
	ID3D12Resource* pD3dVertexUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW d3dVertexBufferView;
	D3D12_PRIMITIVE_TOPOLOGY d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	UINT slotNum = 0;
	UINT vertexNum = 0;
	UINT strideNum = 0;
	UINT offsetNum = 0;

public:
	Mesh(ID3D12Device* d3dDevice, ID3D12GraphicsCommandList* pD3dCommandList);
	virtual ~Mesh();

	void AddRef()
	{
		references++;
	}

	void Release()
	{
		if (--references <= 0)
			delete this;
	}

	void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList* pD3dCommandList);
};

class TriangleMesh :public Mesh {
public:
	TriangleMesh(ID3D12Device* pD3dDevice, ID3D12GraphicsCommandList* pD3dCommandList);
	virtual ~TriangleMesh() {  }
};

class Vertex {
protected:
	XMFLOAT3 position;

public:
	Vertex() :position(XMFLOAT3(0.0f, 0.0f, 0.0f)) {  }
	Vertex(XMFLOAT3 position) :position(position) {  }
	~Vertex() {  }
};

class DiffusedVertex :public Vertex {
protected:
	XMFLOAT4 diffuse;

public:
	DiffusedVertex() : diffuse(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)) { position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	DiffusedVertex(float x, float y, float z, XMFLOAT4 diffuse) :diffuse(diffuse) { position = XMFLOAT3(x, y, z); }
	DiffusedVertex(XMFLOAT3 fPosition, XMFLOAT4 diffuse) :diffuse(diffuse) { position = fPosition; }
	~DiffusedVertex() {  }
};