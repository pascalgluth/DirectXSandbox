#include "GameObject.h"

const DirectX::XMVECTOR& GameObject::GetPosition()
{
	return m_position;
}

DirectX::XMFLOAT3 GameObject::GetPositonF3()
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMStoreFloat3(&pos, m_position);
	return pos;
}

const DirectX::XMVECTOR& GameObject::GetRotation()
{
	return m_rotation;
}

DirectX::XMFLOAT3 GameObject::GetRotationF3()
{
	DirectX::XMFLOAT3 rot;
	DirectX::XMStoreFloat3(&rot, m_rotation);
	return rot;
}

void GameObject::SetPosition(const DirectX::XMVECTOR& pos)
{
	m_position = pos;
	UpdateMatrix();
}

void GameObject::SetPosition(const DirectX::XMFLOAT3& pos)
{
	m_position = DirectX::XMLoadFloat3(&pos);
	UpdateMatrix();
}

void GameObject::AddOffset(float ox, float oy, float oz)
{
	AddOffset({ ox, oy, oz });
}

void GameObject::AddOffset(const DirectX::XMFLOAT3& offset)
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMStoreFloat3(&pos, m_position);

	pos.x += offset.x;
	pos.y += offset.y;
	pos.z += offset.z;

	m_position = DirectX::XMLoadFloat3(&pos);
	UpdateMatrix();
}

void GameObject::SetRotation(const DirectX::XMVECTOR& rot)
{
	m_rotation = rot;
	UpdateMatrix();
	UpdateDirectionVectors();
}

void GameObject::SetRotation(const DirectX::XMFLOAT3& rot)
{
	m_rotation = DirectX::XMLoadFloat3(&rot);
	UpdateMatrix();
	UpdateDirectionVectors();
}

void GameObject::AddRotationOffset(float x, float y, float z)
{
	DirectX::XMFLOAT3 rot;
	DirectX::XMStoreFloat3(&rot, m_rotation);

	rot.x += x;
	rot.y += y;
	rot.z += z;

	m_rotation = DirectX::XMLoadFloat3(&rot);
	UpdateMatrix();
	UpdateDirectionVectors();
}

void GameObject::SetScale(float sx, float sy, float sz)
{
	m_scale = { sx, sy, sz };

	UpdateMatrix();
}

void GameObject::UpdateDirectionVectors()
{
	DirectX::XMFLOAT3 rotf3;
	DirectX::XMStoreFloat3(&rotf3, m_rotation);

	DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(rotf3.x, rotf3.y, 0.f);
	m_forward = DirectX::XMVector3TransformCoord(DEFAULT_FORWARD, rotation);
	m_back = DirectX::XMVector3TransformCoord(DEFAULT_BACK, rotation);
	m_left = DirectX::XMVector3TransformCoord(DEFAULT_LEFT, rotation);
	m_right = DirectX::XMVector3TransformCoord(DEFAULT_RIGHT, rotation);
}
