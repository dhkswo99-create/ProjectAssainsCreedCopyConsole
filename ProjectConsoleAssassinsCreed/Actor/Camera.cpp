#include "Camera.h"
#include <Render/Renderer.h>
#include <Level/GameLevel.h>

using namespace Craft;

Camera::Camera()
	: super(L"C", (0, 0), Color::White)
{
	sortingOrder = -1;
} //위치는 안바꿀 것임.

void Camera::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// 카메라가 보여줄 위치
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
	cameraView = level->GetPlayerPosition();  //TODO
	playerFace = level->GetPlayerFace();  //TODO
	// 기본 카메라 조정
	cameraView.x -= 45;
	cameraView.y -= 15;

	// 플레이어가 바라보는 방향에 맞추어 시야 조정
	int faceCheck = playerFace.x * 3 + playerFace.y;
	switch (faceCheck)
	{
	case 1: //하단 ( x = 0 y = 1 ) 
		cameraView.y += 8;
		break;
	case 2: //우상단 ( x =1 y = -1 )
		cameraView.x += 5;
		cameraView.y -= 5;
		break;
	case 3: //우 ( x = 1 y = 0 )
		cameraView.x += 8;
		break;
	case 4: //우하단 ( x = 1 y = 1 )
		cameraView.x += 5;
		cameraView.y += 5;
		break;
	case -1: //상단
		cameraView.y -= 8;
		break;
	case -2: //좌하단
		cameraView.x -= 5;
		cameraView.y += 5;
		break;
	case -3: //좌
		cameraView.x -= 8;
		break;
	case -4: //좌상단
		cameraView.x -= 5;
		cameraView.y -= 5;
		break;
	}

	Renderer::Get().SetCameraView(cameraView);  // 이건 렌더가 알아야 함..
	
}
