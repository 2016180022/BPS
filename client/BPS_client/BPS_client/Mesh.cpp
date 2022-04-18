#include "Mesh.h"

Mesh::Mesh(ID3D12Device* d3dDevice, ID3D12GraphicsCommandList* pD3dCommandList)
{

}

Mesh::~Mesh()
{
	if (pD3dVertexBuffer)	pD3dVertexBuffer->Release();
	if (pD3dVertexUploadBuffer)	pD3dVertexUploadBuffer->Release();
}

void Mesh::ReleaseUploadBuffers()
{
	if (pD3dVertexUploadBuffer)
		pD3dVertexUploadBuffer->Release();
	pD3dVertexUploadBuffer = NULL;
}

void Mesh::Render(ID3D12GraphicsCommandList* pD3dCommandList)
{
	pD3dCommandList->IASetPrimitiveTopology(d3dPrimitiveTopology);
	pD3dCommandList->IASetVertexBuffers(slotNum, 1, &d3dVertexBufferView);
	pD3dCommandList->DrawInstanced(vertexNum, 1, offsetNum, 0);
}

TriangleMesh::TriangleMesh(ID3D12Device* pD3dDevice, ID3D12GraphicsCommandList* pD3dCommandList) :
	Mesh(pD3dDevice, pD3dCommandList)
{
	vertexNum = 3;
	strideNum = sizeof(DiffusedVertex);
	d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	DiffusedVertex vertices[3];
	vertices[0] = DiffusedVertex(XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	vertices[1] = DiffusedVertex(XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
	vertices[2] = DiffusedVertex(XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(Colors::Blue));

	pD3dVertexBuffer = ::CreateBufferResource(pD3dDevice, pD3dCommandList, vertices, strideNum * vertexNum,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &pD3dVertexUploadBuffer);

	d3dVertexBufferView.BufferLocation = pD3dVertexBuffer->GetGPUVirtualAddress();
	d3dVertexBufferView.StrideInBytes = strideNum;
	d3dVertexBufferView.SizeInBytes = strideNum * vertexNum;
}