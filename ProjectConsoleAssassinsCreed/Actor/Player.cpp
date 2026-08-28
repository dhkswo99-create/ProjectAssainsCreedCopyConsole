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
	isSighted = true;
	face = Vector2::Right;
	moveSpeed = 10.0f;
	sortingOrder = 14;
	xPosition = position.x;
	yPosition = position.y;
	//충돌 허용
	SetColiisionEnabled(true);
}



void Player::Tick(float deltaTime)
{
	//상위 객체 tick 호출
	super::Tick(deltaTime);

	MiniMapSubmit();

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
		buff.SetTargetTime(buffDuration);
		buff.Reset();
		moveSpeed = 50.0f;
	}
	if (buff.IsTimeOut())
	{
		moveSpeed = 10.0f;
	}

	if (Input::Get().GetKeyDown(VK_SPACE))
	{
		doAttack = true;
	}
	// 선딜레이
	if (doAttack)
	{
		delay.SetTargetTime(castDelay);
		if (delay.IsTimeOut())
		{
			CalcSwordRoute(GetFace(), GetPosition());
  			Attack(range, face, deltaTime);
			swordNearRoute.clear();
			swordMiddleRoute.clear();
			swordFarRoute.clear();
			doneAttack = false;
			doAttack = false;
			delay.Reset();
		}
	}

	//후딜레이
	if (!doneAttack)
	{
		delay.SetTargetTime(attackDelay);
		if (delay.IsTimeOut())
		{
			delay.SetTargetTime(0);
			doneAttack = true;
		}
	}

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
{
	Vector2 a = Vector2( // 대각선을 바라보고 45도 계산이라면
		(face.x * face.y != 0 && matrix0 == 1) ?
		Vector2((face.x * matrix0 + face.y * matrix2) / 2,
			(face.x * matrix1 + face.y * matrix3) / 2)
		: Vector2(face.x * matrix0 + face.y * matrix2,
			face.x * matrix1 + face.y * matrix3));
	return a;
}
bool Player::CalcSwordRoute(const Vector2& face, const Vector2& position)
{
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
	{
		if (!level->CanAttack(position, negative90face)) { return false; }
		swordNearRoute.emplace_back(position + negative90face); //1-1
		if (!level->CanAttack(position + negative90face, negative90face)) { return false; }
		swordMiddleRoute.emplace_back(position + negative90face + negative90face); //1-2
		if (!level->CanAttack(position + negative90face + negative90face, negative90face)) { return false; }
		swordFarRoute.emplace_back(position + negative90face + negative90face + negative90face); //1-3

		if (!level->CanAttack(position, negative90face)) { return false; }
		swordNearRoute.emplace_back(position + negative90face); //2-1
		if (!level->CanAttack(position + negative90face, negative90face)) { return false; }
		swordMiddleRoute.emplace_back(position + negative90face + negative45face); //2-2
		if (!level->CanAttack(position + negative90face + negative90face, negative45face)) { return false; }
		swordFarRoute.emplace_back(position + negative90face + negative90face + negative45face); //2-3

		if (!level->CanAttack(position, negative45face)) { return false; }
		swordNearRoute.emplace_back(position + negative45face); //3-1
		if (!level->CanAttack(position + negative45face, negative45face)) { return false; }
		swordMiddleRoute.emplace_back(position + negative45face + negative45face); //3-2
		if (!level->CanAttack(position + negative45face, negative45face)) { return false; }
		swordFarRoute.emplace_back(position + negative45face + negative45face); //3-3

		if (!level->CanAttack(position, face)) { return false; }
		swordNearRoute.emplace_back(position + face); //4-1
		if (!level->CanAttack(position + negative45face, face)) { return false; }
		swordMiddleRoute.emplace_back(position + negative45face + face); //4-2
		if (!level->CanAttack(position + negative45face, negative45face)) { return false; }
		swordFarRoute.emplace_back(position + negative45face + negative45face); //4-3

		if (!level->CanAttack(position, face)) { return false; }
		swordNearRoute.emplace_back(position + face); //5-1
		if (!level->CanAttack(position + face, face)) { return false; }
		swordMiddleRoute.emplace_back(position + face + face); //5-2
		if (!level->CanAttack(position + face + face, face)) { return false; }
		swordFarRoute.emplace_back(position + face + face + face); //5-3

		if (!level->CanAttack(position, positive45face)) { return false; }
		swordNearRoute.emplace_back(position + positive45face); //6-1
		if (!level->CanAttack(position + face, positive45face)) { return false; }
		swordMiddleRoute.emplace_back(position + face + positive45face); //6-2
		if (!level->CanAttack(position + face, positive45face)) { return false; }
		swordFarRoute.emplace_back(position + face + positive45face); //6-3

		if (!level->CanAttack(position, positive45face)) { return false; }
		swordNearRoute.emplace_back(position + positive45face); //7 -1
		if (!level->CanAttack(position + positive45face, positive90face)) { return false; }
		swordMiddleRoute.emplace_back(position + positive45face + positive90face); //7-2
		if (!level->CanAttack(position + positive45face, positive45face)) { return false; }
		swordFarRoute.emplace_back(position + positive45face + positive45face); //7-3
	}
	else
	{
		if (!level->CanAttack(position, negative90face)) { return false; }
		swordNearRoute.emplace_back(position + negative90face); //1-1
		if (!level->CanAttack(position + negative90face, negative90face)) { return false; }
		swordMiddleRoute.emplace_back(position + negative90face + negative90face); //1-2
		if (!level->CanAttack(position + negative90face, negative90face)) { return false; }
		swordFarRoute.emplace_back(position + negative90face + negative90face); //1-3

		if (!level->CanAttack(position, negative45face)) { return false; }
		swordNearRoute.emplace_back(position + negative45face); //2-1
		if (!level->CanAttack(position + negative45face, negative45face)) { return false; }
		swordMiddleRoute.emplace_back(position + negative45face + negative45face); //2-2
		if (!level->CanAttack(position + negative90face , negative45face)) { return false; }
		swordFarRoute.emplace_back(position + negative90face + negative45face); //2-3

		if (!level->CanAttack(position, face)) { return false; }
		swordNearRoute.emplace_back(position + face); //3-1
		if (!level->CanAttack(position + negative45face, face)) { return false; }
		swordMiddleRoute.emplace_back(position + negative45face + face); //3-2
		if (!level->CanAttack(position + negative45face + negative45face, face)) { return false; }
		swordFarRoute.emplace_back(position + negative45face + negative45face + face); //3-3

		if (!level->CanAttack(position, face)) { return false; }
		swordNearRoute.emplace_back(position + face); //4-1
		if (!level->CanAttack(position + face, face)) { return false; }
		swordMiddleRoute.emplace_back(position + face + face); //4-2
		if (!level->CanAttack(position + negative45face+ face, face)) { return false; }
		swordFarRoute.emplace_back(position + negative45face + face + face); //4-3

		if (!level->CanAttack(position, face)) { return false; }
		swordNearRoute.emplace_back(position + face); //5-1
		if (!level->CanAttack(position + face, positive45face)) { return false; }
		swordMiddleRoute.emplace_back(position + face + positive45face); //5-2
		if (!level->CanAttack(position + face + face, face)) { return false; }
		swordFarRoute.emplace_back(position + face + face + face); //5-3

		if (!level->CanAttack(position, positive45face)) { return false; }
		swordNearRoute.emplace_back(position + positive45face); //6-1
		if (!level->CanAttack(position + positive45face, positive45face)) { return false; }
		swordMiddleRoute.emplace_back(position + positive45face + positive45face); //6-2
		if (!level->CanAttack(position + face + face, positive45face)) { return false; }
		swordFarRoute.emplace_back(position + face + face + positive45face); //6-3

		if (!level->CanAttack(position, positive45face)) { return false; }
		swordNearRoute.emplace_back(position + positive45face); //7-1
		if (!level->CanAttack(position + positive45face, positive45face)) { return false; }
		swordMiddleRoute.emplace_back(position + positive45face + positive45face); //7-2
		if (!level->CanAttack(position + positive45face, positive45face)) { return false; }
		swordFarRoute.emplace_back(position + positive45face + positive45face); //7-3		
	}

	return true;
}
void Player::MiniMapSubmit()
{
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
	Renderer::Get().ScreenSubmit(
		L"P",
		Vector2( static_cast<int>(
			static_cast<float>(position.x) 
			/ (static_cast<float>(level->GetMap().size()) / 49)),
			1 + 	static_cast<int>(
			static_cast<float>(position.y)
			/ (static_cast<float>(level->GetMap().size()) / 20))
		), 
		Color::White,
		20,
		true
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

void Player::Attack(const int range, const Vector2& face, float deltaTime)
{
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
	std::shared_ptr<Level> owner = GetOwner();
	if (owner)
	{
		Vector2 	swordPath = position;
		owner->SpawnActor<Sword>(GetPosition(), GetSwordNearRoute());
		owner->SpawnActor<Sword>(GetPosition(), GetSwordMiddleRoute());
		owner->SpawnActor<Sword>(GetPosition(), GetSwordFarRoute());
	}
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
