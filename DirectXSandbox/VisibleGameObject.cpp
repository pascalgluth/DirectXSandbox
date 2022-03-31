#include "VisibleGameObject.h"
#include <DirectXMath.h>
#include <WICTextureLoader.h>

#include "Engine.h"
#include "Graphics.h"
#include "Logger.h"

bool VisibleGameObject::AreObjectsLoading = false;

bool VisibleGameObject::Init(const std::string& model, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	m_device = device;
	m_deviceContext = deviceContext;

	m_stencilMask = new Stencil(device, deviceContext, Stencil::MASK);
	m_stencilWrite = new Stencil(device, deviceContext, Stencil::WRITE);

	LoadModel(model);

	if (!m_objectCBuffer.Init(device, deviceContext))
	{
		LOG_ERROR("Failed to create constant buffer for object");
		return false;
	}

	m_modelFile = model;

	return true;
}

bool VisibleGameObject::InitAsync(const std::string& model, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	m_device = device;
	m_deviceContext = deviceContext;

	AreObjectsLoading = true;
	
	/*m_loadModelTask = std::async(std::launch::async, [&]()
	{
		LoadModel(model);
	});*/

	m_stencilMask = new Stencil(device, deviceContext, Stencil::MASK);
	m_stencilWrite = new Stencil(device, deviceContext, Stencil::WRITE);

	m_loadModelTask = std::async(std::launch::async, &VisibleGameObject::LoadModel, this, model);
	
	if (!m_objectCBuffer.Init(device, deviceContext))
	{
		LOG_ERROR("Failed to create constant buffer for object");
		return false;
	}

	m_modelFile = model;

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
	
	// Pass 1: Drawing and masking the normal version
	{
		m_stencilWrite->Bind();
		m_deviceContext->PSSetShader(Engine::GetGFX()->m_scenePixelShader.GetShader(), NULL, 0);

		for (int i = 0; i < m_meshes.size(); ++i)
		{
			m_objectCBuffer.Data.worldMatrix = m_meshes[i]->GetTransform() * m_worldMatrix;
			m_objectCBuffer.ApplyChanges();
			m_meshes[i]->Render();
		}
	}

	// Pass 2: Drawing the red outline with the mask
	{
		DirectX::XMFLOAT3 oldScale = GetScaleF3();
		SetScale(oldScale.x + 0.02f, oldScale.y + 0.02f, oldScale.z + 0.02f);
		m_stencilMask->Bind();
		m_deviceContext->PSSetShader(Engine::GetGFX()->m_solidColorPS.GetShader(), NULL, 0);
	
		for (int i = 0; i < m_meshes.size(); ++i)
		{
			m_objectCBuffer.Data.worldMatrix = m_meshes[i]->GetTransform() * m_worldMatrix;
			m_objectCBuffer.ApplyChanges();
			m_meshes[i]->Render();
		}
		SetScale(oldScale.x, oldScale.y, oldScale.z);
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

	m_deviceContext->GenerateMips(texture);

	m_textures.push_back(texture);

	return true;
}

bool VisibleGameObject::LoadModel(const std::string& path)
{
	try
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
	catch (...)
	{
		LOG_ERROR("Error loading model: " + path);
		AreObjectsLoading = false;
		return false;		
	}
}

void VisibleGameObject::LoadNode(aiNode* node, const aiScene* scene, const DirectX::XMMATRIX& parentTransform)
{
	DirectX::XMMATRIX nodeTranform = DirectX::XMMatrixTranspose(DirectX::XMMATRIX(&node->mTransformation.a1)) * parentTransform;

	for (UINT i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.emplace_back(LoadMesh(mesh, scene, nodeTranform));
	}

	for (UINT i = 0; i < node->mNumChildren; ++i)
	{
		LoadNode(node->mChildren[i], scene, nodeTranform);
	}
}

Mesh* VisibleGameObject::LoadMesh(aiMesh* mesh, const aiScene* scene, const DirectX::XMMATRIX& parentTransform)
{
	std::vector<Vertex> vertices;
	std::vector<DWORD> indices;


	for (UINT i = 0; i < mesh->mNumVertices; ++i)
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

	for (UINT i = 0; i < mesh->mNumFaces; ++i)
	{
		for (UINT j = 0; j < mesh->mFaces[i].mNumIndices; ++j)
		{
			indices.push_back(mesh->mFaces[i].mIndices[j]);
		}
	}

	std::vector<Texture> texturesForMesh;

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		aiString textureFileName;
		std::string stdTextureFileName;

		if (material->GetTexture(aiTextureType_SPECULAR, 0, &textureFileName) == aiReturn_SUCCESS)
		{
			stdTextureFileName = textureFileName.C_Str();
			texturesForMesh.emplace_back(m_device, FILE_TEXTURE(stdTextureFileName), 1);
		}

		aiReturn ret = material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFileName);

		if (ret == aiReturn_SUCCESS)
		{
			stdTextureFileName = textureFileName.C_Str();
			texturesForMesh.emplace_back(m_device, FILE_TEXTURE(stdTextureFileName), 0);
		}
	}

	return new Mesh(m_device, m_deviceContext, vertices, indices, texturesForMesh, parentTransform);
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
