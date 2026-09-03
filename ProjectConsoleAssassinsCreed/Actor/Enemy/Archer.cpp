#include "Archer.h"

#include <Level/GameLevel.h>
#include <Actor/Arrow.h>
#include <Util/Bresenham.h>


using namespace Craft; 

Archer::Archer(const Vector2& position)
	:super(L"A", position, Color::Cyan)
{
	//도망 범위
	runRange = 5.0f;
	//공격 범위
	range = 14;
	sightRange = 20;
	moveSpeed = 5.0f;
	sortingOrder = 3;
	castDelay = 1.0f;
	attackDelay = 0.5f;

	face = Vector2::Left;
	//충돌 허용
	SetColiisionEnabled(true);

}

void Archer::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
	delay.Tick(deltaTime);
	invincibilityTimer.Tick(deltaTime);

	if (invincibilityTimer.IsTimeOut())
	{
		if (this->color == (Color::Red))
		{
			this->SetColor(Color::Cyan);
		}
	}

	if (found)
	{
		sightRange = 35.f;
		sightDegree = 90;
		SetMoveSpeed(5.0f);
	}
	else
	{

		sightRange = 18.f;
		sightDegree = 45;
	}

	if (!sleep && found
		&& doneAttack && !doAttack)
	{
		if (pathDirection.size())
		{
			pathDirection.clear();
		}
		pathDirection = FindRoute(level->GetPlayerPosition());
	}


	if (InAttackRange() && !isWall && distance > runRange)
	{
		FacePlayer();
		WillAttack();
		if (pathDirection.size())
		{
			pathDirection.clear();
			moveIndex = 0;
		}
	}
	if (doAttack)
	{
		delay.SetTargetTime(castDelay);
		if (delay.IsTimeOut())
		{
			Attack(range, face, deltaTime);
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


	if (doneAttack && !doAttack || distance < runRange)
	{
		delay.Reset();
		if (distance < runRange)
		{
			Vector2 reverseFace = Vector2(0, 0) - FacingDirection(GetPosition());
			Move(reverseFace, deltaTime);
		}
		else if (moveIndex > 0 && !InAttackRange())
		{
			Move(pathDirection[moveIndex], deltaTime);
		}
	}

}

void Archer::Attack(int range, const Vector2& face, float deltaTime)
{
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
	std::vector<std::vector<int>> map = level->GetMap();
 	Bresenham bresenham(map);
	std::shared_ptr<Level> owner = GetOwner();
	Vector2 pPo = level->GetPlayerPosition();
	std::vector<Vector2> arrowPath = bresenham.BresenhamFinder(distance, GetPosition(), pPo);
	if (owner)
	{
		if (!isWall && arrowPath.size())
		{
			owner->SpawnActor<Arrow>(GetPosition(), arrowPath);
		}
	}
}

void Archer::BeAttacked(const Vector2& face, int damage)
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
	}

	// 체력 0 이하
	if (this->hp <= 0)
	{
		Destroy();
	}
	invincibilityTimer.Reset();
}

void Archer::WillAttack()
{	
	doAttack = true;
}

void Archer::FacePlayer()
{
	face = FacingDirection(GetPosition());
}