#pragma once

namespace InputProcFuncs
{
	void SetFirstPersonView();
	void Terminate();

	void MoveForward(float dt);
	void MoveBackward(float dt);
	void MoveLeft(float dt);
	void MoveRight(float dt);
	void MoveUp(float dt);
	void MoveDown(float dt);

	void MoveView(float dt);

	void DragBegin();
	void DragObject(float dt);
	void DragEnd();

	void RotateBegin();
	void RotateObject(float dt);
	void RotateEnd();
}	// InputProcFuncs