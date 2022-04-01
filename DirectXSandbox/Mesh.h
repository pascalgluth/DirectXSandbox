#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

#include "Texture.h"

struct Vertex;

class Mesh
{
public:
	Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<Vertex>& vertices, std::vector<DWORD>& indices, std::vector<Texture>& textures, const DirectX::XMMATRIX& parentTransform);
	~Mesh();

	void Render();
	const DirectX::XMMATRIX& GetTransform() { return m_transform; }

	std::vector<Vertex> Vertices;
	std::vector<DWORD> Indices;

private:
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;

	int m_indexCount = 0;
	int m_vertexCount = 0;

	std::vector<Texture> m_textures;

	DirectX::XMMATRIX m_transform;

};
