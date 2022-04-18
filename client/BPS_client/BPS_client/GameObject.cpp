#include "GameObject.h"

GameObject::GameObject()
{
	XMStoreFloat4x4(&worldMatrix4x4, XMMatrixIdentity());
}

GameObject::~GameObject()
{
	if (m_pMesh)	m_pMesh->Release();
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
}

void GameObject::SetMesh(Mesh* pMesh)
{
	if (m_pMesh)	m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh)	m_pMesh->AddRef();
}

void GameObject::SetShader(Shader* pShader)
{
	if (m_pShader)	m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader)	m_pShader->AddRef();
}

void GameObject::ReleaseUploadBuffers()
{
	if (m_pMesh)	m_pMesh->ReleaseUploadBuffers();
}

void GameObject::Animate(float timeElapsed)
{

}

void GameObject::OnPrepareRender()
{

}

void GameObject::Render(ID3D12GraphicsCommandList* pD3dCommandList)
{
	OnPrepareRender();

	if (m_pShader) m_pShader->Render(pD3dCommandList);
	if (m_pMesh)	m_pMesh->Render(pD3dCommandList);
}