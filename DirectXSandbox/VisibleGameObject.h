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
#include "Stencil.h"

class VisibleGameObject : public GameObject
{
public:
	typedef std::function<void()> Callback;

	static bool AreObjectsLoading;
	
	bool Init(const std::string& model, ID3D11Device* device,
		ID3D11DeviceContext* deviceContext);
	bool InitAsync(const std::string& model, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual ~VisibleGameObject() override;

	virtual void Render(UINT technique);

	bool LoadModel(const std::string& path);
	void LoadNode(struct aiNode* node, const struct aiScene* scene, const DirectX::XMMATRIX& parentTransform);
	Mesh* LoadMesh(struct aiMesh* mesh, const aiScene* scene, const DirectX::XMMATRIX& parentTransform);

	std::vector<Mesh*>* GetMeshes() { return &m_meshes; }

	bool IsLoading() const { return m_loaded; }
	const std::string& GetModelFile() const { return m_modelFile; }
	const std::string& GetTextureFile() const { return m_textureFile; }
	void SetDrawOutline(bool drawOutline) { m_drawOutline = drawOutline; }

protected:
	void UpdateMatrix() override;

	bool m_loaded = false;
	std::future<bool> m_loadModelTask;
	std::future<void> m_loadTextureTask;

	std::string m_modelFile;
	std::string m_textureFile;

	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;

	std::vector<Mesh*> m_meshes;
	DirectX::XMMATRIX m_worldMatrix = DirectX::XMMatrixIdentity();
	ConstantBuffer<ObjectCBuffer> m_objectCBuffer;

	bool m_drawOutline = false;
	Stencil* m_stencilMask = nullptr;
	Stencil* m_stencilWrite = nullptr;

	void RenderMeshes();
	
	void SetTechniqueDrawNormal();
	void SetTechniqueDrawOutline();
	
};
