#pragma once

#include "GameObject.h"

class Camera3D : public GameObject
{
public:
	void Init(int width, int height);
	void SetProjection(float fov, float farZ, float nearZ);
	void Resize(int width, int height);

	DirectX::XMMATRIX& GetViewMatrix() { return m_viewMatrix; }
	DirectX::XMMATRIX& GetProjectionMatrix() { return m_projectionMatrix; }

protected:
	void UpdateMatrix() override;

private:
	void UpdateProjection();

	DirectX::XMMATRIX m_viewMatrix;
	DirectX::XMMATRIX m_projectionMatrix;

	float m_fov = 90.f;
	float m_farZ = 1000.f;
	float m_nearZ = 0.1f;
	int m_width;
	int m_height;

};
