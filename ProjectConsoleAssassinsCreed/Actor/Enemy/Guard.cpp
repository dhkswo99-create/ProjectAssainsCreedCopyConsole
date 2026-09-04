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
	hp = 50;
	range = 3;
	SetSightRange(15.0f);
	SetMoveSpeed(8.0f);
	sortingOrder = 10;
	castDelay = 0.6f;
	attackDelay = 0.3f;
	invincibilityTimer.SetTargetTime(0.2f);
	//충돌 허용
	SetColiisionEnabled(true);
	for (int ix = 0; ix < range; ++ix)
	{
		swordRoute.emplace_back();
	}
}

void Guard::Tick(float deltaTime)
{
	super::Tick(deltaTime);
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
	delay.Tick(deltaTime);
	invincibilityTimer.Tick(deltaTime);


	if (invincibilityTimer.IsTimeOut())
	{
		if (this->color == Color::Red)
		{
			this->SetColor(Color::Yellow);
		}
	}


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
	Vector2 currentPos = GetPosition();
	if (owner)
	{
		for (int ix = 0; ix < range && level->CanAttack(currentPos, GetFace()) ; ++ix)
		{
			swordRoute[ix].emplace_back(currentPos = currentPos + GetFace());
			swordSet.emplace_back(
				owner->SpawnActor<Sword>(GetPosition(), swordRoute[ix], weak_from_this(), guardDamage)
			);
			swordRoute[ix].clear();
		}
	}
}

void Guard::BeAttacked(const Vector2& face, int damage)
{
	if(!invincibilityTimer.IsTimeOut())
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
		// 플레이어가 휘두른 칼 삭제 -> 필요한가? 싶긴 함
		swordSet.clear();
		// 플레이어 소멸
		Destroy();
	}
	invincibilityTimer.Reset();
}

void Guard::DestroyWeapon()
{
	swordSet.clear();
}

void Guard::WillAttack()
{
	doAttack = true;
}

void Guard::FacePlayer()
{
	SetFace(FacingDirection(GetPosition()));
}


//void Guard::Calling(const Vector2& spotOfDetection)
//{
//}
//
//void Guard::Call(const Vector2& spotOfDetection)
//{
//}
