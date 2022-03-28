#include "VisibleGameObject.h"
#include <DirectXMath.h>
#include <WICTextureLoader.h>

#include "Graphics.h"
#include "Logger.h"

bool VisibleGameObject::AreObjectsLoading = false;

bool VisibleGameObject::Init(const std::string& model, const std::string textures[], UINT textureCount, ID3D11Device* device,
                             ID3D11DeviceContext* deviceContext)
{
	m_device = device;
	m_deviceContext = deviceContext;

	LoadModel(model);

	for (int i = 0; i < textureCount; ++i)
	{
		LoadTexture(textures[i]);
	}

	if (!m_objectCBuffer.Init(device, deviceContext))
	{
		LOG_ERROR("Failed to create constant buffer for object");
		return false;
	}

	m_modelFile = model;
	m_textureFile = textures[0];

	return true;
}

bool VisibleGameObject::InitAsync(const std::string& model, const std::string textures[], UINT textureCount,
	ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	m_device = device;
	m_deviceContext = deviceContext;

	AreObjectsLoading = true;
	
	m_loadModelTask = std::async(std::launch::async, [&]()
	{
		LoadModel(model);
	});

	for (int i = 0; i < textureCount; ++i)
	{
		LoadTexture(textures[i]);
	}

	if (!m_objectCBuffer.Init(device, deviceContext))
	{
		LOG_ERROR("Failed to create constant buffer for object");
		return false;
	}

	m_modelFile = model;
	m_textureFile = textures[0];

	return true;
}

bool VisibleGameObject::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	m_device = device;
	m_deviceContext = deviceContext;
	
	if (!m_objectCBuffer.Init(device, deviceContext))
	{
		LOG_ERROR("Failed to create constant buffer for object");
		return false;
	}

	m_loaded = true;

	return true;
}

VisibleGameObject::~VisibleGameObject()
{
	for (int i = 0; i < m_textures.size(); ++i)
	{
		m_textures[i]->Release();
	}
}

void VisibleGameObject::Render()
{
	if (!m_loaded) return;
	
	m_deviceContext->VSSetConstantBuffers(1, 1, m_objectCBuffer.GetBuffer());

	for (int i = 0; i < m_textures.size(); ++i)
	{
		m_deviceContext->PSSetShaderResources(0, 1, &m_textures[i]);
	}

	for (int i = 0; i < m_meshes.size(); ++i)
	{
		m_objectCBuffer.Data.worldMatrix = m_meshes[i]->GetTransform() * m_worldMatrix;
		m_objectCBuffer.ApplyChanges();
		m_meshes[i]->Render();
	}
}

bool VisibleGameObject::LoadTexture(const std::string& path)
{
	std::wstring wpath(path.begin(), path.end());

	ID3D11ShaderResourceView* texture;
	
	HRESULT hr = DirectX::CreateWICTextureFromFile(m_device, wpath.c_str(), nullptr, &texture);
	if (FAILED(hr))
	{
		LOG_ERROR_HR("Failed to load texture: " + path, hr);
		return false;
	}

	m_textures.push_back(texture);

	return true;
}

bool VisibleGameObject::LoadModel(const std::string& path)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	if (!scene) return false;

	aiNode* node = scene->mRootNode;

	LoadNode(node, scene, DirectX::XMMatrixIdentity());

	m_loaded = true;
	AreObjectsLoading = false;

	return true;
}

void VisibleGameObject::LoadNode(aiNode* node, const aiScene* scene, const DirectX::XMMATRIX& parentTransform)
{
	DirectX::XMMATRIX nodeTranform = DirectX::XMMatrixTranspose(DirectX::XMMATRIX(&node->mTransformation.a1)) * parentTransform;

	for (int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.emplace_back(LoadMesh(mesh, scene, nodeTranform));
	}

	for (int i = 0; i < node->mNumChildren; ++i)
	{
		LoadNode(node->mChildren[i], scene, nodeTranform);
	}
}

Mesh* VisibleGameObject::LoadMesh(aiMesh* mesh, const aiScene* scene, const DirectX::XMMATRIX& parentTransform)
{
	std::vector<Vertex> vertices;
	std::vector<DWORD> indices;


	for (int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex v;

		v.pos.x = mesh->mVertices[i].x;
		v.pos.y = mesh->mVertices[i].y;
		v.pos.z = mesh->mVertices[i].z;
	
		v.normal.x = mesh->mNormals[i].x;
		v.normal.y = mesh->mNormals[i].y;
		v.normal.z = mesh->mNormals[i].z;

		if (mesh->mTextureCoords[0])
		{
			v.texCoord.x = (float)mesh->mTextureCoords[0][i].x;
			v.texCoord.y = (float)mesh->mTextureCoords[0][i].y;
		}

		vertices.push_back(v);
	}

	for (int i = 0; i < mesh->mNumFaces; ++i)
	{
		for (int j = 0; j < mesh->mFaces[i].mNumIndices; ++j)
		{
			indices.push_back(mesh->mFaces[i].mIndices[j]);
		}
	}

	return new Mesh( m_device, m_deviceContext, vertices, indices, parentTransform);
}

void VisibleGameObject::UpdateMatrix()
{
	DirectX::XMFLOAT3 rot;
	DirectX::XMStoreFloat3(&rot, m_rotation);
	DirectX::XMFLOAT3 pos;
	DirectX::XMStoreFloat3(&pos, m_position);

	m_worldMatrix = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) *
					DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z) *
					DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

	UpdateDirectionVectors();
}
