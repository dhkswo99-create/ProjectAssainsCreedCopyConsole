#include "Archer.h"

#include <Level/GameLevel.h>
#include <Actor/Arrow.h>
#include <Util/Bresenham.h>

using namespace Craft; 

Archer::Archer(const Vector2& position)
	:super(L"A", position, Color::Cyan)
{
	//도망 범위
	runRange = 4.0f;
	//공격 범위
	range = 14;
	sightRange = 18;
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


	if (doneAttack && !doAttack)
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
	Bresenham bresenham(level->GetMap());
	std::shared_ptr<Level> owner = GetOwner();
	Vector2 pPo = level->GetPlayerPosition();
	std::vector<Vector2> arrowPath = bresenham.BresenhamFinder(GetPosition(), pPo);
	if (owner)
	{
		if (!isWall)
		{
			owner->SpawnActor<Arrow>(GetPosition(), arrowPath);
		}
	}
}

void Archer::WillAttack()
{
	doAttack = true;
}

void Archer::FacePlayer()
{
	face = FacingDirection(GetPosition());
}