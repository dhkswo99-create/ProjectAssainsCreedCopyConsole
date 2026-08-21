#include "Camera.h"
#include <Render/Renderer.h>
#include <Level/GameLevel.h>
#include <cmath>

using namespace Craft;

Camera::Camera()
	: super(L"C", (0, 0), Color::White)
{
	cameraView = Vector2::Zero;
	sortingOrder = -1;
} //위치는 안바꿀 것임.

void Camera::Move(float deltaTime)
{
	SetMoveSpeed(
		(cameraTargetView - cameraView) == Vector2::Zero ? 0 : 1
	);
	
	dx += (cameraTargetView.x - cameraView.x) * 5 * deltaTime * moveSpeed;
	dy += (cameraTargetView.y - cameraView.y) * 5 * deltaTime * moveSpeed;
	
	Vector2 currentPosition = cameraView;
	if (dx >= 1)
	{
		++currentPosition.x;
		dx = 0;
	}
	if (dx <= -1)
	{
		--currentPosition.x;
		dx = 0;
	}

	if (dy >= 1)
	{
		++currentPosition.y;
		dy = 0;
	}
	if (dy <= -1)
	{
		--currentPosition.y;
		dy = 0;
	
	}
	SetCameraView(currentPosition);

}

void Camera::Tick(float deltaTime)
{
	super::Tick(deltaTime);
	// 카메라가 보여줄 위치
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
	cameraTargetView = level->GetPlayerPosition();
	playerFace = level->GetPlayerFace();  //TODO
	// 기본 카메라 조정
	cameraTargetView.x -= 45;
	cameraTargetView.y -= 15;

	// 플레이어가 바라보는 방향에 맞추어 시야 조정
	int faceCheck = playerFace.x * 3 + playerFace.y;
	switch (faceCheck)
	{
	case 1: //하단 ( x = 0 y = 1 ) 
		cameraTargetView.y += 8;
		break;
	case 2: //우상단 ( x =1 y = -1 )
		cameraTargetView.x += 5;
		cameraTargetView.y -= 5;
		break;
	case 3: //우 ( x = 1 y = 0 )
		cameraTargetView.x += 8;
		break;
	case 4: //우하단 ( x = 1 y = 1 )
		cameraTargetView.x += 5;
		cameraTargetView.y += 5;
		break;
	case -1: //상단
		cameraTargetView.y -= 8;
		break;
	case -2: //좌하단
		cameraTargetView.x -= 5;
		cameraTargetView.y += 5;
		break;
	case -3: //좌
		cameraTargetView.x -= 8;
		break;
	case -4: //좌상단
		cameraTargetView.x -= 5;
		cameraTargetView.y -= 5;
		break;
	}
	Move(deltaTime);
	

	Renderer::Get().SetCameraView(cameraView);  // 이건 렌더가 알아야 함..
	
}
