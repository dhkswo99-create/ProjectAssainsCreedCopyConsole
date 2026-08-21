#pragma once

#include <Actor/ACTOR.H>

class Camera : public Craft::Actor
{
	TYPE_DECLARATIONS(Camera, Actor)
public:
	Camera();
	void SetCameraTargetView(const Craft::Vector2& position) { cameraTargetView = position; }
	void SetCameraView(const Craft::Vector2& position) { cameraView = position; }
	void SetMoveSpeed(float newMoveSpeed) { moveSpeed = newMoveSpeed; }
	void Move(float deltaTime);

private:
	virtual void Tick(float deltaTime) override;

private:
	float moveSpeed;
	float xCameraPosition;
	float yCameraPosition;
	float dx;
	float dy;
	// 카메라 위치 변수
	Craft::Vector2 cameraView = Craft::Vector2::Zero;
	Craft::Vector2 cameraTargetView = Craft::Vector2::Zero;

	// 플레이어 방향 변수
	Craft::Vector2 playerFace = Craft::Vector2::Zero;
};

