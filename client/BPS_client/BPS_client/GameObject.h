#pragma once

#include "Mesh.h"
#include "Shader.h"

class Shader;

class GameObject
{
private:
	int references = 0;

protected:
	XMFLOAT4X4 worldMatrix4x4;
	Mesh* m_pMesh = NULL;
	Shader* m_pShader = NULL;

public:
	GameObject();
	virtual ~GameObject();

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

	virtual void SetMesh(Mesh* pMesh);
	virtual void SetShader(Shader* pShader);

	virtual void Animate(float timeElapsed);

	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* pD3dCommandList);
};

