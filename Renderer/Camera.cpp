#include "pch.h"
#include "Camera.h"

#include <algorithm>

using namespace DirectX;
using SimpleMath::Quaternion;

Camera::Camera(Vector3 pos, Vector3 up, float yaw, float pitch)
{
	mPosition = pos;
	mUpDir = up;
	mYaw = yaw;
	mPitch = pitch;
	updateDirections();
}

void Camera::Update(float dt)
{
	BoundingFrustum::CreateFromMatrix(mViewFrustum, ProjMatrix());
	mViewFrustum.Origin = mPosition;
	mViewFrustum.Orientation = Quaternion::CreateFromYawPitchRoll(mYaw, mPitch, 0.0f);
}

bool Camera::IsVisible(const BoundingBox& boundingBox)
{
	return mViewFrustum.Intersects(boundingBox);
}

Matrix Camera::ProjMatrix() const
{
	return XMMatrixPerspectiveFovLH(XMConvertToRadians(mFovY), mAspectRatio, mNearZ, mFarZ);
}

Matrix Camera::ViewMatrix() const
{
	return XMMatrixLookToLH(mPosition, mFrontDir, mUpDir);
}

const Vector3& Camera::GetPosition() const { return mPosition; }

float Camera::GetYaw() const { return mYaw; }
float Camera::GetPitch() const { return mPitch; }
float Camera::GetSpeed() const { return mSpeed; }
float Camera::GetMouseSensitivity() const { return mMouseSensitivity; }
float Camera::GetFovY() const { return mFovY; }
float Camera::GetAspectRatio() const { return mAspectRatio; }
float Camera::GetNearZ() const { return mNearZ; }
float Camera::GetFarZ() const { return mFarZ; }

void Camera::SetPosition(Vector3&& position) { mPosition = std::move(position); }
void Camera::SetSpeed(float value) { mSpeed = value; }
void Camera::SetMouseSensitivity(float value) { mMouseSensitivity = value; }
void Camera::SetFovY(float value) { mFovY = value; }
void Camera::SetAspectRatio(float value) { mAspectRatio = value; }
void Camera::SetNearZ(float value) { mNearZ = value; }
void Camera::SetFarZ(float value) { mFarZ = value; }

void Camera::Move(eDirection dir, float delta)
{
	Vector3 dirVec = { 0,0,0 };
	switch (dir)
	{
	case eDirection::Forward:
		dirVec = mFrontDir;
		dirVec.y = 0.0f;
		break;
	case eDirection::Backward:
		dirVec = -mFrontDir;
		dirVec.y = 0.0f;
		break;
	case eDirection::Left:
		dirVec = -mRightDir;
		break;
	case eDirection::Right:
		dirVec = mRightDir;
		break;
	case eDirection::Up:
		dirVec = mWorldUp;
		break;
	case eDirection::Down:
		dirVec -= mWorldUp;
		break;
	default:
		assert(false);
		break;
	}
	mPosition += dirVec * (mSpeed * delta);
}

void Camera::OnMouseMove(float mouseNdcX, float mouseNdcY, bool constrainPitch)
{
	mYaw += mouseNdcX * mMouseSensitivity;
	mPitch += -mouseNdcY * mMouseSensitivity;
	if (constrainPitch)
	{
		constexpr float MaxPitch{ 3.141592f / 2.0f };
		mPitch = std::clamp(mPitch, -MaxPitch, MaxPitch);
	}
	updateDirections();
}

void Camera::OnMouseScroll(float yOffset)
{

}

void Camera::updateDirections()
{
	Vector3 front = Vector3::Transform(Vector3(0, 0, 1), Matrix::CreateFromYawPitchRoll({ mPitch, mYaw, 0 }));
	front.Normalize();
	mFrontDir = front;

	Vector3 right = mWorldUp.Cross(mFrontDir);
	right.Normalize();
	mRightDir = right;

	Vector3 up = mFrontDir.Cross(mRightDir);
	up.Normalize();
	mUpDir = up;
}
