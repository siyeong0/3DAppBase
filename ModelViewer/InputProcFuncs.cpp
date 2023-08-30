#include "InputProcFuncs.h"
#include "ModelViewer.h"
#include <Windows.h>

static bool bFPV = false;
static POINT lockBeginPos;

void InputProcFuncs::SetFirstPersonView()
{
	ShowCursor(bFPV);
	bFPV = !bFPV;

	if (bFPV == true)
	{
		GetCursorPos(&lockBeginPos);
	}
	else
	{
		SetCursorPos(lockBeginPos.x, lockBeginPos.y);
	}
}

void InputProcFuncs::Terminate()
{
	GetModelViewer()->ExitLoop();
}

void InputProcFuncs::MoveForward(float dt)
{
	if (bFPV) GetModelViewer()->GetCamera().Move(Camera::eDirection::Forward, dt);
};
void InputProcFuncs::MoveBackward(float dt)
{
	if (bFPV) GetModelViewer()->GetCamera().Move(Camera::eDirection::Backward, dt);
};
void InputProcFuncs::MoveLeft(float dt)
{
	if (bFPV) GetModelViewer()->GetCamera().Move(Camera::eDirection::Left, dt);
};
void InputProcFuncs::MoveRight(float dt)
{
	if (bFPV) GetModelViewer()->GetCamera().Move(Camera::eDirection::Right, dt);
};
void InputProcFuncs::MoveUp(float dt)
{
	if (bFPV) GetModelViewer()->GetCamera().Move(Camera::eDirection::Up, dt);
}
void InputProcFuncs::MoveDown(float dt)
{
	if (bFPV) GetModelViewer()->GetCamera().Move(Camera::eDirection::Down, dt);
}

void InputProcFuncs::MoveView(float dt)
{
	if (bFPV)
	{
		GetModelViewer()->GetCamera().OnMouseMove(GetRenderer()->mCursorNdcX, GetRenderer()->mCursorNdcY);

		POINT Center{ int(GetRenderer()->mViewportTopLeftX + GetRenderer()->mViewportWidth / 2), int(GetRenderer()->mViewportTopLeftY + GetRenderer()->mViewportHeight / 2)};
		ClientToScreen(GetModelViewer()->GetWindow(), &Center);
		SetCursorPos(Center.x, Center.y);
	}
}

static bool bDrag = false;
static float prevRatio = 0.0f;
static Vector3 prevPos(0.0f);
static Vector3 prevVector(0.0f);

void InputProcFuncs::DragBegin()
{
	bDrag = true;
}
void InputProcFuncs::DragObject(float dt)
{
	Quaternion q = Quaternion::CreateFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), 0.0f);

	Vector3 cursorNdcNear = Vector3(GetRenderer()->mCursorNdcX, GetRenderer()->mCursorNdcY, 0.0f);
	Vector3 cursorNdcFar = Vector3(GetRenderer()->mCursorNdcX, GetRenderer()->mCursorNdcY, 1.0f);
	Matrix inverseProjView = (GetModelViewer()->GetCamera().ViewMatrix() * GetModelViewer()->GetCamera().ProjMatrix()).Invert();
	Vector3 cursorWorldNear = Vector3::Transform(cursorNdcNear, inverseProjView);
	Vector3 cursorWorldFar = Vector3::Transform(cursorNdcFar, inverseProjView);
	Vector3 dir = cursorWorldFar - cursorWorldNear;
	dir.Normalize();

	DirectX::SimpleMath::Ray curRay = DirectX::SimpleMath::Ray(cursorWorldNear, dir);
	float dist = 0.0f;
	Vector3 center = GetModelViewer()->GetModel().TransformMatrix.Translation();
	bool bSelected = curRay.Intersects(BoundingBox(center, GetRenderer()->mMeshTable[GetModelViewer()->GetModel().MeshName]->mExtents), dist);

	if (bSelected)
	{
		Vector3 pickPoint = cursorWorldNear + dist * dir;

		if (bDrag)
		{
			bDrag = false;
			prevVector = pickPoint - center;
			prevVector.Normalize();
		}
		else
		{
			Vector3 currentVector = pickPoint - center;
			currentVector.Normalize();
			float theta = acos(prevVector.Dot(currentVector));
			if (theta > 3.141592f / 180.0f * 3.0f)
			{
				Vector3 axis = prevVector.Cross(currentVector);
				axis.Normalize();
				q = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(axis, theta);
				prevVector = currentVector;
			}
		}

		Vector3 translation = GetModelViewer()->GetModel().TransformMatrix.Translation();
		GetModelViewer()->GetModel().TransformMatrix.Translation(Vector3(0.0f));
		GetModelViewer()->GetModel().TransformMatrix = GetModelViewer()->GetModel().TransformMatrix * Matrix::CreateFromQuaternion(q) * Matrix::CreateTranslation(translation);
	}
}
void InputProcFuncs::DragEnd()
{
	bDrag = false;
}

void InputProcFuncs::RotateBegin()
{
		bDrag = true;
}
void InputProcFuncs::RotateObject(float dt)
{
	Vector3 dragTranslation(0.0f);

	Vector3 cursorNdcNear = Vector3(GetRenderer()->mCursorNdcX, GetRenderer()->mCursorNdcY, 0.0f);
	Vector3 cursorNdcFar = Vector3(GetRenderer()->mCursorNdcX, GetRenderer()->mCursorNdcY, 1.0f);
	Matrix inverseProjView = (GetModelViewer()->GetCamera().ViewMatrix() * GetModelViewer()->GetCamera().ProjMatrix()).Invert();
	Vector3 cursorWorldNear = Vector3::Transform(cursorNdcNear, inverseProjView);
	Vector3 cursorWorldFar = Vector3::Transform(cursorNdcFar, inverseProjView);
	Vector3 dir = cursorWorldFar - cursorWorldNear;
	dir.Normalize();

	DirectX::SimpleMath::Ray curRay = DirectX::SimpleMath::Ray(cursorWorldNear, dir);
	float dist = 0.0f;
	Vector3 center = GetModelViewer()->GetModel().TransformMatrix.Translation();
	bool bSelected = curRay.Intersects(BoundingBox(center, GetRenderer()->mMeshTable[GetModelViewer()->GetModel().MeshName]->mExtents), dist);

	if (bSelected)
	{
		Vector3 pickPoint = cursorWorldNear + dist * dir;
		if (bDrag)
		{
			bDrag = false;
			prevRatio = dist / (cursorWorldFar - cursorWorldNear).Length();
			prevPos = pickPoint;
		}
		else
		{
			Vector3 newPos = cursorWorldNear + prevRatio * (cursorWorldFar - cursorWorldNear);
			if ((newPos - prevPos).Length() > 1e-3)
			{
				dragTranslation = newPos - prevPos;
				prevPos = newPos;
			}
		}

		Vector3 translation = GetModelViewer()->GetModel().TransformMatrix.Translation();
		GetModelViewer()->GetModel().TransformMatrix.Translation(Vector3(0.0f));
		GetModelViewer()->GetModel().TransformMatrix = GetModelViewer()->GetModel().TransformMatrix * Matrix::CreateTranslation(dragTranslation + translation);
	}
}

void InputProcFuncs::RotateEnd()
{
	bDrag = false;
}
