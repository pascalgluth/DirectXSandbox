#pragma once

#include <DirectXMath.h>

class GameObject
{
public:
	virtual ~GameObject(){};
	const DirectX::XMVECTOR& GetPosition();
	DirectX::XMFLOAT3 GetPositonF3();
	const DirectX::XMVECTOR& GetRotation();
	DirectX::XMFLOAT3 GetRotationF3();
	DirectX::XMFLOAT3 GetUpVectorF3();
	DirectX::XMFLOAT3 GetScaleF3() { return m_scale; }

	virtual void SetPosition(const DirectX::XMVECTOR& pos);
	virtual void SetPosition(const DirectX::XMFLOAT3& pos);
	virtual void AddOffset(float ox, float oy, float oz);
	virtual void AddOffset(const DirectX::XMFLOAT3& offset);
	virtual void SetRotation(const DirectX::XMVECTOR& rot);
	virtual void SetRotation(const DirectX::XMFLOAT3& rot);
	virtual void AddRotationOffset(float x, float y, float z);
	virtual void SetScale(float sx, float sy, float sz);

protected:
	virtual void UpdateMatrix() {};
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
	DirectX::XMVECTOR m_up;

	DirectX::XMVECTOR m_position = DirectX::XMVectorSet(0.f, 0.f, 0.f, 0.f);
	DirectX::XMVECTOR m_rotation = DirectX::XMVectorSet(0.f, 0.f, 0.f, 0.f);
	DirectX::XMFLOAT3 m_scale = DirectX::XMFLOAT3(1.f, 1.f, 1.f);

};
