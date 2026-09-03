#include "Target.h"

using namespace Craft;
Target::Target(const Vector2& position)
	: super(L"T", position, Color::White)
{
	sortingOrder = 3;
	sightDegree = 180;
	sightRange = 50;
	range = 10;
	moveSpeed = 20.f;




	// 충돌 가능 객체
	SetColiisionEnabled(true);
}

Target::~Target()
{
	isDead = true;
}

void Target::BeAttacked(const Vector2& face, int damage)
{
	// 무적시간
	if (!invincibilityTimer.IsTimeOut())
	{
		return;
	}
	// 체력 감소
	this->hp -= damage;
	// 넉백 
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(GetOwner());
	if (level->CanMove(GetPosition() + face))
	{
		SetPosition(GetPosition() + face);
	}

	// 체력 0 이하
	if (this->hp <= 0)
	{
		// 타겟이 휘두른 칼 삭제 
		swordSet.clear();
		// 타겟 소멸
		Destroy();
	}
	invincibilityTimer.Reset();
}

void Target::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	invincibilityTimer.Tick(deltaTime);
	if (found)
	{
		SetFace(FacingDirection(GetPosition()));
	}
}