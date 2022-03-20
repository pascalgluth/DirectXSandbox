#include "Camera3D.h"

void Camera3D::Init(int width, int height)
{
	//m_position = DirectX::XMVectorSet(0.f, 0.f, 2.f, 0.f);
	//m_up = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f), DirectX::XMMatrixRotationRollPitchYaw(0.f, 0.f, 0.f));
	//m_target = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(0.f, 0.f, 1.f, 0.f), DirectX::XMMatrixRotationRollPitchYaw(0.f, 0.f, 0.f));
	//m_rotation = DirectX::XMVectorSet(0.f, 0.f, 0.f, 0.f);

	Resize(width, height);
	UpdateDirectionVectors();
	UpdateMatrix();
}

void Camera3D::SetProjection(float fov, float farZ, float nearZ)
{
	m_fov = fov;
	m_farZ = farZ;
	m_nearZ = nearZ;

	UpdateProjection();
}

void Camera3D::Resize(int width, int height)
{
	m_width = width;
	m_height = height;

	UpdateProjection();
}

void Camera3D::UpdateMatrix()
{
	DirectX::XMFLOAT3 rotf3;
	DirectX::XMStoreFloat3(&rotf3, m_rotation);

	DirectX::XMMATRIX camRotation = DirectX::XMMatrixRotationRollPitchYaw(rotf3.x, rotf3.y, rotf3.z);
	DirectX::XMVECTOR camTarget = DirectX::XMVectorAdd(m_forward, m_position);

	DirectX::XMVECTOR upDireciton = DirectX::XMVector3TransformCoord(DEFAULT_UP, camRotation);

	m_viewMatrix = DirectX::XMMatrixLookAtLH(m_position, camTarget, upDireciton);
}

void Camera3D::UpdateProjection()
{
	m_projectionMatrix = DirectX::XMMatrixPerspectiveLH((m_fov / 360.f) * DirectX::XM_2PI, (float)m_width / (float)m_height, m_nearZ, m_farZ);
}
