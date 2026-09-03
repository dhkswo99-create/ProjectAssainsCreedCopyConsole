#include "Player.h"
#include <Input/Input.h>
#include <Level/GameLevel.h>
#include <Actor/Sword.h>
#include <Actor/Item/Item.h>
#include <Render/Renderer.h>
#include <Game/Game.h>

using namespace Craft;

Player::Player(const Vector2& position)
	: super(L"→", position, Color::Green)
{
	// 다른 객체들보다 높은 우선 순위를 둘 것.
	// Enemy 객체와 벽 객체와는 Collision 시 overlap 불가
	hp = 100;
	isSighted = true;
	face = Vector2::Right;
	moveSpeed = 10.0f;
	sortingOrder = 14;
	xPosition = static_cast<float>(position.x);
	yPosition = static_cast<float>(position.y);
	delay.SetTargetTime(0);
	invincibilityTimer.SetTargetTime(0.2f);
	//충돌 허용
	SetColiisionEnabled(true);

	//검 객체 수만큼 공간 확보
	for (int ix = 0; ix < range; ++ix)
	{
		swordRoute.emplace_back();
	}
}



void Player::Tick(float deltaTime)
{
	//상위 객체 tick 호출
 	super::Tick(deltaTime);

	MiniMapSubmit();
	DisplayCommand();

	//프레임 관련 문자열
	const int size = 256;
	char fpsString[size] = {};

	sprintf_s(
		fpsString,
		size,
		"dt: %f | fps: %.1f",
		deltaTime,
		(1.0f) / deltaTime
	);

	//콘솔 창 이름에 값 설정
	SetConsoleTitleA(fpsString);

	delay.Tick(deltaTime);
	buff.Tick(deltaTime);
	invincibilityTimer.Tick(deltaTime);

	if (invincibilityTimer.IsTimeOut())
	{
		if (this->color == Color::Red)
		{
			this->SetColor(Color::Green);
		}
	}

	//이동 오른쪽 1 | 왼쪽 -1
	float directionX = 0.0f;
	float directionY = 0.0f;

	if (Input::Get().GetKey(VK_RIGHT))
	{
		directionX = 1.0f;
	}
	if (Input::Get().GetKey(VK_LEFT))
	{
		directionX = -1.0f;
	}
	if (Input::Get().GetKey(VK_DOWN))
	{
		directionY = 1.0f;
	}
	if (Input::Get().GetKey(VK_UP))
	{
		directionY = -1.0f;
	}


	if (Input::Get().GetKeyDown(VK_CONTROL))
	{
		//부스터 시간
		buff.SetTargetTime(buffDuration);
		buff.Reset();
		//부스터 계수
		moveSpeed = 50.0f;
	}
	if (buff.IsTimeOut())
	{
		moveSpeed = 10.0f;
	}

	if (Input::Get().GetKeyDown(VK_SPACE) && bDoSecondAttack)
	{
		bDoThirdAttack = true;
	}

	if (Input::Get().GetKeyDown(VK_SPACE) && doAttack)
	{
		bDoSecondAttack = true;
	}
	if (!delay.IsTimeOut())
	{
		return;
	}
	if (Input::Get().GetKeyDown(VK_SPACE) && !doAttack)
	{
		//공격
		doAttack = true;
		delay.Reset();
	}

	// 선딜레이
	if (doAttack)
	{
		//선딜 세팅
		delay.SetTargetTime(castDelay);
		if (delay.IsTimeOut())
		{
			Renderer::Get().Submit(
				L"⒂",
				GetPosition(),
				Color::Gray,
				0,
				true
			);
			Renderer::Get().Submit(
				L"⒂",
				GetPosition() + face,
				Color::Gray,
				0,
				true
			);
			std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
			// 공격 전 이동
			if (level->CanMove(GetPosition() + face))
			{
				SetPosition(GetPosition() + face);
				xPosition = static_cast<float>(position.x);
				yPosition = static_cast<float>(position.y);
			}
			// 검 루트 생성
			CalcSwordRoute(GetFace(), GetPosition());
			// 루트를 따라 공격
  			Attack(face, deltaTime);
			
			// 루트 초기화
			for (std::vector<Vector2>& route : swordRoute)
			{
				route.clear();
			}
			// 검 소유권 제거
			swordSet.clear();
			// 공격 중
			if (!bDoSecondAttack)
			{
				doneAttack = false;
			}
			// 1차 공격 시도 끝
			doAttack = false;
			// 딜레이 리셋
			delay.Reset();
		}
	}
	
	if (bDoSecondAttack && !doAttack)
	{
		//선딜 세팅
		delay.SetTargetTime(castDelay);
		if (delay.IsTimeOut())
		{
			// 검 루트 생성
  			CalcSecondSwordRoute(GetFace(), GetPosition());
			// 루트를 따라 공격
			Attack(face, deltaTime);
			// 공격 중 이동
			std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
			Vector2 positive90face = CalMatrix(face, positive90Degree[0], positive90Degree[1],
				positive90Degree[2], positive90Degree[3]);
			if (level->CanMove(GetPosition() + positive90face))
			{
				Renderer::Get().Submit(
					L"⒂",
					GetPosition(),
					Color::Gray,
					0,
					true
				);
				Renderer::Get().Submit(
					L"⒂",
					GetPosition() + positive90face,
					Color::Gray,
					0,
					true
				);
				SetPosition(GetPosition() + positive90face);
				xPosition = static_cast<float>(position.x);
				yPosition = static_cast<float>(position.y);
			}

			// 루트 초기화
			for (std::vector<Vector2>& route : swordRoute)
			{
				route.clear();
			}
			// 검 소유권 제거
			swordSet.clear();
			// 공격 중
			if (!bDoThirdAttack)
			{
				doneAttack = false;
			}
			// 1차 공격 시도 끝
			bDoSecondAttack = false;
			// 딜레이 리셋
			delay.Reset();
		}
	}
	if (bDoThirdAttack &&!bDoSecondAttack && !doAttack)
	{
		//선딜 세팅
		delay.SetTargetTime(castDelay);
		if (delay.IsTimeOut())
		{
			// 검 루트 생성
  			CalcThirdSwordRoute(GetFace(), GetPosition());
			// 루트를 따라 공격
			Attack(face, deltaTime);
			// 루트 초기화
			for (std::vector<Vector2>& route : swordRoute)
			{
				route.clear();
			}
			// 검 소유권 제거
			swordSet.clear();
			// 공격 중
			doneAttack = false;
			// 3차 공격 시도 끝
			bDoThirdAttack = false;
			// 딜레이 리셋
			delay.Reset();
		}
	}

	//후딜레이
	if (!doneAttack)
	{
		//후딜 세팅
		delay.SetTargetTime(attackDelay);
		if (delay.IsTimeOut())
		{
			//딜레이 초기화
			delay.SetTargetTime(0);
			// 공격 종료
			doneAttack = true;
		}
	}
	// 방향 설정 및 이동
	if (doneAttack && !doAttack)
	{
		if (Input::Get().GetKeyDown('D') || Input::Get().GetKeyDown('d'))
	{
		this->image = L"→";
		SetFace(Vector2(1,0));
		if (Input::Get().GetKey('w') || Input::Get().GetKey('W'))
		{
			this->image = L"↗";
			SetFace(Vector2(1, -1));
		}
		if (Input::Get().GetKey('s') || Input::Get().GetKey('S'))
		{
			this->image = L"↘";
			SetFace(Vector2(1, 1));
		}
		
	}
		if (Input::Get().GetKeyDown('A') || Input::Get().GetKeyDown('a'))
	{
		this->image = L"←";
		SetFace(Vector2(-1, 0));
		if (Input::Get().GetKey('w') || Input::Get().GetKey('W'))
		{
			this->image = L"↖";
			SetFace(Vector2(-1, -1));
		}
		if (Input::Get().GetKey('s') || Input::Get().GetKey('S'))
		{
			this->image = L"↙";
			SetFace(Vector2(-1, 1));
		}
	}

		if (Input::Get().GetKeyDown('W') || Input::Get().GetKeyDown('w'))
	{
		this->image = L"↑";
		SetFace(Vector2(0, -1));
		if (Input::Get().GetKey('d') || Input::Get().GetKey('D'))
		{
			this->image = L"↗";
			SetFace(Vector2(1, -1));
		}
		if (Input::Get().GetKey('a') || Input::Get().GetKey('A'))
		{
			this->image = L"↖";
			SetFace(Vector2(-1, -1));
		}
	}
		if (Input::Get().GetKeyDown('S') || Input::Get().GetKeyDown('s'))
	{
		this->image = L"↓";
		SetFace(Vector2(0, 1));
		if (Input::Get().GetKey('a') || Input::Get().GetKey('A'))
		{
			this->image = L"↙";
			SetFace(Vector2(-1, 1));
		}
		if (Input::Get().GetKey('d') || Input::Get().GetKey('D'))
		{
			this->image = L"↘";
			SetFace(Vector2(1, 1));
		}
	}

		Move(directionX, directionY, deltaTime);
		delay.Reset();
	}
}
Vector2 Player::CalMatrix(const Vector2& face, int matrix0, int matrix1, int matrix2, int matrix3)
{ // 바라보는 방향 기준으로 회전 계산 함수
	return Vector2( // 대각선을 바라보고 45도 계산이라면
		(face.x * face.y != 0 && matrix0 == 1) ?
		Vector2((face.x * matrix0 + face.y * matrix2) / 2,
			(face.x * matrix1 + face.y * matrix3) / 2)
		: Vector2(face.x * matrix0 + face.y * matrix2,
			face.x * matrix1 + face.y * matrix3));
}
bool Player::CalcSwordRoute(const Vector2& face, const Vector2& position)
{ // 바라보는 방향 기준으로 회전 계산
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());

	Vector2 negative90face = CalMatrix(face, negative90Degree[0], negative90Degree[1],
		negative90Degree[2], negative90Degree[3]);
	Vector2 negative45face = CalMatrix(face, negative45Degree[0], negative45Degree[1],
		negative45Degree[2], negative45Degree[3]);
	Vector2 positive45face = CalMatrix(face, positive45Degree[0], positive45Degree[1],
		positive45Degree[2], positive45Degree[3]);
	Vector2 positive90face = CalMatrix(face, positive90Degree[0], positive90Degree[1],
		positive90Degree[2], positive90Degree[3]);
	//검 궤적
	if (face.x * face.y == 0)
	{ // 12시 3시 6시 9시 방향을 바라볼 때

		if (!level->CanAttack(position, negative90face)) { return false; }
	 	swordRoute[0].emplace_back(position + negative90face); //1-1
		if (!level->CanAttack(position, negative90face * 2)) { return false; }
		swordRoute[1].emplace_back(position + negative90face * 2); //1-2
		if (!level->CanAttack(position, negative90face * 3)) { return false; }
		swordRoute[2].emplace_back(position + negative90face * 3); //1-3

		if (!level->CanAttack(position, negative90face)) { return false; }
		swordRoute[0].emplace_back(position + negative90face); //2-1
		if (!level->CanAttack(position, negative90face + negative45face)) { return false; }
		swordRoute[1].emplace_back(position + negative90face + negative45face); //2-2
		if (!level->CanAttack(position, negative90face * 2 + negative45face)) { return false; }
		swordRoute[2].emplace_back(position + negative90face * 2 + negative45face); //2-3

		if (!level->CanAttack(position, negative45face)) { return false; }
		swordRoute[0].emplace_back(position + negative45face); //3-1
		if (!level->CanAttack(position, negative45face * 2)) { return false; }
		swordRoute[1].emplace_back(position + negative45face * 2); //3-2
		if (!level->CanAttack(position, negative45face * 2)) { return false; }
		swordRoute[2].emplace_back(position + negative45face * 2); //3-3

		if (!level->CanAttack(position, face)) { return false; }
		swordRoute[0].emplace_back(position + face); //4-1
		if (!level->CanAttack(position, negative45face + face)) { return false; }
		swordRoute[1].emplace_back(position + negative45face + face); //4-2
		if (!level->CanAttack(position, negative45face * 2)) { return false; }
		swordRoute[2].emplace_back(position + negative45face * 2); //4-3

		if (!level->CanAttack(position, face)) { return false; }
		swordRoute[0].emplace_back(position + face); //5-1
		if (!level->CanAttack(position, face * 2)) { return false; }
		swordRoute[1].emplace_back(position + face * 2); //5-2
		if (!level->CanAttack(position, face * 3)) { return false; }
		swordRoute[2].emplace_back(position + face * 3); //5-3

		if (!level->CanAttack(position, positive45face)) { return false; }
		swordRoute[0].emplace_back(position + positive45face); //6-1
		if (!level->CanAttack(position, face + positive45face)) { return false; }
		swordRoute[1].emplace_back(position + face + positive45face); //6-2
		if (!level->CanAttack(position, face + positive45face)) { return false; }
		swordRoute[2].emplace_back(position + face + positive45face); //6-3

		if (!level->CanAttack(position, positive45face)) { return false; }
		swordRoute[0].emplace_back(position + positive45face); //7 -1
		if (!level->CanAttack(position, positive45face + positive90face)) { return false; }
		swordRoute[1].emplace_back(position + positive45face + positive90face); //7-2
		if (!level->CanAttack(position, positive45face * 2)) { return false; }
		swordRoute[2].emplace_back(position + positive45face * 2); //7-3
	} 
	else
	{ //대각선 방향을 바라볼 때
		if (!level->CanAttack(position, negative90face)) { return false; }
		swordRoute[0].emplace_back(position + negative90face); //1-1
		if (!level->CanAttack(position, negative90face * 2)) { return false; }
		swordRoute[1].emplace_back(position + negative90face * 2); //1-2
		if (!level->CanAttack(position, negative90face * 2)) { return false; }
		swordRoute[2].emplace_back(position + negative90face * 2); //1-3

		if (!level->CanAttack(position, negative45face)) { return false; }
		swordRoute[0].emplace_back(position + negative45face); //2-1
		if (!level->CanAttack(position, negative45face * 2)) { return false; }
		swordRoute[1].emplace_back(position + negative45face * 2); //2-2
		if (!level->CanAttack(position, negative90face + negative45face)) { return false; }
		swordRoute[2].emplace_back(position + negative90face + negative45face); //2-3

		if (!level->CanAttack(position, face)) { return false; }
		swordRoute[0].emplace_back(position + face); //3-1
		if (!level->CanAttack(position, negative45face + face)) { return false; }
		swordRoute[1].emplace_back(position + negative45face + face); //3-2
		if (!level->CanAttack(position, negative45face * 2 + face)) { return false; }
		swordRoute[2].emplace_back(position + negative45face * 2 + face); //3-3

		if (!level->CanAttack(position, face)) { return false; }
		swordRoute[0].emplace_back(position + face); //4-1
		if (!level->CanAttack(position, face * 2)) { return false; }
		swordRoute[1].emplace_back(position + face * 2); //4-2
		if (!level->CanAttack(position, negative45face + face * 2)) { return false; }
		swordRoute[2].emplace_back(position + negative45face + face * 2); //4-3

		if (!level->CanAttack(position, face)) { return false; }
		swordRoute[0].emplace_back(position + face); //5-1
		if (!level->CanAttack(position, face * 2)) { return false; }
		swordRoute[1].emplace_back(position + face * 2); //5-2
		if (!level->CanAttack(position, face * 3)) { return false; }
		swordRoute[2].emplace_back(position + face * 3); //5-3

		if (!level->CanAttack(position, positive45face)) { return false; }
		swordRoute[0].emplace_back(position + positive45face); //6-1
		if (!level->CanAttack(position, face + positive45face)) { return false; }
		swordRoute[1].emplace_back(position + face + positive45face); //6-2
		if (!level->CanAttack(position, face * 2 + positive45face)) { return false; }
		swordRoute[2].emplace_back(position + face * 2 + positive45face); //6-3

		if (!level->CanAttack(position, positive45face)) { return false; }
		swordRoute[0].emplace_back(position + positive45face); //7-1
		if (!level->CanAttack(position, positive45face * 2)) { return false; }
		swordRoute[1].emplace_back(position + positive45face * 2); //7-2
		if (!level->CanAttack(position, positive45face * 2)) { return false; }
		swordRoute[2].emplace_back(position + positive45face * 2); //7-3		
	}

 	return true;
}
bool Player::CalcSecondSwordRoute(const Vector2& face, const Vector2& position)
{ // 바라보는 방향 기준으로 회전 계산
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());

	Vector2 negative90face = CalMatrix(face, negative90Degree[0], negative90Degree[1],
		negative90Degree[2], negative90Degree[3]);
	Vector2 negative45face = CalMatrix(face, negative45Degree[0], negative45Degree[1],
		negative45Degree[2], negative45Degree[3]);
	Vector2 positive45face = CalMatrix(face, positive45Degree[0], positive45Degree[1],
		positive45Degree[2], positive45Degree[3]);
	Vector2 positive90face = CalMatrix(face, positive90Degree[0], positive90Degree[1],
		positive90Degree[2], positive90Degree[3]);
	//검 궤적
	if (face.x == 0 || face.y == 0)
	{ // 12시 3시 6시 9시 방향을 바라볼 때
		if (!level->CanAttack(position, positive90face )) { return false; }
		swordRoute[0].emplace_back(position + positive90face); //1-1
		if (!level->CanAttack(position, positive90face - face)) { return false; }
		swordRoute[1].emplace_back(position + positive90face - face); //1-2
		if (!level->CanAttack(position, positive90face - face)) { return false; }
		swordRoute[2].emplace_back(position + positive90face - face); //1-3

		if (!level->CanAttack(position, positive45face)) { return false; }
		swordRoute[0].emplace_back(position + positive45face); //2-1
		if (!level->CanAttack(position, positive90face * 2)) { return false; }
		swordRoute[1].emplace_back(position + positive90face * 2); //2-2
		if (!level->CanAttack(position, positive90face * 2)) { return false; }
		swordRoute[2].emplace_back(position + positive90face * 2); //2-3

		if (!level->CanAttack(position, positive45face)) { return false; }
		swordRoute[0].emplace_back(position + positive45face); //3-1
		if (!level->CanAttack(position, positive90face + positive45face)) { return false; }
		swordRoute[1].emplace_back(position + positive90face + positive45face); //3-2
		if (!level->CanAttack(position, positive45face + positive90face * 2)) { return false; }
		swordRoute[2].emplace_back(position + positive90face * 2 + positive45face); //3-3

		if (!level->CanAttack(position, positive45face)) { return false; }
		swordRoute[0].emplace_back(position + positive45face); //4-1
		if (!level->CanAttack(position, positive45face * 2)) { return false; }
		swordRoute[1].emplace_back(position + positive45face * 2); //4-2
		if (!level->CanAttack(position, positive45face * 2 + positive90face)) { return false; }
		swordRoute[2].emplace_back(position + positive45face * 2 + positive90face); //4-3

		if (!level->CanAttack(position, positive45face)) { return false; }
		swordRoute[0].emplace_back(position + positive45face); //5-1
		if (!level->CanAttack(position, positive45face)) { return false; }
		swordRoute[1].emplace_back(position + positive45face + face); //5-2
		if (!level->CanAttack(position, positive45face * 2 + face)) { return false; }
		swordRoute[2].emplace_back(position + positive45face * 2 + face); //5-3

		if (!level->CanAttack(position, positive45face)) { return false; }
		swordRoute[0].emplace_back(position + positive45face); //6-1
		if (!level->CanAttack(position, positive45face + face)) { return false; }
		swordRoute[1].emplace_back(position + positive45face + face); //6-2
		if (!level->CanAttack(position, positive45face + face * 2)) { return false; }
		swordRoute[2].emplace_back(position + positive45face + face * 2); //6-3

		if (!level->CanAttack(position, positive45face)) { return false; }
		swordRoute[0].emplace_back(position + positive45face); //7 -1
		if (!level->CanAttack(position, face * 2)) { return false; }
		swordRoute[1].emplace_back(position + face * 2); //7-2
		if (!level->CanAttack(position, face * 3)) { return false; }
		swordRoute[2].emplace_back(position + face * 3); //7-3
	}
	else
	{ //대각선 방향을 바라볼 때
		if (!level->CanAttack(position, positive45face)) { return false; }
		swordRoute[0].emplace_back(position + positive45face); //1-1
		if (!level->CanAttack(position, negative45face * -1)) { return false; }
		swordRoute[1].emplace_back(position + negative45face * -1); //1-2
		if (!level->CanAttack(position, negative45face * -1)) { return false; }
		swordRoute[2].emplace_back(position + negative45face * -1); //1-3

		if (!level->CanAttack(position, positive45face)) { return false; }
		swordRoute[0].emplace_back(position + positive45face); //2-1
		if (!level->CanAttack(position, positive90face)) { return false; }
		swordRoute[1].emplace_back(position + positive90face); //2-2
		if (!level->CanAttack(position, positive90face)) { return false; }
		swordRoute[2].emplace_back(position + positive90face); //2-3

		if (!level->CanAttack(position, positive45face)) { return false; }
		swordRoute[0].emplace_back(position + positive45face); //3-1
		if (!level->CanAttack(position, positive45face + positive90face)) { return false; }
		swordRoute[1].emplace_back(position + positive45face + positive90face); //3-2
		if (!level->CanAttack(position, positive90face * 2)) { return false; }
		swordRoute[2].emplace_back(position + positive90face * 2); //3-3

		if (!level->CanAttack(position, positive45face)) { return false; }
		swordRoute[0].emplace_back(position + positive45face); //4-1
		if (!level->CanAttack(position, positive45face * 2)) { return false; }
		swordRoute[1].emplace_back(position + positive45face * 2); //4-2
		if (!level->CanAttack(position, positive45face * 2 + positive90face)) { return false; }
		swordRoute[2].emplace_back(position + positive45face * 2 + positive90face); //4-3

		if (!level->CanAttack(position, positive45face)) { return false; }
		swordRoute[0].emplace_back(position + positive45face); //5-1
		if (!level->CanAttack(position, positive45face + face)) { return false; }
		swordRoute[1].emplace_back(position + positive45face + face); //5-2
		if (!level->CanAttack(position, positive45face * 2 + face)) { return false; }
		swordRoute[2].emplace_back(position + positive45face * 2 + face); //5-3


		if (!level->CanAttack(position, positive45face)) { return false; }
		swordRoute[0].emplace_back(position + positive45face); //6-1
		if (!level->CanAttack(position, positive45face + face)) { return false; }
		swordRoute[1].emplace_back(position + positive45face + face); //6-2
		if (!level->CanAttack(position, face * 2)) { return false; }
		swordRoute[2].emplace_back(position + face * 2); //6-3

		if (!level->CanAttack(position, positive45face)) { return false; }
		swordRoute[0].emplace_back(position + positive45face); //7-1
		if (!level->CanAttack(position, face)) { return false; }
		swordRoute[1].emplace_back(position + face); //7-2
		if (!level->CanAttack(position, face + negative45face)) { return false; }
		swordRoute[2].emplace_back(position + face + negative45face); //7-3		
	}

	return true;
}
bool Player::CalcThirdSwordRoute(const Craft::Vector2& face, const Craft::Vector2& position)
{
	// 바라보는 방향 기준으로 회전 계산
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());

	Vector2 negative90face = CalMatrix(face, negative90Degree[0], negative90Degree[1],
		negative90Degree[2], negative90Degree[3]);
	Vector2 negative45face = CalMatrix(face, negative45Degree[0], negative45Degree[1],
		negative45Degree[2], negative45Degree[3]);
	Vector2 positive45face = CalMatrix(face, positive45Degree[0], positive45Degree[1],
		positive45Degree[2], positive45Degree[3]);
	Vector2 positive90face = CalMatrix(face, positive90Degree[0], positive90Degree[1],
		positive90Degree[2], positive90Degree[3]);
	//검 궤적
	if (face.x == 0 || face.y == 0)
	{ // 12시 3시 6시 9시 방향을 바라볼 때
		if (!level->CanAttack(position, negative45face)) { return false; }
		swordRoute[0].emplace_back(position + negative45face); //1-1
		if (!level->CanAttack(position, negative45face * 2)) { return false; }
		swordRoute[1].emplace_back(position + negative45face * 2); //1-2
		if (!level->CanAttack(position, negative45face * 2)) { return false; }
		swordRoute[2].emplace_back(position + negative45face * 2); //1-3

		if (!level->CanAttack(position, face)) { return false; }
		swordRoute[0].emplace_back(position + face); //2-1
		if (!level->CanAttack(position, negative45face + face)) { return false; }
		swordRoute[1].emplace_back(position + negative45face + face); //2-2
		if (!level->CanAttack(position, negative45face * 2 + face)) { return false; }
		swordRoute[2].emplace_back(position + negative45face * 2 + face); //2-3

		if (!level->CanAttack(position, positive45face)) { return false; }
		swordRoute[0].emplace_back(position + positive45face); //3-1
		if (!level->CanAttack(position, face * 2)) { return false; }
		swordRoute[1].emplace_back(position + face * 2); //3-2
		if (!level->CanAttack(position, negative45face + face * 2)) { return false; }
		swordRoute[2].emplace_back(position + negative45face + face * 2); //3-3

		if (!level->CanAttack(position, positive45face)) { return false; }
		swordRoute[0].emplace_back(position + positive45face); //4-1
		if (!level->CanAttack(position, positive45face + face)) { return false; }
		swordRoute[1].emplace_back(position + positive45face + face); //4-2
		if (!level->CanAttack(position, face * 3)) { return false; }
		swordRoute[2].emplace_back(position + face * 3); //4-3

		if (!level->CanAttack(position, positive45face)) { return false; }
		swordRoute[0].emplace_back(position + positive45face); //5-1
		if (!level->CanAttack(position, positive45face + positive90face)) { return false; }
		swordRoute[1].emplace_back(position + positive45face + positive90face); //5-2
		if (!level->CanAttack(position, positive45face * 2)) { return false; }
		swordRoute[2].emplace_back(position + positive45face * 2); //5-3

		if (!level->CanAttack(position, positive90face)) { return false; }
		swordRoute[0].emplace_back(position + positive90face); //6-1
		if (!level->CanAttack(position, positive90face * 2)) { return false; }
		swordRoute[1].emplace_back(position + positive90face * 2); //6-2
	}
	else
	{ //대각선 방향을 바라볼 때
		if (!level->CanAttack(position, face)) { return false; }
		swordRoute[0].emplace_back(position + face); //1-1
		if (!level->CanAttack(position, negative45face + face)) { return false; }
		swordRoute[1].emplace_back(position + negative45face + face); //1-2
		if (!level->CanAttack(position, negative45face + face)) { return false; }
		swordRoute[2].emplace_back(position + negative45face + face); //1-3

		if (!level->CanAttack(position, face)) { return false; }
		swordRoute[0].emplace_back(position + face); //2-1
		if (!level->CanAttack(position, face * 2)) { return false; }
		swordRoute[1].emplace_back(position + face * 2); //2-2
		if (!level->CanAttack(position, face * 3)) { return false; }
		swordRoute[2].emplace_back(position + face * 3); //2-3

		if (!level->CanAttack(position, face)) { return false; }
		swordRoute[0].emplace_back(position + face); //3-1
		if (!level->CanAttack(position, face + positive45face)) { return false; }
		swordRoute[1].emplace_back(position + face + positive45face); //3-2
		if (!level->CanAttack(position, face * 2 + positive45face)) { return false; }
		swordRoute[2].emplace_back(position + face * 2 + positive45face); //3-3

		if (!level->CanAttack(position, positive45face)) { return false; }
		swordRoute[0].emplace_back(position + positive45face); //4-1
		if (!level->CanAttack(position, positive45face * 2)) { return false; }
		swordRoute[1].emplace_back(position + positive45face * 2); //4-2
		if (!level->CanAttack(position, face + positive45face * 2)) { return false; }
		swordRoute[2].emplace_back(position + face + positive45face * 2); //4-3

		if (!level->CanAttack(position, positive45face)) { return false; }
		swordRoute[0].emplace_back(position + positive45face); //5-1
		if (!level->CanAttack(position, positive45face * 2)) { return false; }
		swordRoute[1].emplace_back(position + positive45face * 2); //5-2
		if (!level->CanAttack(position, positive45face * 3)) { return false; }
		swordRoute[2].emplace_back(position + positive45face * 3); //5-3


		if (!level->CanAttack(position, positive45face)) { return false; }
		swordRoute[0].emplace_back(position + positive45face); //6-1
		if (!level->CanAttack(position, positive45face + positive90face)) { return false; }
		swordRoute[1].emplace_back(position + positive45face + positive90face); //6-2
	}


	return true;
}

void Player::MiniMapSubmit()
{
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
	Renderer::Get().ScreenSubmit( //플레이어 미니맵 처리
		L"P",
		Vector2( static_cast<int>(
			static_cast<float>(position.x) 
			/ (static_cast<float>(level->GetMap().size()) / 49)),
			1 + 	static_cast<int>(
			static_cast<float>(position.y)
			/ (static_cast<float>(level->GetMap().size()) / 20))
		), // offset 0, 1 -> 세부 조정
		Color::Green,
		20,
		true
	);
}
void Player::DisplayCommand()
{
	// 누르는 키 보여주기
	Renderer::Get().ScreenSubmit( L"Ctrl", Vector2(2, 47), 
		(Input::Get().GetKey(VK_CONTROL) ? Color::White : Color::Gray),
		21, true
	);
	Renderer::Get().ScreenSubmit( L"A", Vector2(9, 47), 
		((Input::Get().GetKey('a') || Input::Get().GetKey('A')) ? Color::White : Color::Gray),
		21, true
	);
	Renderer::Get().ScreenSubmit( L"W", Vector2(11, 45), 
		((Input::Get().GetKey('w') || Input::Get().GetKey('W')) ? Color::White : Color::Gray),
		21, true
	);
	Renderer::Get().ScreenSubmit( L"S", Vector2(11, 47), 
		((Input::Get().GetKey('s') || Input::Get().GetKey('S')) ? Color::White : Color::Gray),
		21, true
	);
	Renderer::Get().ScreenSubmit( L"D", Vector2(13, 47), 
		((Input::Get().GetKey('d') || Input::Get().GetKey('D')) ? Color::White : Color::Gray),
		21, true
	);
	Renderer::Get().ScreenSubmit( L"SpaceBar", Vector2(16, 47), 
		(Input::Get().GetKey(VK_SPACE) ? Color::White : Color::Gray),
		21, true
	);
	Renderer::Get().ScreenSubmit( L"←", Vector2(26, 47), 
		(Input::Get().GetKey(VK_LEFT) ? Color::White : Color::Gray),
		21, true
	);
	Renderer::Get().ScreenSubmit( L"↑", Vector2(28, 45), 
		(Input::Get().GetKey(VK_UP) ? Color::White : Color::Gray),
		21, true
	);
	Renderer::Get().ScreenSubmit( L"↓", Vector2(28, 47), 
		(Input::Get().GetKey(VK_DOWN) ? Color::White : Color::Gray),
		21, true
	);
	Renderer::Get().ScreenSubmit( L"→", Vector2(30, 47), 
		(Input::Get().GetKey(VK_RIGHT) ? Color::White : Color::Gray),
		21, true
	);
}
void Player::Move(float directionX, float directionY, float deltaTime)
{
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
	// x위치 업데이트
	// 이동 처리 -> 이동 방향과 빠르기를 적용해서 새로운 위치를 구하는 것.
	// 이동 방향(direction), 빠르기(moveSpeed), 시간(deltaTime)
	// 등속운동. 이동 거리 =  기존위치 + 이동방향*빠르기*시간;
	Vector2 currentPosition = GetPosition();
	dx += directionX * moveSpeed * deltaTime;
	dy += directionY * moveSpeed * deltaTime;
 	if (dx > 1)
	{
		++xPosition;
		dx = 0;
	}
	else if (-1 > dx)
	{
		--xPosition; 
		dx = 0;
	}
	if (dy > 1)
	{
		++yPosition;
		dy = 0;
	}
	else if (-1 > dy)
	{
		--yPosition;
		dy = 0;
	}
	
	//화면 왼쪽 벗어나지 않도록 처리
	if (xPosition < 1)
	{
		xPosition = 1.0f;
	}
	if (yPosition < 1)
	{
		yPosition = 1.0f;
	}
	//화면 오른쪽 벗어나지 않도록 처리
	if (xPosition + width >= level->GetMap().size() + 1)
	{
		xPosition = static_cast<float>((Engine::Get().GetGameWidth() - width));
	}
	if (yPosition + height >= level->GetMap().size() + 1)
	{
		yPosition = static_cast<float>((Engine::Get().GetGameHeight() - height));
	}

	//위치 업데이트
	Vector2 newPosition;
	// float -> int 형변환시 소숫점은 버림처리됨.
	newPosition.x = static_cast<int>(xPosition);
	newPosition.y = static_cast<int>(yPosition);
	if (level->CanMove(newPosition))
	{
		SetPosition(newPosition);
	}
	else
	{
		xPosition = static_cast<float>(currentPosition.x);
		yPosition = static_cast<float>(currentPosition.y); //초기화문제였다.
	}
}

void Player::Attack(const Vector2& face, float deltaTime)
{
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
	std::shared_ptr<Level> owner = GetOwner();
	if (owner)
	{ //공격 범위만큼 생성 -> 확장은 벡터2 배열의 배열로 경로를 받고 
		// - 검루트의 수만큼 for문 돌리면 됨.
		for (int ix = 0; ix < range; ++ix)
		{ 
			// 검 객체 생성 및 소유권 획득
			swordSet.emplace_back(owner->SpawnActor<Sword>(
				GetPosition(),
				GetSwordRoute(ix),
				weak_from_this(), // 검에게 생성 객체 접근 권한 부여
				playerDamage
			));
		}
	} 
}

void Player::DestroyWeapon()
{
	swordSet.clear();
	doAttack = false;
	bDoSecondAttack = false;
	bDoThirdAttack = false;
	doneAttack = true;
	delay.Reset();
}

void Player::BeAttacked(const Vector2& face, int damage)
{
	if (!invincibilityTimer.IsTimeOut())
	{
		return;
	}
	// 체력 감소
	this->hp -= damage;
	this->SetColor(Color::Red);
	// 넉백 
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
	if (level->CanMove(GetPosition() + face))
	{
		SetPosition(GetPosition() + face);
		xPosition = GetPosition().x + face.x;
		yPosition = GetPosition().y + face.y;
	}
	
	// 체력 0 이하
	if (this->hp <= 0)
	{
		// 플레이어가 휘두른 칼 삭제 -> 필요한가? 싶긴 함
		swordSet.clear();
		// 플레이어 소멸
		Destroy();
	}
	invincibilityTimer.Reset();
}

void Player::OnCollision(const std::shared_ptr<Actor>& other)
{
	if (Input::Get().GetKey('f')
		|| Input::Get().GetKey('F'))
	//if(true)
	{
		if (other->IsTypeOf<Clue>())
		{
			other->Destroy();
		}
	}
}
