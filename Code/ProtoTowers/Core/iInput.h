// Author : Jack Gillette
//
// An Interface class for anything that should recieve input events from a PlayerController.
#pragma once

class iInput
{
public:
	// === Interface === //
//	virtual void MoveForward(float _axisValue) = 0;
//	virtual void MoveRight(float _axisValue) = 0;
	virtual void Shoot(int shotType) = 0;
	// ================= //
};