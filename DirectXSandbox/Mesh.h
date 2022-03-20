#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <vector>

struct Vertex;

class Mesh
{
public:
	Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<Vertex>& vertices, std::vector<DWORD>& indices, const DirectX::XMMATRIX& parentTransform);
	~Mesh();

	void Render();
	const DirectX::XMMATRIX& GetTransform() { return m_transform; }

private:
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;

	int m_indexCount = 0;
	int m_vertexCount = 0;

	DirectX::XMMATRIX m_transform;

};
