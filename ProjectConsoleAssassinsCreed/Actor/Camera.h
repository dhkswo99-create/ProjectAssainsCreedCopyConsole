#pragma once

#include <Actor/ACTOR.H>

class Camera : public Craft::Actor
{
	TYPE_DECLARATIONS(Camera, Actor)
public:
	Camera();
	void SetCameraView(const Craft::Vector2& position) { cameraView = position; }

private:
	virtual void Tick(float deltaTime) override;

private:
	// 카메라 위치 변수
	Craft::Vector2 cameraView = Craft::Vector2::Zero;

	// 플레이어 방향 변수
	Craft::Vector2 playerFace = Craft::Vector2::Zero;
};

