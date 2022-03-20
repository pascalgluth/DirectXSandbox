#pragma once

#include <DirectXMath.h>

class GameObject
{
public:
	virtual ~GameObject() = default;
	const DirectX::XMVECTOR& GetPosition();
	DirectX::XMFLOAT3 GetPositonF3();
	const DirectX::XMVECTOR& GetRotation();
	DirectX::XMFLOAT3 GetRotationF3();

	void SetPosition(const DirectX::XMVECTOR& pos);
	void SetPosition(const DirectX::XMFLOAT3& pos);
	void AddOffset(float ox, float oy, float oz);
	void AddOffset(const DirectX::XMFLOAT3& offset);
	void SetRotation(const DirectX::XMVECTOR& rot);
	void SetRotation(const DirectX::XMFLOAT3& rot);
	void AddRotationOffset(float x, float y, float z);
	void SetScale(float sx, float sy, float sz);

protected:
	virtual void UpdateMatrix() = 0;
	void UpdateDirectionVectors();

	const DirectX::XMVECTOR DEFAULT_FORWARD = DirectX::XMVectorSet(0.f, 0.f, 1.f, 0.f);
	const DirectX::XMVECTOR DEFAULT_UP = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_BACK = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_LEFT = DirectX::XMVectorSet(-1.f, 0.0f, 0.f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_RIGHT = DirectX::XMVectorSet(1.f, 0.0f, 0.f, 0.0f);

	DirectX::XMVECTOR m_forward;
	DirectX::XMVECTOR m_left;
	DirectX::XMVECTOR m_right;
	DirectX::XMVECTOR m_back;

	DirectX::XMVECTOR m_position = DirectX::XMVectorSet(0.f, 0.f, 0.f, 0.f);
	DirectX::XMVECTOR m_rotation = DirectX::XMVectorSet(0.f, 0.f, 0.f, 0.f);
	DirectX::XMFLOAT3 m_scale = DirectX::XMFLOAT3(1.f, 1.f, 1.f);

};
