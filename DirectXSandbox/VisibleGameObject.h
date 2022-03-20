#pragma once

#include <string>
#include <d3d11.h>
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
	bool Init(const std::string& model, const std::string textures[], UINT textureCount, ID3D11Device* device,
		ID3D11DeviceContext* deviceContext);
	~VisibleGameObject() override;

	void Render();

	bool LoadTexture(const std::string& path);
	bool LoadModel(const std::string& path);
	void LoadNode(struct aiNode* node, const struct aiScene* scene, const DirectX::XMMATRIX& parentTransform);
	Mesh* LoadMesh(struct aiMesh* mesh, const aiScene* scene, const DirectX::XMMATRIX& parentTransform);

protected:
	void UpdateMatrix() override;

	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;

	std::vector<Mesh*> m_meshes;
	std::vector<ID3D11ShaderResourceView*> m_textures;
	DirectX::XMMATRIX m_worldMatrix = DirectX::XMMatrixIdentity();
	ConstantBuffer<ObjectCBuffer> m_objectCBuffer;

};
