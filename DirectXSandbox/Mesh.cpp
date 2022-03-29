#include "Mesh.h"
#include "Logger.h"
#include <vector>

#include "Graphics.h"

Mesh::Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<Vertex>& vertices, std::vector<DWORD>& indices, std::vector<Texture>& textures, const DirectX::XMMATRIX& parentTransform)
{
	m_device = device;
	m_deviceContext = deviceContext;

	this->Vertices = vertices;
	this->Indices = indices;
	m_textures = textures;

	m_transform = parentTransform;

	m_vertexCount = static_cast<int>(vertices.size());
	m_indexCount = static_cast<int>(indices.size());

	UINT size = sizeof(Vertex) * m_vertexCount;
	CD3D11_BUFFER_DESC vertexBufferDesc(size, D3D11_BIND_VERTEX_BUFFER);
	D3D11_SUBRESOURCE_DATA vertexData{};
	vertexData.pSysMem = vertices.data();
	HRESULT hr = m_device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(hr))
	{
		LOG_ERROR_HR("Failed to create vertex buffer for mesh", hr);
		return;
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	
	D3D11_SUBRESOURCE_DATA indexData{};
	indexData.pSysMem = indices.data();
	
	hr = m_device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(hr))
	{
		LOG_ERROR_HR("Failed to create index buffer for mesh", hr);
		return;
	}
}

Mesh::~Mesh()
{
	m_vertexBuffer->Release();
	m_indexBuffer->Release();
}

void Mesh::Render()
{
	for (int i = 0; i < m_textures.size(); ++i)
	{
		m_deviceContext->PSSetShaderResources(m_textures[i].GetSlot(), 1, m_textures[i].GetTextureResourceViewAddress());
	}
	
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	m_deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	m_deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	m_deviceContext->DrawIndexed(m_indexCount, 0, 0);
}
