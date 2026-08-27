#include "Guard.h"

#define ANGLE 180/3.14

#include <Level/GameLevel.h>
#include <Render/Renderer.h>
#include <Actor/Sword.h>
#include <cmath>

using namespace Craft;

Guard::Guard(const Vector2& position)
	:super(L"G", position, Color::Yellow)
{
	range = 3;
	SetSightRange(15.0f);
	SetMoveSpeed(8.0f);
	sortingOrder = 3;
	castDelay = 0.6f;
	attackDelay = 0.3f;
	SetFace(Vector2::Left);
	//충돌 허용
	SetColiisionEnabled(true);
}

void Guard::Tick(float deltaTime)
{
	super::Tick(deltaTime);
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
	delay.Tick(deltaTime);

	if (found)
	{
		sightRange = 30.f;
		sightDegree = 70;
		SetMoveSpeed(8.0f);
	}
	else
	{

		sightRange = 15.f;
		sightDegree = 45;
	}

	if (!sleep && found
		&& doneAttack && !doAttack)
	{
		pathDirection.clear();
		pathDirection = FindRoute(level->GetPlayerPosition()); //Guard.
	}
	

	if (InAttackRange() && !isWall && !doAttack)
	{
		FacePlayer();
		WillAttack();
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
		if (moveIndex > 0 && !InAttackRange())
		{
			Move(pathDirection[moveIndex], deltaTime);
		}
	}

}

void Guard::Attack(int range, const Vector2&face, float deltaTime)
{
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
	std::shared_ptr<Level> owner = GetOwner();
	if (owner)
	{
		Vector2 	swordPath = position;
		for (int ix = 1; ix <= range && level->CanAttack(swordPath, face); ++ix)
		{
			swordPath.x += face.x;
			swordPath.y += face.y;
			owner->SpawnActor<Sword>(swordPath);
		}
	}
}

void Guard::WillAttack()
{
	doAttack = true;
}

void Guard::FacePlayer()
{
	face = FacingDirection(GetPosition());
}


//void Guard::Calling(const Vector2& spotOfDetection)
//{
//}
//
//void Guard::Call(const Vector2& spotOfDetection)
//{
//}
