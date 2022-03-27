#pragma once

#include <string>
#include <d3d11.h>
#include <functional>
#include <future>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "ConstantBuffer.h"
#include "GameObject.h"
#include "Mesh.h"

class VisibleGameObject : public GameObject
{
public:
	typedef std::function<void()> Callback;

	static bool AreObjectsLoading;
	
	bool Init(const std::string& model, const std::string textures[], UINT textureCount, ID3D11Device* device,
		ID3D11DeviceContext* deviceContext);
	bool InitAsync(const std::string& model, const std::string textures[], UINT textureCount, ID3D11Device* device,
		ID3D11DeviceContext* deviceContext);
	virtual bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~VisibleGameObject() override;

	virtual void Render();

	bool LoadTexture(const std::string& path);
	bool LoadModel(const std::string& path);
	void LoadNode(struct aiNode* node, const struct aiScene* scene, const DirectX::XMMATRIX& parentTransform);
	Mesh* LoadMesh(struct aiMesh* mesh, const aiScene* scene, const DirectX::XMMATRIX& parentTransform);

	std::vector<Mesh*>* GetMeshes() { return &m_meshes; }

	bool IsLoading() const { return m_loaded; }

protected:
	void UpdateMatrix() override;

	bool m_loaded = false;
	std::future<void> m_loadModelTask;
	std::future<void> m_loadTextureTask;

	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;

	std::vector<Mesh*> m_meshes;
	std::vector<ID3D11ShaderResourceView*> m_textures;
	DirectX::XMMATRIX m_worldMatrix = DirectX::XMMatrixIdentity();
	ConstantBuffer<ObjectCBuffer> m_objectCBuffer;

};
