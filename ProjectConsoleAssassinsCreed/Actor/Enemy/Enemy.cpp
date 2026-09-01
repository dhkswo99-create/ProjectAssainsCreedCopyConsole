#include "Enemy.h"
#include <Level/GameLevel.h>
#include <Actor/Enemy/Guard.h>
#include <Actor/Enemy/Archer.h>
#include <Util/Astar.h>
#include <Util/Bresenham.h>
#include <Render/Renderer.h>
#include <Input/Input.h>
#include <Util/Util.h>
#include <cmath>

#define ANGLE 180/3.14

using namespace Craft;

Enemy::Enemy(
	const std::wstring& image,
	const Vector2& position,
	Color color)
	:super(image, position, color)
{
	isSighted = true;
}

void Enemy::Tick(float deltaTime)
{
	super::Tick(deltaTime);



	found = Searching(); // << 얘가 병목 성능이 100 프레임에서 50프로임 아래로 떨어트림

	//생각보다 성능에 영향 안큼
	if (!found && moveIndex <= 0)
	{
		// 무작위 패트롤
		std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
		std::vector<std::vector<int>> map = level->GetMap();
		AStar patrolFinder;
		int ran = rand();
		int ran_1 = ran % 41;
		int ran_2 = ran % 79;
		int ran_3 = ran % 111;
		int ran_4 = ran % 1901;
		SetMoveSpeed(1.0f);
		int posX = GetPosition().x
			+ (ran_1 % 3 - 1) * (ran_2 % 5);
		int posY = GetPosition().y
			+ (ran_3 % 3 - 1) * (ran_4 % 5);
		if (GetPosition().x + posX >= level->GetMap().size()
			&& 0 >= GetPosition().x + posX)
		{
			posX = -posX;
		}
		if (GetPosition().y + posY >= level->GetMap().size()
			&& 0 >= GetPosition().y + posY)
		{
			posY = -posY;
		}
		patrolFinder.FindPath(GetPosition(), Vector2(posX, posY), map, pathDirection);
			
		if (pathDirection.size() > 1)
		{
			moveIndex = static_cast<int>(pathDirection.size()) - 1;
		}
	}

	


	// Enemy 방향에 맞게 조정
	int faceCheck = face.x * 3 + face.y;
	switch (faceCheck)
	{
	case 1: //하단 ( x = 0 y = 1 ) 
		this->image = L"↓";
		break;
	case 2: //우상단 ( x =1 y = -1 )
		this->image = L"↗";
		break;
	case 3: //우 ( x = 1 y = 0 )
		this->image = L"→";
		break;
	case 4: //우하단 ( x = 1 y = 1 )
		this->image = L"↘";
		break;
	case -1: //상단
		this->image = L"↑";
		break;
	case -2: //좌하단
		this->image = L"↙";
		break;
	case -3: //좌
		this->image = L"←";
		break;
	case -4: //좌상단
		this->image = L"↖";
		break;
	}
}
// 시야 범위 내에 Player가 5발각된다면 Calling 상태로 진입
// 이미 Call이 호출된 상태라면 Tracking 호출
//// 발각 위치를 갖고 간다.
//void Enemy::Calling(const Vector2& spotOfDetection)
//{
//	//TODO CallArea 객체 만들고 거기로 향하게 Move 찍는 함수.
//}
//
//// 모든 Enemy Awake, Move Call완료 시 caller = false;
//void Enemy::Call(const Vector2& spotOfDetection)
//{
//	//TODO CallArea 객체 만들고 충돌 처리? 하면 될듯?
//}

std::vector<Vector2> Enemy::FindRoute(const Vector2& destination)
{
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
	map = level->GetMap();
	Vector2 Start = GetPosition();
	AStar routeFinder;
	 //if (routeFinder.AstarFinder(map, Start, destination, moveStack) {}
	std::vector<Vector2> moveStack;
	routeFinder.FindPath(Start, destination, map, moveStack);
	if (level->GetDebuger())
	{
		Vector2 currentPos = Start;
		std::reverse(moveStack.begin(), moveStack.end());
		for (int ix = 0; ix < map[0].size(); ++ix)
		{
			for (int jx = 0; jx < map.size(); ++jx)
			{
				if (map[jx][ix] 
					== static_cast<int>(AStar::TileType::Visited))
				{
					Renderer::Get().Submit(
						L"⅓",
						Vector2(ix, jx),
						Color::Red, 
						1,
						true
					);
				}
			}
		}
		for (Vector2 path : moveStack)
		{
			Renderer::Get().Submit(
				L"⅓",
				currentPos = currentPos + path,
				Color::Green,
				1,
				true
			);
		}
	}
	moveIndex = static_cast<int>(moveStack.size() - 1);
	return moveStack;
}

//틱마다 호출되고 moveSpeed에 비례한 속도로 이동하며 한 칸마다 이동방향 갱신.
void Enemy::Move(const Vector2& direction, float deltaTime)
{
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
	Vector2 currentPosition = GetPosition();
	Vector2 newPosition;
	dx += direction.x * moveSpeed * deltaTime;
	dy += direction.y * moveSpeed * deltaTime;
	//이동 전 바라보는 방향 세팅
	SetFace(direction);
	if (dx > 1 || -1 > dx || dy > 1 || -1 > dy)
	{
		newPosition = currentPosition + direction;
		if (level->CanMove(newPosition))
		{
			SetPosition(newPosition);
			dx = 0;
			dy = 0;
			--moveIndex;
		}
		else
		{
			pathDirection.clear();
			moveIndex = 0;
		}
	}
}

void Enemy::Awake()
{
	sleep = true;
}

// 이 함수에서 Calling, Call이 호출
// sightDegree로 판별. 
bool Enemy::Searching()
{
	//GameLevel 객체 생성
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
	//Bresenham 알고리즘 사용을 위해 객체 생성
	Vector2 playerPos = level->GetPlayerPosition();
	Vector2 myPos = GetPosition();
	distance =static_cast<float>(
		(playerPos.x - myPos.x)
		* (playerPos.x - myPos.x)
		+ (playerPos.y - myPos.y)
		* (playerPos.y - myPos.y));
	if (distance > sightRange * sightRange) // 단순 비교
	{ // 적군이 많아질수록 성능에 유리
		return false;
	}
	std::vector<std::vector<int>> map = level->GetMap();
	Bresenham bresenham(map);
	Vector2 myFace = GetFace();
	float innerProduct = static_cast<float>(
		(playerPos.x - myPos.x)* myFace.x + (playerPos.y - myPos.y)* myFace.y
		);
		
	float absFace = static_cast<float>(std::sqrt(
		std::pow(myFace.x, 2)
		+ std::pow(myFace.y, 2)
	));
	distance = std::sqrt(distance);
	if (distance * absFace)
	{
		relativeAngle = acos(innerProduct / (distance * absFace)) * ANGLE;
	}
	else
	{
		relativeAngle = 0;
	}
	// 직선 경로
	std::vector<Vector2> rayDirectionQueue = bresenham.BresenhamFinder(distance, myPos, playerPos);

	if (rayDirection.size() != 0)
	{
		rayDirection.pop_back();
	}
	//std::vector<Vector2> rayDirectionQueue = RayDirectionQueueInsert(myPos);
	isWall = false;
	for (Vector2 path : rayDirectionQueue)
	{
		if (level->GetDebuger())
		{
			Renderer::Get().Submit(
				L"#",
				path,
				Color::White,
				1,
				true
			);
		}
		isWall = level->IsWall(path);
		if (isWall)
		{
			break;
		}
	}
	if (!isWall)
	{
		// 아주 가깝다면 깨어남
		if (sightRange / 4 > distance)
		{
			if (sleep) Awake();
			return true;
		}
		// 일정 거리 내에 있고 시야 각 내에 있다면 발견
		else if ((sightRange > distance)
			&& (sightDegree > relativeAngle
				&& relativeAngle > -1 * sightDegree)
			)
		{
			return true;
		}
	}
	return false;
}

//// 각도를 계산하여 바라보는 방향 반환
Vector2 Enemy::FacingDirection(const Vector2& currentPosition)
{
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
	Vector2 playerPos = level->GetPlayerPosition();
	float innerProduct = static_cast<float>(
		(playerPos.x - currentPosition.x) * rightVector.x
		+ (playerPos.y - currentPosition.y) * rightVector.y
		);
	float rayDistance = static_cast<float>(std::sqrt(
		std::pow(playerPos.x - currentPosition.x, 2)
		+ std::pow(playerPos.y - currentPosition.y, 2)
	));
	float absFace = static_cast<float>(std::sqrt(
		std::pow(rightVector.x, 2)
		+ std::pow(rightVector.y, 2)
	));
	if (rayDistance > 0)
	{
		facingAngle = acos(innerProduct / (rayDistance * absFace)) * ANGLE;
	}
	if (playerPos.y - currentPosition.y < 0)
	{
		if (facingAngle < 23)
		{
			return Vector2(1, 0);
		}
		else if (facingAngle < 68)
		{
			return Vector2(1, -1);
		}
		else if (facingAngle < 113)
		{
			return Vector2(0, -1);
		}
		else if (facingAngle < 158)
		{
			return Vector2(-1, -1);
		}
		else if (facingAngle >= 158)
		{
			return Vector2(-1, 0);
		}
	}
	else
	{
		if (facingAngle < 23)
		{
			return Vector2(1, 0);
		}
		else if (facingAngle < 68)
		{
			return Vector2(1, 1);
		}
		else if (facingAngle < 113)
		{
			return Vector2(0, 1);
		}
		else if (facingAngle < 158)
		{
			return Vector2(-1, 1);
		}
		else if (facingAngle >= 158)
		{
			return Vector2(-1, 0);
		}
	}
	return Vector2(0, 0);
}

//std::vector<Vector2> Enemy::RayDirectionQueueInsert(const Vector2& currentPosition)
//{
//	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
//	std::vector<Vector2> rayDirectionQueue;
//	Vector2 pos = currentPosition;
//	Vector2 playerPos = level->GetPlayerPosition();
//	while (pos != playerPos)
//	{
//		Vector2 faceDirction = FacingDirection(pos);
//		pos = pos + faceDirction;
//		rayDirectionQueue.emplace_back(pos);
//	}
//	return rayDirectionQueue;
//}

